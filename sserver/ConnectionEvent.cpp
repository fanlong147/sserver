#include "ConnectionEvent.h"
#include <algorithm>
#include <vector>
#include <netdb.h>
CConnectionEvent::CConnectionEvent(CConnection* pConn /* = NULL */, CEventLoop* pLoop /* = NULL */)
	:m_localConn(pConn),m_nLocalMode(0), m_remoteConn(NULL),m_nRemoteMode(0), m_pLoop(pLoop),m_nStage(Stage_Init)
{

}

CConnectionEvent::~CConnectionEvent()
{
//	destroy();
}

int CConnectionEvent::HandleInput(int fd)
{
	printf("handle input fd:%d\n", fd);
	if (m_localConn && fd == m_localConn->GetSocketFD())
	{
		int nSize = m_localConn->Recv(m_szLocalRecvBuf,LOCAL_RECV_BUF_SIZE);
		if (0 > nSize)
		{
			if (errno == EAGAIN)
				return 0;
			return -1;
		}
		else if(0 == nSize)
		{
			printf("local closed\n");
			return -1;
		}
		
		switch (m_nStage)
		{
		case CConnectionEvent::Stage_Init:
			printf("current stage:Stage_Init,recv:%d bytes\n", nSize);
			return HandleStageInit(m_szLocalRecvBuf, nSize);
			break;
		case CConnectionEvent::Stage_Addr:
			printf("current stage:Stage_Addr,recv:%d bytes\n", nSize);
			return HandleStageAddr(m_szLocalRecvBuf, nSize);
			break;
		case CConnectionEvent::Stage_UDP_ASSOC:		//暂不支持
			return -1;
			break;
		case CConnectionEvent::Stage_DNS:		//暂不支持
			return -1;		
			break;
		case CConnectionEvent::Stage_Connectting:
			printf("current stage:Stage_Connectting,recv:%d bytes\n",nSize);
			return HandleConnecting(m_szLocalRecvBuf, nSize);
			break;
		case CConnectionEvent::Stage_Stream:
			printf("current stage:Stage_Stream,recv:%d bytes\n",nSize);
			return HandleStream(m_szLocalRecvBuf, nSize);
			break;
		default:
			return -1;
			break;
		}
	}else if (m_remoteConn && fd == m_remoteConn->GetSocketFD())
	{
		int nSize = m_remoteConn->Recv(m_szRemoteRecvBuf, REMOTE_RECV_BUF_SIZE);
		if (0 > nSize)
		{
			if (errno == EAGAIN)
				return 0;
			return -1;
		}
		else if (0 == nSize)
		{
			printf("remote closed\n");
			return -1;
		}
		printf("Remote recv %d bytes\n", nSize);
		return WriteToSock(m_szRemoteRecvBuf, nSize, m_localConn);
	}

}

int CConnectionEvent::HandleOutput(int fd /* = -1 */)
{
	printf("handle output fd:%d\n", fd);
	if (m_localConn && fd == m_localConn->GetSocketFD())
	{
		return WriteToSock(NULL, 0, m_localConn);
	}
	else if (m_remoteConn && fd == m_remoteConn->GetSocketFD())
	{
		if (Stage_Connectting == m_nStage)
		{
			m_nStage = Stage_Stream;
			m_nRemoteMode |= EPOLLIN;
			m_pLoop->ModifyEvent(fd, this, m_nRemoteMode);
			printf("stage changed to Stage_Stream\n");
		}
		
		return WriteToSock(NULL, 0, m_remoteConn);
	}
	printf("handle output error\n");
	return -1;
}

int CConnectionEvent::HandleStageInit(char* szBuf, int nSize)
{
	char cCheckRes = CheckSocksHeader(szBuf, nSize);
	if (-1 == cCheckRes)
	{
		return -1;
	}
	if (0xFF == cCheckRes)
	{
		char returnBuf[] = {0x05,0x01};
		WriteToSock(returnBuf, sizeof(returnBuf), m_localConn);
		return -1;

	}else if (0x00 == cCheckRes)
	{
		char returnBuf[] = { 0x05,0x00 };
		if (0 == WriteToSock(returnBuf, sizeof(returnBuf), m_localConn))
		{
			m_nStage = Stage_Addr;
			return 0;
		}
		return -1;
	}
	return -1;
}

