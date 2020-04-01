#ifndef __SSERVER_NET_POLLER_H__
#define __SSERVER_NET_POLLER_H__

#include <map>

//暂定，够用
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

	//返回>0，返回结果表示触发事件的描述符数量，fd_event_map含对应描述符->事件集合
	//返回=0，表示指定时间内没有事件触发
	//失败返回-1
	virtual int Poll(std::map<int,Event_Mask> & fd_event_map, int timeout_millisecond) = 0;		//提供毫秒级精度

	virtual int AddEvent(int fd, Event_Mask event) = 0;			//添加事件
	virtual int ModifyEvent(int fd, Event_Mask event) = 0;		//修改事件
	virtual int DelEvent(int fd) = 0;							//删除文件描述符
private:

};
#endif
