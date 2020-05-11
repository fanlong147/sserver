#include "stream_event.h"


StreamEvent::StreamEvent() 
	: local_stream_(NULL), remote_stream_(NULL), loop_(NULL)
{
	
}


StreamEvent::~StreamEvent()
{
	if (local_stream_)
	{
		delete local_stream_;
		local_stream_ = NULL;
	}
	if (remote_stream_)
	{
		delete remote_stream_;
		remote_stream_ = NULL;
	}
}

int StreamEvent::Init(SockStream * sock_stream, EventLoop * loop)
{
	local_stream_ = sock_stream;
	loop_ = loop;
	return loop->Register(local_stream_->GetSockFd(), this, kReadMask);
}

int StreamEvent::HandleInput(int fd)
{
	if (!local_stream_)
	{
		return -1;
	}
	char recv_buf[512] = { 0 };
	int recv_nums = local_stream_->Recv(recv_buf, 10);
	if (0 == recv_nums)
	{
		printf("remote closed\n");
		return -1;
	}
	if (0 > recv_nums )
	{
		if (EAGAIN == errno)
		{
			return 0;
		}
		return -1;
	}
	printf("recv:%s\n", recv_buf);
	loop_->Register(fd, this, kWriteMask);
	return 0;
}

int StreamEvent::HandleOutput(int fd)
{
	if (local_stream_)
	{
		local_stream_->Send("hello too", 9);
	}
	loop_->UnRegister(fd, this, kWriteMask);
}

int StreamEvent::HandleClose(int fd, EventMask event_mask)
{
	if (local_stream_)
	{
		loop_->UnRegisterAll(local_stream_->GetSockFd());
		local_stream_->Close();
	}
	if (remote_stream_)
	{
		loop_->UnRegisterAll(remote_stream_->GetSockFd());
		remote_stream_->Close();
	}
	delete this;
	return 0;
}