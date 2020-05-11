#ifndef __SSERVER_STREAM_HANDLER_H__
#define __SSERVER_STREAM_HANDLER_H__

#include "net/event_handler.h"
#include "net/sock_stream.h"
#include "net/event_loop.h"

class StreamEvent : public EventHandler {
public:
	StreamEvent();
	~StreamEvent();
	int Init(SockStream * sock_stream, EventLoop * loop);

	int HandleInput(int fd);

	int HandleOutput(int fd);

	int HandleClose(int fd, EventMask event_mask);

private:

private:
	SockStream * local_stream_;
	SockStream * remote_stream_;
	EventLoop * loop_;
};
#endif
