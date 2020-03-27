#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__
#include "EventHandler.h"
#include "Connection.h"
class CEventLoop {
public:
	CEventLoop() {}
	virtual ~CEventLoop() {}

	virtual int AddEvent(int fd, CEventHandler* pHandler, int nMode) = 0;

	virtual int DelEvent(int fd, CEventHandler* pHandler, int nMode) = 0;

	virtual int ModifyEvent(int fd, CEventHandler* pHandler, int nMode) = 0;

	virtual int Run() = 0;

	virtual int Stop() = 0;
private:

};
#endif
