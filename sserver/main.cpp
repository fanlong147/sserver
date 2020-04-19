#include "net/acceptor.h"
#include "net/epoll_poller.h"
#include "net/event_loop.h"

#include "acceptor_event.h"
int main()
{
	EventLoop loop;
	AcceptorEvent acceptor_event(&loop);

	acceptor_event.Init("0.0.0.0", 62891);

	loop.Loop();

	/*
	Acceptor acceptor;
	if (-1 == acceptor.Open("0.0.0.0", 62111))
	{
		printf("acceptor error, errno:%d\n", errno);
		return -1;
	}
	for (;;)
	{
		printf("begin accept\n");
		SockStream sock_stream;
		if (-1 == acceptor.Accept(sock_stream))
		{
			printf("acceptor error, errno:%d\n", errno);
			return -1;
		}
		sock_stream.SetTimeout(5, 0);
//		sock_stream.SetNoBlock();
		sock_stream.SetNoDelay();

		char rcv_buf[1024] = { 0 };
		int rcv_bytes = sock_stream.Recv(rcv_buf, 1023);
		if (0 > rcv_bytes )
		{
			printf("recv error,errno:%d\n", errno);
			sock_stream.Close();
			return -1;
		}
		else if (0 == rcv_bytes) {
			printf("remote closed\n");
			return -1;
		}

		printf("recv from client:%s,bytes:%d\n", rcv_buf,rcv_bytes);
		int send_bytes = sock_stream.Send("hello!", 6);
		if (0 > send_bytes)
		{
			printf("send bytes error, errno:%d\n", send_bytes);
			return -1;
		}
		printf("close socket:%d\n", sock_stream.GetSockFd());
		sock_stream.Close();
	}
	*/
	/*
	CEventLoop* loop = new CEpollLoop();
	CAcceptorEvent* pAcceptor = new CAcceptorEvent(loop);
	if (0 > pAcceptor->Open("0.0.0.0", 62111))
	{
		printf("listen failed\n");
		return 1;
	}
	loop->Run();
	*/
}