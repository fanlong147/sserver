#include "AcceptorEvent.h"
#include "ConnectionEvent.h"
int CAcceptorEvent::Open(const std::string& strIP, int nPort)
{
	if (0 > m_Acceptor.Open(strIP, nPort))
	{
		printf("open %s:%d failed, errno:%d\n", strIP.c_str(), nPort, errno);
		return -1;
	}
	m_pLoop->AddEvent(m_Acceptor.GetSocketFD(),this, EPOLLIN);
	return 0;
}

int CAcceptorEvent::HandleInput(int fd)
{
	CConnection* pConn = new CConnection();
	pConn->SetNoBlock();
	pConn->SetNoDelay();

	if (-1 == m_Acceptor.Accept(pConn))
	{
		delete pConn;
		return -1;
	}
	
	CConnectionEvent* pConnEvent = new CConnectionEvent(pConn, m_pLoop);
	if (0 > pConnEvent->AddEvent(pConn->GetSocketFD(), EPOLLIN))
		delete pConnEvent;

	return 0;
}
