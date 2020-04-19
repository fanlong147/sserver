#ifndef __SSERVER_NET_POLLER_H__
#define __SSERVER_NET_POLLER_H__

#include "event_handler.h"
#include <map>

class Poller {
public:
	Poller() = default;
	virtual ~Poller() =default;

	Poller(const Poller&) = delete;
	Poller& operator=(const Poller&) = delete;

	//����>0�����ؽ����ʾ�����¼���������������fd_event_map����Ӧ������->�¼�����
	//����=0����ʾָ��ʱ����û���¼�����
	//ʧ�ܷ���-1
	virtual int Poll(std::map<int,EventMask> & fd_event_map, int timeout_millisecond) = 0;		//�ṩ���뼶����

	virtual int AddEvent(int fd, EventMask event) = 0;			//����¼�
	virtual int ModifyEvent(int fd, EventMask event) = 0;		//�޸��¼�
	virtual int DelEvent(int fd, EventMask event = kAllMask) = 0;							//ɾ���ļ�������
private:

};
#endif
