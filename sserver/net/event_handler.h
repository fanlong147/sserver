#ifndef __SSERVER_NET_EVENT_HANDLER_H__
#define __SSERVER_NET_EVENT_HANDLER_H__

#include "poller.h"

//�¼�����ӿ���
class EventHandler {
public:
	EventHandler();
	virtual ~EventHandler();

	virtual int HandleInput(int fd) = 0;
	virtual int HandleOutput(int fd) = 0;

	virtual int HandleClose(int fd, EventMask event_mask) = 0;
private:

};

#endif
