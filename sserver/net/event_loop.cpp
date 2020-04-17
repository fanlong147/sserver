#include "event_loop.h"
#include "epoll_poller.h"

#include <assert.h>

EventLoop::EventLoop():poller_(NULL),cancel_loop_(false)
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
	for(auto iter : fd_handlers_)
	{
		if (iter->second.handler_ptr)
		{
			delete iter->second.handler_ptr;
		}
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

	//同一个fd的处理器必须相同，否则需先删除原有处理器，再重新注册
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

int EventLoop::UnRegister(int fd, EventHandler * handler_ptr, EventMask mask, bool is_delete_handler /* = false */)
{
	auto result = fd_handlers_.find(fd);
	if (result == fd_handlers_.end())
	{
		return 0;
	}

	EventMask change_mask = (EventMask)(result->second.events & ~mask);
	if (change_mask == kNoneMask)
	{
		return UnRegisterAll(fd, is_delete_handler);
	}

	if (result->second.handler_ptr != handler_ptr)
	{
		return -1;
	}
	
	if (result->second.events != change_mask)
	{
		result->second.events = change_mask;
		return poller_->ModifyEvent(fd, change_mask);
	}
	return 0;
}

int EventLoop::UnRegisterAll(int fd, bool is_delete_handler /* = false */)
{
	auto result = fd_handlers_.find(fd);
	if (result == fd_handlers_.end())
	{
		if (is_delete_handler)	//需要删除handler 但是无此handler，需告知客户
		{
			return -1;
		}
		return 0;
	}

	if (is_delete_handler)
	{
		delete result->second.handler_ptr;
		result->second.handler_ptr = NULL;
	}

	return poller_->DelEvent(fd);
}


EventHandler* EventLoop::GetMonitorHandler(int fd) const
{
	auto result = fd_handlers_.find(fd);
	if (result == fd_handlers_.end())
	{
		return NULL;
	}
	return result->second.handler_ptr;
}
int EventLoop::Loop()
{
	std::map<int, EventMask> result;
	while (!cancel_loop_)
	{
		int fd_counts = poller_->Poll(result, 10);
		if (0 > fd_counts)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		for (auto s : result)
		{
			auto handler_iter = fd_handlers_.find(s.first);
			if (handler_iter == fd_handlers_.end())
			{
				printf("no find handler for fd:%d\n", s.first);
				continue;
			}
			if (s.second & kReadMask)
			{
				if (!(handler_iter->second.events & kReadMask))
				{
					printf("error event\n");
					handler_iter->second.handler_ptr->HandleClose(s.first, kAllMask);
				}
				if (0 > handler_iter->second.handler_ptr->HandleInput(s.first))
				{
					handler_iter->second.handler_ptr->HandleClose(s.first, kAllMask);
					continue;
				}
			}
			if (s.second & kWriteMask)
			{
				if (!(handler_iter->second.events & kWriteMask))
				{
					printf("error event\n");
					handler_iter->second.handler_ptr->HandleClose(s.first, kAllMask);
				}
				if (0 > handler_iter->second.handler_ptr->HandleOutput(s.first))
				{
					handler_iter->second.handler_ptr->HandleClose(s.first,kAllMask);
				}
			}
		}
	}
}

int EventLoop::CancelLoop()
{

}