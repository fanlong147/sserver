#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

class CEventHandler {
public:
	CEventHandler() {}
	virtual ~CEventHandler() {}

	//����ʱ��
	virtual int HandleInput(int fd = -1) {}

	virtual int HandleOutput(int fd = -1) {}
	//����ʱ
	virtual int HandleTimeout(int fd = -1) {}

	virtual int HandleClose(int fd = -1) {}

private:

};

#endif
