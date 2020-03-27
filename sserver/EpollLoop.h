#ifndef __EPOLL_LOOP_H__
#define __EPOLL_LOOP_H__

#include "EventLoop.h"
#include "EventHandler.h"
#include "Connection.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <vector>

class CEpollLoop : public CEventLoop {
#define Iinit_Max_Event_Num 16	//��ʼ��m_vecEvents��С
public:	
	CEpollLoop();
	virtual ~CEpollLoop();
	
	virtual int AddEvent(int fd, CEventHandler * pHandler, int nMode);
	
	virtual int DelEvent(int fd, CEventHandler* pHandler, int nMode);

	virtual int ModifyEvent(int fd, CEventHandler* pHandler, int nMode);

	virtual int Run();

	virtual int Stop();
private:
	int m_nEpollFD;		// epollʵ��������
	std::vector<struct epoll_event> m_vecEvents;	//�洢epoll�¼�����
	std::map<int, CEventHandler *> m_mapHandleEvents;	//�����¼�����
};

#endif
