#ifndef __SSERVER_NET_POLLER_H__
#define __SSERVER_NET_POLLER_H__

#include <map>

//�ݶ�������
enum Event_Mask
{
	kReadMask = 0x01,
	kWriteMask = 0x02,
	kRWMask = kReadMask | kWriteMask,
	kAllMask = 0xFF
};

class Poller {
public:
	Poller() = default;
	virtual ~Poller() =default;

	Poller(const Poller&) = delete;
	Poller& operator=(const Poller&) = delete;

	//����>0�����ؽ����ʾ�����¼���������������fd_event_map����Ӧ������->�¼�����
	//����=0����ʾָ��ʱ����û���¼�����
	//ʧ�ܷ���-1
	virtual int Poll(std::map<int,Event_Mask> & fd_event_map, int timeout_millisecond) = 0;		//�ṩ���뼶����

	virtual int AddEvent(int fd, Event_Mask event) = 0;			//����¼�
	virtual int ModifyEvent(int fd, Event_Mask event) = 0;		//�޸��¼�
	virtual int DelEvent(int fd) = 0;							//ɾ���ļ�������
private:

};
#endif
