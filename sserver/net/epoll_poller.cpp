#include "epoll_poller.h"

#include <sys/epoll.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

EpollPoller::EpollPoller():epoll_fd_(-1)
{
	epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
	assert(0 <= epoll_fd_);		//必须保证不出错
	epoll_events_.resize(kDefaultNumEvents_);
}

EpollPoller::~EpollPoller()
{
	close(epoll_fd_);
}

int EpollPoller::Poll(std::map<int, EventMask>& fd_event_map, int timeout_millisecond)
{
	
	int num_fds = epoll_wait(epoll_fd_, &epoll_events_[0], epoll_events_.size(), timeout_millisecond);
	if (0 > num_fds)
		return -1;

	//当前仅支持读写
	for (int i = 0; i != num_fds; ++i)
	{
		//epoll事件转换为程序事件，当前仅支持读写
		auto& ep_event = epoll_events_[i];
		int event = kNoneMask;
		event = (ep_event.events | EPOLLIN ? (event | kReadMask) : event);
		event = (ep_event.events | EPOLLOUT ? (event | kWriteMask) : event);
		fd_event_map[ep_event.data.fd] = (EventMask)event;
	}
	
	if (num_fds == epoll_events_.size())
		epoll_events_.resize(2 * epoll_events_.size());

	return num_fds;
}

int EpollPoller::AddEvent(int fd, EventMask event)
{
	struct epoll_event ep_event = TransEvent(fd, event);
	if (!IsValidEvent(ep_event))
		return -1;
	
	return (0 > epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ep_event) ? -1 : 0);
		
}

int EpollPoller::ModifyEvent(int fd, EventMask event)
{
	
	struct epoll_event ep_event = TransEvent(fd, event);
	if (!IsValidEvent(ep_event))
		return -1;

	return (0 > epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ep_event) ? -1 : 0);
}

int EpollPoller::DelEvent(int fd, EventMask event)
{
	struct epoll_event ep_event = TransEvent(fd, event);
	if (!IsValidEvent(ep_event))
		return -1;

	return (0 > epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ep_event) ? -1 : 0);
}

struct epoll_event EpollPoller::TransEvent(int fd, EventMask event)
{
	struct epoll_event ep_event;
	ep_event.data.fd = fd;

	//当前仅支持读写
	if (event | kReadMask)
	{
		ep_event.events |= EPOLLIN;
	}
	if (event | kWriteMask)
	{
		ep_event.events |= EPOLLOUT;
	}
	return ep_event;
}

bool EpollPoller::IsValidEvent(struct epoll_event& ep_event)
{
	return (ep_event.data.fd > 0 && ep_event.events > 0);
}