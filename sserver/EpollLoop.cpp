#include "EpollLoop.h"
#include <stdio.h>

CEpollLoop::CEpollLoop():CEventLoop(),m_nEpollFD(-1)
{
	m_vecEvents.resize(Iinit_Max_Event_Num);
	m_nEpollFD = epoll_create1(EPOLL_CLOEXEC);
	if (0 > m_nEpollFD)
	{
		//此处应抛异常，后续补充
		printf("create epoll fd error\n");
		m_nEpollFD = -1;
	}
}

CEpollLoop::~CEpollLoop()
{
	if (0 > m_nEpollFD)
	{
		close(m_nEpollFD);
	}
}

int CEpollLoop::AddEvent(int fd, CEventHandler* pHandler, int nMode)
{
	if (m_mapHandleEvents.find(fd) != m_mapHandleEvents.end())
	{
		return -1;
	}

	struct epoll_event Event;
	Event.events = nMode;
	Event.data.fd = fd;
	int nRes = epoll_ctl(m_nEpollFD, EPOLL_CTL_ADD, fd, &Event);
	if (0 > nRes)
		return -1;
	m_mapHandleEvents[fd] = pHandler;
	return 0;
}

int CEpollLoop::ModifyEvent(int fd, CEventHandler* pHandler, int nMode)
{
	struct epoll_event Event;
	Event.events = nMode;
	Event.data.fd = fd;

	int nRes = epoll_ctl(m_nEpollFD, EPOLL_CTL_MOD, fd, &Event);
	if (0 > nRes)
		return -1;
	return 0;
}

int CEpollLoop::DelEvent(int fd, CEventHandler* pHandler, int nMode)
{
	struct epoll_event Event;
	Event.events = nMode;
	Event.data.fd = fd;

	int nRes = epoll_ctl(m_nEpollFD, EPOLL_CTL_DEL, fd, &Event);
	m_mapHandleEvents.erase(fd);
	if (0 > nRes)
		return -1;
	return 0;
}

int CEpollLoop::Run()
{
	int nErrNum = 0;
	while (true)
	{
		int nNum = epoll_wait(m_nEpollFD, &*m_vecEvents.begin(), m_vecEvents.size(), 1000);
		if (nNum < 0)
		{
			++nErrNum;
			printf("epoll_wait error %d times,errno:%d\n",nErrNum, errno);
			if (nErrNum > 100)
				return -1;
			else
				continue;
		}
		nErrNum = 0;
		for (int i = 0; i != nNum; ++i)
		{
			printf("size:%d, events:%d\n", m_mapHandleEvents.size(), m_vecEvents[i].events);
			int nFD = m_vecEvents[i].data.fd;

			if (m_mapHandleEvents.find(nFD) == m_mapHandleEvents.end())
			{
				printf("error, can not store fd:%d\n", nFD);
				continue;
			}
			CEventHandler* handler = m_mapHandleEvents[nFD];
			if (m_vecEvents[i].events & EPOLLIN)
			{
				//简单调用读函数
				if (-1 == handler->HandleInput(nFD))
				{
//					DelEvent(nFD, m_mapHandleEvents[nFD], 0);
					handler->HandleClose(nFD);
					continue;
				}

			}
			
			if (m_vecEvents[i].events & EPOLLOUT)
			{
				//简单调用写函数
				if (-1 == handler->HandleOutput(nFD))
				{
//					DelEvent(nFD, m_mapHandleEvents[nFD], 0);
					handler->HandleClose(nFD);
					continue;
				}
			}
		}
	}
	return 0;
}

int CEpollLoop::Stop()
{
	int nRes = close(m_nEpollFD);
	if (0 > nRes)
		return -1;
	return 0;
}