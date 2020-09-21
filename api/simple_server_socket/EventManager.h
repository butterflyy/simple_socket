#pragma once
#include <common/utils.h>
#include <list>
#include "whsarmserver.h" //for callback


enum EventType
{
	EVENT_CONNECT,
	EVENT_DISCONNECT,
	EVENT_ERROR,
	EVENT_RECV_FRAME,
};

struct EventData{
	EventType type;
	SS_SESSION session;
	union
	{
		struct{
			SS_SERVER server;
			char client_ip[50];
			int client_port;
		}client;

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

	void SetCallback(ss_connected_callback on_connected,
		ss_disconnected_callback on_disconnected,
		ss_error_callback on_error,
		ss_recvframe_callback on_recvframe);

private:
	void AddThreadEvent(EventThread* thead);

	void ClearThreadEvent();
public:
	std::list<EventThread*> _events;
	utils::Mutex _mutex;

	ss_connected_callback _on_connected;
	ss_disconnected_callback _on_disconnected;
	ss_error_callback _on_error;
	ss_recvframe_callback _on_recvframe;
};


inline void EventManager::SetCallback(ss_connected_callback on_connected,
	ss_disconnected_callback on_disconnected,
	ss_error_callback on_error,
	ss_recvframe_callback on_recvframe){
	_on_connected = on_connected;
	_on_disconnected = on_disconnected;
	_on_error = on_error;
	_on_recvframe = on_recvframe;
}


extern EventManager* EVENT;

