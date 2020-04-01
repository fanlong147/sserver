#include "epoll_poller.h"

#include <sys/epoll.h>
EpollPoller::EpollPoller():epoll_fd_(-1)
{
	epoll_fd_ = epoll_create1();
}