#include "sock_stream.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>		//套接字地址结构
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <netinet/tcp.h>		//套接字选项

#include <string.h>

SockStream::SockStream()
	: sock_fd_(-1), sock_family_(AF_INET), sock_type_(SOCK_STREAM), sock_protocal_(IPPROTO_TCP)
{
	
}

SockStream::~SockStream()
{
	Close();
}

int SockStream::CreateSocket()
{
	//需先关闭已有socket
	if (-1 != sock_fd_)
		return -1;

	sock_fd_ = socket(sock_family_, sock_type_, sock_protocal_);
	if (0 > sock_fd_)
		sock_fd_ = -1;

	return sock_fd_;
}

void SockStream::SetSockFd(int sock_fd /* = -1 */)
{
	if (-1 != sock_fd_)
		Close();
	sock_fd_ = sock_fd;
}

int SockStream::SetTimeout(long second, long u_second)
{
	//无socket，设置失败
	if (-1 == sock_fd_)
	{
		return -1;
	}

	struct timeval timeout_value;
	bzero(&timeout_value, sizeof(timeout_value));
	timeout_value.tv_sec = second;
	timeout_value.tv_usec = u_second;
	if ( (0 > setsockopt(sock_fd_, SOL_SOCKET, SO_SNDTIMEO, &timeout_value, sizeof(timeout_value))) ||
		(0 > setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout_value, sizeof(timeout_value)))
		)
	{
		return -1;
	}
	return 0;
}

int SockStream::Connect(string& remote_ip, int remote_port)
{
	if (-1 == sock_fd_)	
		return -1;
	
	struct sockaddr_in remote_addr;
	bzero(&remote_addr, sizeof(remote_addr));

	remote_addr.sin_family = sock_family_;
	remote_addr.sin_addr.s_addr = inet_addr(remote_ip.c_str());
	remote_addr.sin_port = htons(remote_port);

	if (0 > connect(sock_fd_, (struct sockaddr*) & remote_addr, sizeof(remote_addr)))
	{
		Close();		//链接失败，必须关闭后重新调用CreateSocket函数
		return -1;	
	}
	return 0;
}

int SockStream::SetNoBlock(bool flag /* = true */)
{
	if (-1 == sock_fd_)
		return -1;
	int sock_flag = fcntl(sock_fd_, F_GETFL, 0);
	if (flag)
	{
		sock_flag |= O_NONBLOCK;
	}
	else
	{
		sock_flag &= ~O_NONBLOCK;
	}
	return (0 > fcntl(sock_fd_, F_SETFL, sock_flag) ? -1 : 0);
}

int SockStream::SetNoDelay(bool flag /* = true */)
{
	if (-1 == sock_fd_)
		return -1;
	int sock_flag = flag ? 1 : 0;
	return (0 >setsockopt(sock_fd_, IPPROTO_TCP, TCP_NODELAY, &sock_flag, sizeof(sock_flag)) ? -1 : 0);
}

int SockStream::SetKeepAlive(bool flag /* = true */)
{
	if (-1 == sock_fd_)
		return -1;
	int sock_flag = flag ? 1 : 0;
	return (0 > setsockopt(sock_fd_, SOL_SOCKET, SO_KEEPALIVE, &sock_flag, sizeof(sock_flag)) ? -1 : 0);
}

int SockStream::Send(const char * data_buf, int num_bytes, int flag /* = 0 */)
{
	return send(sock_fd_,data_buf, num_bytes, flag);
}

int SockStream::Recv(char * data_buf, int num_bytes, int flag /* = 0 */)
{
	return recv(sock_fd_, data_buf, num_bytes, flag);
}

int SockStream::Close()
{
	if (-1 == sock_fd_)
		return 0;
	
	if (0 > close(sock_fd_))
		return -1;
	
	sock_fd_ = -1;
	return 0;
	
}

