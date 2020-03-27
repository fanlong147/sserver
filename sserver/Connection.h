#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>
class CConnection {
public:
	CConnection(int nDomain = AF_INET, int nType = SOCK_STREAM, int nProtocal = 0);
	CConnection(int nSocketFD);
	~CConnection();
	
	inline void SetSocketFD(int socketFD) { m_socketFD = socketFD; }
	inline int GetSocketFD() { return m_socketFD; }

	void SetTimeOut(int nSec, int nUsec);

	int Connect(std::string strIP, int nPort);
	
	int SetNoBlock(bool bFlag = true);	//true：设置， false:取消设置
	int SetNoDelay(bool bFlag = true);	//true：设置， false:取消设置

	int Send(const char * szBuf, int nSize);
	int Recv(char * szBuf, int nSize);

	int Close();

	bool operator==(const CConnection& conn) { return m_socketFD == conn.m_socketFD; }
	bool operator<(const CConnection& conn) { return m_socketFD < conn.m_socketFD; }

private:
	int m_socketFD;
	int m_nDomain;		//协议族
	int m_nType;		//数据包类型 SOCK_STREA-->TCP, SOCK_DGRAM-->UDP
	int m_nProtocal;
	struct timeval m_timeVal;
	bool m_bResetTimeout;	//重置超时时间标志
};
#endif
