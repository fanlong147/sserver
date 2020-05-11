#ifndef __SSERVER_NET_SOCK_STREAM_H__
#define __SSERVER_NET_SOCK_STREAM_H__

#include <string>
using std::string;

//仅支持IPV4的链接
class SockStream {
public:
	SockStream();
	~SockStream();

	SockStream(const SockStream& rhs) = delete;
	SockStream& operator=(const SockStream& rhs) = delete;

	int CreateSocket();	//创建Socket

	void SetSockFd(int sock_fd = -1);
	int GetSockFd() const { return sock_fd_; }

	int SetTimeout(long second, long u_second);

	int Connect(string& remote_ip, int remote_port);

	int SetNoBlock(bool flag = true);		//true：设置， false:取消设置
	int SetNoDelay(bool flag = true);		//true：设置， false:取消设置
	int SetKeepAlive(bool flag = true);		//true：设置， false:取消设置 
//	int SetSockOpt();

	int Send(const char * data_buf, int num_bytes, int flag = 0);
	int Recv(char * data_buf, int num_bytes, int flag = 0);

	int Close();

private:
	int sock_fd_;				//socket文件描述符

	int sock_family_;				//地址族
	int sock_type_;					//套接字类型
	int sock_protocal_;				//协议类型
};

#endif
