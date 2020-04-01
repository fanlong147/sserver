#include "acceptor.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

Acceptor::Acceptor()
	:listen_fd_(-1), sock_family_(AF_INET), sock_type_(SOCK_STREAM), sock_protocal_(IPPROTO_TCP)
{

}

Acceptor::~Acceptor()
{
	Close();
}

int Acceptor::Open(const string & listen_ip, int listen_port, bool is_reuseaddr /* = true */)
{
	listen_fd_ = socket(sock_family_, SOCK_STREAM, sock_protocal_);
	if (0 > listen_fd_)
	{
		listen_fd_ = -1;
		return listen_fd_;
	}
	int flag = is_reuseaddr ? 1 : 0;
	if (0 > setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
	{
		Close();
		listen_fd_ = -1;
		return -1;
	}

	struct sockaddr_in listen_address;
	listen_address.sin_family = sock_family_;
	listen_address.sin_addr.s_addr = inet_addr(listen_ip.c_str());
	listen_address.sin_port = htons(listen_port);
	if ( 0 > bind(listen_fd_, (struct sockaddr *)&listen_address, sizeof(listen_address)))
	{

		Close();
		listen_fd_ = -1;
		return -1;
	}
	
	if (0 > listen(listen_fd_, 1000))		//backlog默认设置1000
	{
		Close();
		listen_fd_ = -1;
		return -1;
	}
	return 0;
}

int Acceptor::Accept(SockStream & sock_stream)
{
	int sock_fd = accept(listen_fd_, NULL, NULL);	//无需在accept时获取对端地址
	if (0 > sock_fd)
	{
		return -1;
	}
	sock_stream.SetSockFd(sock_fd);
	return 0;
}

int Acceptor::Close()
{
	if (-1 == listen_fd_)
		return 0;

	if (0 > close(listen_fd_))
	{
		return -1;
	}
	listen_fd_ = -1;
	return 0;
}