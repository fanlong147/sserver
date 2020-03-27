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
#define Iinit_Max_Event_Num 16	//初始化m_vecEvents大小
public:	
	CEpollLoop();
	virtual ~CEpollLoop();
	
	virtual int AddEvent(int fd, CEventHandler * pHandler, int nMode);
	
	virtual int DelEvent(int fd, CEventHandler* pHandler, int nMode);

	virtual int ModifyEvent(int fd, CEventHandler* pHandler, int nMode);

	virtual int Run();

	virtual int Stop();
private:
	int m_nEpollFD;		// epoll实例描述符
	std::vector<struct epoll_event> m_vecEvents;	//存储epoll事件集合
	std::map<int, CEventHandler *> m_mapHandleEvents;	//保存事件集合
};

#endif
