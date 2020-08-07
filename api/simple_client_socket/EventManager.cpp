#include "EventManager.h"
#include "NetConfig.h" //for glog


class EventThread : public utils::Thread
{
public:
	EventThread(EventManager* manager, const EventData& eventData) :
	_manager(manager),
	_eventData(eventData)
	{
		Thread::start();
	}


	~EventThread()
	{
		Thread::quit();

		if (_eventData.type == EVENT_RECV_FRAME
			&& _eventData.frame.data
			&& _eventData.frame.len > 0){
			SAFE_DELETE_ARRAY(_eventData.frame.data);
		}
	}

	void run() override
	{
		switch (_eventData.type)
		{
		case EVENT_DISCONNECT:
			if (_manager->_on_disconnected)
				_manager->_on_disconnected(_eventData.client);
			break;
		case EVENT_ERROR:
			if (_manager->_on_error)
				_manager->_on_error(_eventData.client, _eventData.error_code);
			break;
		case EVENT_RECV_FRAME:
			if (_manager->_on_disconnected)
				_manager->_on_recvframe(_eventData.client, _eventData.frame.data,
				_eventData.frame.len, _eventData.frame.type);
			break;
		default:
			break;
		}
	}
private:
	EventManager* _manager;
	EventData _eventData;
};




EventManager::EventManager():
_on_disconnected(nullptr),
_on_error(nullptr),
_on_recvframe(nullptr)
{

}

EventManager::~EventManager()
{
	for (std::list<EventThread*>::iterator it = _events.begin();
		it != _events.end();
		++it) {
		if ((*it)->isRunning()) {
			LOG(WARNING) << "Event thread has not released , event = 0x" << std::hex << (*it);
		}
		else {
			SAFE_DELETE(*it);
		}
	}
}

void EventManager::OnCallback(const EventData& eventData)
{
	ClearThreadEvent();

	EventThread* eventThread = new EventThread(this, eventData);

	AddThreadEvent(eventThread);
}

void EventManager::AddThreadEvent(EventThread* thead)
{
	utils::LockGuard<utils::Mutex> lock(_mutex);

	_events.push_back(thead);
}

void EventManager::ClearThreadEvent()
{
	utils::LockGuard<utils::Mutex> lock(_mutex);

	for (std::list<EventThread*>::iterator it = _events.begin();
		it != _events.end();){
		if (!(*it)->isRunning()) {//event over
			delete (*it);
			it = _events.erase(it);
		}
		else {
			it++;
		}
	}
}
