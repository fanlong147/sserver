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
		Stage_Init = 0,	//��ʼ��
		Stage_Addr,		//��ַ����
		Stage_UDP_ASSOC,	//UDP����
		Stage_DNS,		//DNS�����ɹ�������Զ��
		Stage_Connectting,	//Զ��������
		Stage_Stream		//Զ�����ӽ������ɴ�������
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

	//����ʱ
	virtual int HandleTimeout(int fd = -1) {}

	virtual int HandleClose(int fd = -1) { return destroy(); }

	int destroy();
private:
	int HandleStageInit(char * szBuf, int nSize);		//�����ʼ���׶�����
	int HandleStageAddr(char * szBuf, int nSize);		//����socksѰַ�׶�
	int HandleConnecting(char * szBuf, int nSize);		//����remote������
	int HandleStream(char * szBuf, int nSize);			//���������ӽ׶�
	int CheckSocksHeader(char* szBuf, int nSize);		//����socks5ͷ����Ϣ
	int WriteToSock(char * szBuf, int nSize, CConnection * pConn);	//д����

private:
	//�ַ�������
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
		unsigned int m_nPos;		//��ǰλ��
		std::string m_strBuf;
	};

private:
#define LOCAL_RECV_BUF_SIZE 1024*16
#define REMOTE_RECV_BUF_SIZE 1024*32

	CConnection* m_localConn;		//����sock
	int m_nLocalMode;				//����sockģʽ				
	CConnection* m_remoteConn;		//Զ��sock
	int m_nRemoteMode;				//Զ��sockģʽ
	CEventLoop* m_pLoop;
	StageType m_nStage;
	char m_szLocalRecvBuf[LOCAL_RECV_BUF_SIZE];	//���ؽ�������buf
	char m_szRemoteRecvBuf[REMOTE_RECV_BUF_SIZE];	//Զ�˽�������buf
	CCharBuffer m_bufLocal2Remote;		//���˷�����Զ��buf
	CCharBuffer m_bufRemote2Local;		//Զ�˷���������buf
};
#endif
