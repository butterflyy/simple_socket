#pragma once
#include <common/utils.h>
#include <list>
#include "whsarmclient.h" //for callback


enum EventType
{
	EVENT_DISCONNECT,
	EVENT_ERROR,
	EVENT_RECV_FRAME,
};

struct EventData{
	EventType type;
	SC_CLIENT client;
	union
	{
		int error_code;
		struct
		{
			byte* data;
			int len;
			int type;
		}frame;
	};
};

class EventThread;
class EventManager{
public:
	friend class EventThread;

	EventManager();
	~EventManager();

	void OnCallback(const EventData& eventData);

	void SetCallback(sc_disconnected_callback on_disconnected,
		sc_error_callback on_error,
		sc_recvframe_callback on_recvframe);

private:
	void AddThreadEvent(EventThread* thead);

	void ClearThreadEvent();
public:
	std::list<EventThread*> _events;
	utils::Mutex _mutex;

	sc_disconnected_callback _on_disconnected;
	sc_error_callback _on_error;
	sc_recvframe_callback _on_recvframe;
};


inline void EventManager::SetCallback(sc_disconnected_callback on_disconnected,
	sc_error_callback on_error,
	sc_recvframe_callback on_recvframe){
	_on_disconnected = on_disconnected;
	_on_error = on_error;
	_on_recvframe = on_recvframe;
}


extern EventManager* EVENT;

