#include "acceptor_event.h"


int AcceptorEvent::Init(const std::string& listen_ip, int listen_port)
{
	if (0 > acceptor_.Open(listen_ip, listen_port))
	{
		return -1;
	}

	return (0 > loop_->Register(acceptor_.GetListenFd(), this, kReadMask) ? -1 : 0);

}

int AcceptorEvent::HandleInput(int fd)
{
	SockStream stream;
	if (0 > acceptor_.Accept(stream))
	{
		printf("acceptor error, errno:%d\n", errno);
		return -1;
	}

}

int AcceptorEvent::HandleClose(int fd, EventMask event_mask)
{

}