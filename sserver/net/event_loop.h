#ifndef __SSERVER_NET_EVENT_LOOP_H__
#define __SSERVER_NET_EVENT_LOOP_H__

#include "event_handler.h"
#include "poller.h"

#include <map>
#include <memory>

class EventLoop {
public:
	EventLoop();
	~EventLoop();

	int Register(int fd, EventHandler * handler_ptr, EventMask mask);
	int UnRegister(int fd, EventHandler * handler_ptr, EventMask mask, bool is_delete_handler = false);

//	int RegisterAll();
	int UnRegisterAll(int fd, bool is_delete_handler = false);

	//获取监控中的fd对应的处理器，如fd未在监控中，返回NULL
	EventHandler * GetMonitorHandler(int fd) const;

	int Loop();
	int CancelLoop();

private:
	Poller* poller_;

	struct InnerHandler {
		EventMask events;
		EventHandler * handler_ptr;
	};
	std::map<int, InnerHandler> fd_handlers_;

	bool cancel_loop_;
};

#endif
