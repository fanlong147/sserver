#include "event_loop.h"
#include "epoll_poller.h"

#include <assert.h>

EventLoop::EventLoop():poller_(NULL)
{
	poller_ = new EpollPoller();
	assert(poller_);
}

EventLoop::~EventLoop()
{
	if (poller_)
	{
		delete poller_;
		poller_ = NULL;
	}
}

int EventLoop::Register(int fd, EventHandler * handler_ptr, EventMask mask)
{
	auto result = fd_handlers_.find(fd);
	if (result == fd_handlers_.end())
	{
		if (0 > poller_->AddEvent(fd, mask))
			return -1;

		fd_handlers_[fd].events = mask;
		fd_handlers_[fd].handler_ptr = handler_ptr;
	}

	//ͬһ��fd�Ĵ�����������ͬ����������ɾ��ԭ�д�������������ע��
	if (result->second.handler_ptr != handler_ptr)
	{
		return -1;
	}

	EventMask change_mask = (EventMask)(result->second.events | mask);
	if (result->second.events != change_mask)
	{
		result->second.events = change_mask;
		return poller_->ModifyEvent(fd, change_mask);
	}
	return 0;
}

int EventLoop::UnRegister(int fd, EventHandler * handler_ptr, EventMask mask)
{
	
}

int EventLoop::Loop()
{

}

int EventLoop::CancelLoop()
{

}