int CConnectionEvent::HandleStageAddr(char* szBuf, int nSize)
{
	if (4 > nSize || szBuf[0] != 0x05 || szBuf[1] != 0x01)
	{
		printf("socks head for connect is false\n");
		return -1;
	}
	
	szBuf += 3;
	nSize -= 3;
	std::string strRemoteAddr;
	unsigned short nPort;

	if (szBuf[0] | 0x01)	//IPV4
	{
		if (nSize < 7)
			return -1;
		char ip[16] = { 0 };
		snprintf(ip, 16, "%hhu.%hhu.%hhu.%hhu", szBuf[1], szBuf[2], szBuf[3], szBuf[4]);
		strRemoteAddr.assign(ip);
		nPort = ntohs(*(unsigned short*)&szBuf[5]);
		printf("RemoteAddress: %s:%d\n", strRemoteAddr.c_str(), nPort);

		if (m_remoteConn)
		{
			printf("remote conn already exist\n");
			return -1;
		}
		m_bufLocal2Remote.Append(&szBuf[7], nSize - 7);
	}else if (szBuf[0] | 0x03)	//域名
	{
		char domainLength = szBuf[1];
		strRemoteAddr.assign(szBuf[2], domainLength);
		unsigned short* pPort = (unsigned short*)szBuf[2 + domainLength];
		nPort = ntohs(*pPort);
		m_nStage = Stage_DNS;
		struct hostent* Hptr = gethostbyname(strRemoteAddr.c_str());
		if (NULL == Hptr)
		{
			printf("error get hostent for:%s, errmsg:%s\n",strRemoteAddr.c_str(), hstrerror(h_errno));
			return -1;
		}
		if (AF_INET != Hptr->h_addrtype)
		{
			printf("unknown addrtype for domain:%s,type:%d\n", strRemoteAddr.c_str(), Hptr->h_addrtype);
			return -1;
		}
		if (NULL == *Hptr->h_addr_list)
		{
			printf("have no address\n");	//不存在该种情况，防御性编程
			return -1;				
		}
		strRemoteAddr.assign(Hptr->h_addr_list[0]);	//取第一个有效地址
		m_bufLocal2Remote.Append(&szBuf[domainLength + 1 + 1 + 2], nSize - (domainLength + 1 + 1 + 2));
		
	}else if (szBuf[0] | 0x04)	//IPV6
	{
		printf("not support IPV6\n");
		return -1;
	}
	m_remoteConn = new CConnection();
	m_remoteConn->SetNoBlock();
	m_remoteConn->SetNoDelay();
	if (0 > m_remoteConn->Connect(strRemoteAddr, nPort))
	{
		if (errno != EAGAIN)
		{
			printf("cannot connect to remote addr:%s,errno:%d\n", strRemoteAddr.c_str(), errno);
			return -1;
		}
	}
	m_nRemoteMode |= EPOLLOUT;
	m_pLoop->AddEvent(m_remoteConn->GetSocketFD(), this, m_nRemoteMode);		//远端更新为可写
	m_nStage = Stage_Connectting;	//更新为连接中

	char returnBuf[] = {0x05,0x00,0x00,szBuf[0],0x00,0x00,0x00,0x00,0x10,0x10};
	return WriteToSock(returnBuf, sizeof(returnBuf), m_localConn);
}

int CConnectionEvent::HandleConnecting(char* szBuf, int nSize)
{
	m_bufLocal2Remote.Append(szBuf, nSize);		//远端连接建立中，数据加入缓存
	return 0;
}

int CConnectionEvent::HandleStream(char* szBuf, int nSize)
{
	if (!m_remoteConn)
	{
		return -1;
	}
	return WriteToSock(szBuf, nSize, m_remoteConn);
}

int CConnectionEvent::CheckSocksHeader(char* szBuf, int nSize)
{
	if (nSize < 3)
	{
		printf("Socks Header is too short\n");
		return -1;
	}
	char cVersion = szBuf[0];
	if (0x05 != cVersion)
	{
		printf("Socks Header's version is not 0x05\n");
		return -1;
	}
	char cMethodNum = szBuf[1];
	if (cMethodNum < 1 || cMethodNum != nSize - 2)
	{
		printf("NMETHODS and Number of METHODS is not Matched\n");
		return -1;
	}

	int i = 2;
	for (; i != nSize; ++i)
	{
		if (0x00 == szBuf[i])
			break;
	}
	if (i == nSize)
	{
		printf("NO AUTHENTICATION REQUIRED is Required\n");
		return 0xFF;		//无支持的认证方式
	}
	return 0x00;		//支持无认证连接方式
}

