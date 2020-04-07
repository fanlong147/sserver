#ifndef __SSERVER_NET_EVENT_HANDLER_H__
#define __SSERVER_NET_EVENT_HANDLER_H__

//�ݶ�������
enum EventMask
{
	kNoneMask = 0x00,
	kReadMask = 0x01,
	kWriteMask = 0x02,
	kRWMask = kReadMask | kWriteMask,
	kAllMask = 0xFF
};

//�¼�����ӿ���
class EventHandler {
public:
	EventHandler();
	virtual ~EventHandler();

	virtual int HandleInput(int fd) = 0;
	virtual int HandleOutput(int fd) = 0;

	virtual int HandleClose(int fd, EventMask event_mask) = 0;
private:

};

#endif
