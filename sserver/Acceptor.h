#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Connection.h"
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

class CAcceptor {
public:
	CAcceptor(int nDomain = AF_INET, int nType = SOCK_STREAM, int nProtocal = 0);
	~CAcceptor();

	int GetSocketFD() { return m_nListenFD; }

	int Open(const std::string& strIP, int nPort, bool bReuseAddr = true);

	int Accept(CConnection * pConn);

	int Close();
private:
	int m_nListenFD;		//¼àÌý¾ä±ú
	int m_nDomain;
	int m_nType;
	int m_nProtocal;
};

#endif
