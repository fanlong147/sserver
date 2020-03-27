#include "Acceptor.h"
#include <string.h>

CAcceptor::CAcceptor(int nDomain /* = AF_INET */, int nType /* = SOCK_STREAM */, int nProtocal /* = 0 */)
	:m_nListenFD(-1),m_nDomain(nDomain),m_nType(nType),m_nProtocal(nProtocal)
{

}

CAcceptor::~CAcceptor()
{
	if (-1 != m_nListenFD)
		Close();
}

int CAcceptor::Open(const std::string& strIP, int nPort, bool bReuseAddr)
{
	if (-1 != m_nListenFD)
		Close();

	m_nListenFD = socket(m_nDomain, m_nType, m_nProtocal);
	if (0 > m_nListenFD)
	{
		return -1;
	}
	int nOn = bReuseAddr ? 1 : 0;
	if (0 > setsockopt(m_nListenFD, SOL_SOCKET, SO_REUSEADDR, &nOn, sizeof(nOn)))
	{
		return -1;
	}

	struct sockaddr_in listenAddr;
	listenAddr.sin_family = m_nDomain;
	listenAddr.sin_port = htons(nPort);
	listenAddr.sin_addr.s_addr = inet_addr(strIP.c_str());
	
	if (0 > bind(m_nListenFD, (struct sockaddr *)&listenAddr, sizeof(struct sockaddr)))
		return -1;

	if (0 > listen(m_nListenFD, 100))
		return -1;
	return 0;
}

int CAcceptor::Accept(CConnection * pConn)
{
	if (-1 == m_nListenFD)
		return -1;
	struct sockaddr_in remoteAddr;
	bzero(&remoteAddr, sizeof(remoteAddr));
	socklen_t nLen = sizeof(struct sockaddr);
	int nSocketFD = accept(m_nListenFD, (struct sockaddr *)&remoteAddr, &nLen);
	if (0 > nSocketFD)
		return -1;
	pConn->SetSocketFD(nSocketFD);
	return 0;
}

int CAcceptor::Close()
{
	if (-1 == m_nListenFD)
		return 0;
	int nRes = close(m_nListenFD);
	m_nListenFD = -1;
	return nRes;
}