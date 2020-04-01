#ifndef __SSERVER_NET_EPOLL_POLLER_H__
#define __SSERVER_NET_EPOLL_POLLER_H__

#include "poller.h"

class EpollPoller : public Poller{
public:
	EpollPoller();
	~EpollPoller();

	int Poll(std::map<int, Event_Mask>& fd_event_map, int timeout_millisecond);

	int AddEvent(int fd, Event_Mask event);		
	int ModifyEvent(int fd, Event_Mask event);
	int DelEvent(int fd);
private:
	int epoll_fd_;
};

#endif
