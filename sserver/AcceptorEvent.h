#ifndef __ACCEPTOR_EVENT_H__
#define __ACCEPTOR_EVENT_H__

#include "EventHandler.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include <sys/epoll.h>

class CAcceptorEvent : public CEventHandler {
public:
	CAcceptorEvent(CEventLoop* pLoop):m_pLoop(pLoop) {}
	int Open(const std::string & strIP, int nPort);

	int GetSocketFD() { return m_Acceptor.GetSocketFD(); }

	int HandleInput(int fd);

private:
	CAcceptor m_Acceptor;
	CEventLoop* m_pLoop;
};

#endif
