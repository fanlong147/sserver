#ifndef __CONNECTION_EVENT_H__
#define __CONNECTION_EVENT_H__
#include "EventHandler.h"
#include "Connection.h"
#include "EventLoop.h"
#include <sys/epoll.h>
#include <string>

class CConnectionEvent : public CEventHandler {
	enum StageType
	{
		Stage_Init = 0,	//初始化
		Stage_Addr,		//地址解析
		Stage_UDP_ASSOC,	//UDP解析
		Stage_DNS,		//DNS解析成功，连接远端
		Stage_Connectting,	//远端连接中
		Stage_Stream		//远端链接建立，可传输数据
	};
	
public:
	CConnectionEvent(CConnection* pConn, CEventLoop* pLoop = NULL);
	~CConnectionEvent();
	
	int AddEvent(int nFD, int nMode) {
		if (m_localConn && nFD == m_localConn->GetSocketFD() && m_pLoop)
		{
			m_nLocalMode |= nMode;
			return  m_pLoop->AddEvent(nFD, this, m_nLocalMode);
		}else if (m_remoteConn && nFD == m_remoteConn->GetSocketFD() && m_pLoop)
		{
			m_nRemoteMode |= nMode;
			return m_pLoop->AddEvent(nFD, this, m_nRemoteMode);
		}
		else {
			return -1;
		}
	}

	virtual int HandleInput(int fd = -1);

	virtual int HandleOutput(int fd = -1);

	//处理超时
	virtual int HandleTimeout(int fd = -1) {}

	virtual int HandleClose(int fd = -1) { return destroy(); }

	int destroy();
private:
	int HandleStageInit(char * szBuf, int nSize);		//处理初始化阶段任务
	int HandleStageAddr(char * szBuf, int nSize);		//处理socks寻址阶段
	int HandleConnecting(char * szBuf, int nSize);		//处理remote连接中
	int HandleStream(char * szBuf, int nSize);			//处理已连接阶段
	int CheckSocksHeader(char* szBuf, int nSize);		//处理socks5头部信息
	int WriteToSock(char * szBuf, int nSize, CConnection * pConn);	//写数据

private:
	//字符缓冲类
	class CCharBuffer {
#define BUFFER_THREASHOLD 1024*10*10
	public:
		CCharBuffer();
		~CCharBuffer();
		CCharBuffer& Append(const char* szBuf, int nSize);
		void Clear();
		char operator[](unsigned int nPos);
		char* Begin();
		int Size();
		void SetPos(unsigned int nOffset);
	private:
		unsigned int m_nPos;		//当前位置
		std::string m_strBuf;
	};

private:
#define LOCAL_RECV_BUF_SIZE 1024*16
#define REMOTE_RECV_BUF_SIZE 1024*32

	CConnection* m_localConn;		//本端sock
	int m_nLocalMode;				//本端sock模式				
	CConnection* m_remoteConn;		//远端sock
	int m_nRemoteMode;				//远端sock模式
	CEventLoop* m_pLoop;
	StageType m_nStage;
	char m_szLocalRecvBuf[LOCAL_RECV_BUF_SIZE];	//本地接收数据buf
	char m_szRemoteRecvBuf[REMOTE_RECV_BUF_SIZE];	//远端接收数据buf
	CCharBuffer m_bufLocal2Remote;		//本端发送至远端buf
	CCharBuffer m_bufRemote2Local;		//远端发送至本端buf
};
#endif
