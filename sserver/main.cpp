#include "ConnectionEvent.h"
#include "AcceptorEvent.h"
#include "EpollLoop.h"

int main()
{
	CEventLoop* loop = new CEpollLoop();
	CAcceptorEvent* pAcceptor = new CAcceptorEvent(loop);
	if (0 > pAcceptor->Open("0.0.0.0", 62111))
	{
		printf("listen failed\n");
		return 1;
	}
	loop->Run();

}