int CConnectionEvent::WriteToSock(char* szBuf, int nSize, CConnection* pConn)
{
	if (*pConn == *m_localConn)
	{
		if (szBuf && nSize)
			m_bufRemote2Local.Append(szBuf, nSize);
		if (!m_bufRemote2Local.Size())
		{
			printf("no data to send to local\n");
			if (m_nLocalMode | EPOLLOUT)
			{
				m_nLocalMode &= ~EPOLLOUT;
				return m_pLoop->ModifyEvent(m_localConn->GetSocketFD(), this, m_nLocalMode);
			}
			return 0;
		}
		int nSize = pConn->Send(m_bufRemote2Local.Begin(), m_bufRemote2Local.Size());
		if (0 > nSize && EAGAIN != errno)
		{
				return -1;
		}
		printf("local send %d bytes\n", nSize);
		m_bufRemote2Local.SetPos(nSize);
		if (0 == m_bufRemote2Local.Size() && m_nLocalMode | EPOLLOUT)
		{
			m_nLocalMode &= ~EPOLLOUT;
			return m_pLoop->ModifyEvent(m_localConn->GetSocketFD(), this, m_nLocalMode);
		}
		return 0;
	}
	else if(*pConn == *m_remoteConn){
		if (szBuf && nSize)
			m_bufLocal2Remote.Append(szBuf, nSize);
		if (!m_bufLocal2Remote.Size())
		{
			printf("no data to send to remote\n");
			if (m_nLocalMode | EPOLLOUT)
			{
				m_nLocalMode &= ~EPOLLOUT;
				return m_pLoop->ModifyEvent(m_remoteConn->GetSocketFD(), this, m_nLocalMode);
			}
			return 0;
		}
		int nSize = pConn->Send(m_bufLocal2Remote.Begin(), m_bufLocal2Remote.Size());
		m_bufLocal2Remote.SetPos(nSize);

		printf("remote send %d bytes\n", nSize);
		if (m_bufLocal2Remote.Size() == 0 && m_nRemoteMode | EPOLLOUT)
		{
			m_nRemoteMode &= ~EPOLLOUT;
			return m_pLoop->ModifyEvent(m_remoteConn->GetSocketFD(), this, m_nRemoteMode);
		}
		return 0;
	}
	return -1;

}

int CConnectionEvent::destroy()
{	
	if (m_localConn)
	{
		m_pLoop->DelEvent(m_localConn->GetSocketFD(), this, 0);
		m_localConn->Close();
		delete m_localConn;
	}
	
	if (m_remoteConn)
	{
		m_pLoop->DelEvent(m_remoteConn->GetSocketFD(), this, 0);
		m_remoteConn->Close();
		delete m_remoteConn;
	}
	delete this;
}

CConnectionEvent::CCharBuffer::CCharBuffer():m_nPos(0)
{
	m_strBuf.reserve(2048);
}

CConnectionEvent::CCharBuffer::~CCharBuffer()
{

}

CConnectionEvent::CCharBuffer & CConnectionEvent::CCharBuffer::Append(const char* szBuf, int nSize)
{
	if (m_nPos > BUFFER_THREASHOLD)
	{
		m_strBuf = m_strBuf.substr(m_nPos, m_strBuf.size());
		m_nPos = 0;
	}
	m_strBuf.append(szBuf, nSize);
}

void CConnectionEvent::CCharBuffer::Clear()
{
	m_strBuf.clear();
	m_nPos = 0;
}

char CConnectionEvent::CCharBuffer::operator[](unsigned int nPos)
{
	return m_strBuf[m_nPos+nPos];
}

char* CConnectionEvent::CCharBuffer::Begin()
{
	return &m_strBuf[m_nPos];
}

int CConnectionEvent::CCharBuffer::Size()
{
	return m_strBuf.size() - m_nPos;
}

void CConnectionEvent::CCharBuffer::SetPos(unsigned int nOffset)
{
	m_nPos += nOffset;
	if (m_nPos >= m_strBuf.size())
	{
		m_nPos = m_strBuf.size();
	}
}