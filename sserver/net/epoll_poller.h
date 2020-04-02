#ifndef __SSERVER_NET_EPOLL_POLLER_H__
#define __SSERVER_NET_EPOLL_POLLER_H__

#include "poller.h"

#include <vector>

class EpollPoller : public Poller{
public:
	EpollPoller();
	~EpollPoller() override;

	int Poll(std::map<int, EventMask>& fd_event_map, int timeout_millisecond) override final;

	int AddEvent(int fd, EventMask event) override final;		
	int ModifyEvent(int fd, EventMask event) override final;
	int DelEvent(int fd, EventMask event = kAllMask) override final;

private:
	struct epoll_event TransEvent(int fd, EventMask event);		//事件转换，将程序读写标志转换为epoll可识别的读写标志
	bool IsValidEvent(struct epoll_event & ep_event);				//验证事件有效性

private:
	int epoll_fd_;
	const int kDefaultNumEvents_ = 1024;			//默认处理的文件描述符数量，不足时翻倍			
	std::vector<struct epoll_event> epoll_events_;		//待处理(触发)事件的fd集合
};

#endif
