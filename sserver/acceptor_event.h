#ifndef __SSERVER_ACCEPTOR_EVENT_H__
#define __SSERVER_ACCEPTOR_EVENT_H__

#include "net/acceptor.h"
#include "net/event_loop.h"

#include <string>

class AcceptorEvent : public EventHandler {
public:
	AcceptorEvent(EventLoop * loop):loop_(loop) {}
	~AcceptorEvent() {}

	int Init(const std::string & listen_ip, int listen_port);

	virtual int HandleInput(int fd) override;

	virtual int HandleClose(int fd, EventMask event_mask) override;
private:
	Acceptor acceptor_;
	EventLoop* loop_;
};
#endif
