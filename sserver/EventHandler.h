#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

class CEventHandler {
public:
	CEventHandler() {}
	virtual ~CEventHandler() {}

	//处理时间
	virtual int HandleInput(int fd = -1) {}

	virtual int HandleOutput(int fd = -1) {}
	//处理超时
	virtual int HandleTimeout(int fd = -1) {}

	virtual int HandleClose(int fd = -1) {}

private:

};

#endif
