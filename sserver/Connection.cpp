#include "Connection.h"
#include <string.h>

CConnection::CConnection(int nDomain, int nType, int nProtocal)
	:m_socketFD(-1), m_nDomain(nDomain), m_nType(nType), m_nProtocal(nProtocal), m_timeVal({0,0})
{
	m_bResetTimeout = false;	//默认未重置
}

CConnection::CConnection(int nSocketFD):m_socketFD(nSocketFD)
{

}

CConnection::~CConnection()
{
	if ( -1 == m_socketFD)
		Close();
}

void CConnection::SetTimeOut(int nSec, int nUsec)
{
	m_timeVal.tv_sec = nSec;
	m_timeVal.tv_usec = nUsec;
	m_bResetTimeout = true;
}

int CConnection::Connect(std::string strIP, int nPort)
{
	//已存在有效fd，先关闭
	if (-1 != m_socketFD)
		Close();

	//生成Socket描述符
	m_socketFD = socket(m_nDomain, m_nType, m_nProtocal);
	if (-1 == m_socketFD)
		return -1;

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = m_nDomain;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = inet_addr(strIP.c_str());
	if (m_bResetTimeout)
		if (-1 == setsockopt(m_socketFD, SOL_SOCKET, SO_SNDTIMEO, &m_timeVal, sizeof(m_timeVal)))
			return -1;

	return connect(m_socketFD, (struct sockaddr*) & addr, sizeof(struct sockaddr));
}

int CConnection::Send(const char* szBuf, int nSize)
{
	if (m_bResetTimeout)
		if (-1 == setsockopt(m_socketFD, SOL_SOCKET, SO_SNDTIMEO, &m_timeVal, sizeof(m_timeVal)))
			return -1;
	
	return send(m_socketFD, szBuf, nSize, 0);
}

int CConnection::Recv(char* szBuf, int nSize)
{
	if (m_bResetTimeout)
		if (-1 == setsockopt(m_socketFD, SOL_SOCKET, SO_RCVTIMEO, &m_timeVal, sizeof(m_timeVal)))
			return -1;
	
	return recv(m_socketFD, szBuf, nSize, 0);
}

//true：设置， false:取消设置
int CConnection::SetNoBlock(bool bFlag /* = true */)
{
	if (-1 == m_socketFD)
		return -1;

	int flags = fcntl(m_socketFD, F_GETFL, 0);
	if (0 > flags)
		return -1;
	if (bFlag)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;

	if (0 > fcntl(m_socketFD, F_SETFL, flags))
		return -1;
	return 0;
}

//true：设置, false:取消设置
int CConnection::SetNoDelay(bool bFlag /* = true */)
{
	int on = bFlag ? 1 : 0;
	if (0 > setsockopt(m_socketFD, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)))
		return -1;
	return 0;
}

int CConnection::Close()
{
	if (-1 == m_socketFD)
		return 0;
	int nRes = close(m_socketFD);
	m_socketFD = -1;
	return nRes < 0 ? -1 : 0;
}