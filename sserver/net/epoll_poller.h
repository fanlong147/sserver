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
	struct epoll_event TransEvent(int fd, EventMask event);		//�¼�ת�����������д��־ת��Ϊepoll��ʶ��Ķ�д��־
	bool IsValidEvent(struct epoll_event & ep_event);				//��֤�¼���Ч��

private:
	int epoll_fd_;
	const int kDefaultNumEvents_ = 1024;			//Ĭ�ϴ�����ļ�����������������ʱ����			
	std::vector<struct epoll_event> epoll_events_;		//������(����)�¼���fd����
};

#endif
