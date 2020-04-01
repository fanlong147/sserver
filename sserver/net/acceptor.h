#ifndef __SSERVER_NET_ACCEPTOR_H__
#define __SSERVER_NET_ACCEPTOR_H__

#include "sock_stream.h"

#include <string>

using std::string;

//当前仅支持IPV4
class Acceptor {
public:
	Acceptor();
	~Acceptor();

	Acceptor(const Acceptor& rhs) = delete;
	Acceptor& operator=(const Acceptor& rhs) = delete;

	inline int GetListenFd() const { return listen_fd_; }

	//打开监听
	int Open(const string & listen_ip, int listen_port, bool is_reuseaddr = true);

	int Accept(SockStream & sock_stream);

	int Close();
private:
	int listen_fd_;
	
	int sock_family_;
	int sock_type_;
	int sock_protocal_;
};

#endif
