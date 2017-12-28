#include "basis_socket.h"
#include "basis_event_loop.h"
#include "basis_socket_io.h"
#include "basis_atomic.h"

using namespace basis;

static int32 the_count = 0;

// can write
class WriteableCbk : public BSWriteableCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd, BSFdPartner* partner)
	{
		if (BSSocketIO::write(fd, "abcd", 4) <= 0)
		{
			printf("write error\n");
		}
		printf("send %d abcd.\n", fd);
		el->UnregisterWriteEvent(fd, partner);
		if (++the_count % 10000 == 0)
		{
			printf("process %d the timestamp is %u.\n", the_count, (uint32)time(0));
		}
	}
};

class ReadableCbk : public BSReadableCallBack
{
public:
	virtual void onReadable(BSEventLoop *el, int fd, BSFdPartner* partner)
	{
		char buff[1024] = {0};
		int result = BSSocketIO::read(fd, buff, 1024);
		if (result <= 0)
		{
			el->UnregisterReadEvent(fd, partner);
			printf("socket %d is closed.\n", fd);
			return;
		}
		buff[result] = 0;
		printf("recv remote(%d), buff(%s).\n", fd, buff);
		el->RegisterWriteEvent(fd, partner);
		return;
	}
};

class AcceptableCbk : public BSReadableCallBack
{
public:
	virtual void onReadable(BSEventLoop *el, int fd, BSFdPartner* partner)
	{
		vector<int> sockets;
		sockets.reserve(5);

		if (!BSSocketIO::accept(fd, sockets))
		{
			printf("error accept\n");
			return;
		}
		for (uint32 i = 0; i < sockets.size(); ++i)
		{
			BSFdPartner* fe = new BSFdPartner;
			el->RegisterReadEvent(sockets[i], fe);
			//printf("accept %d.\n", sockets[i]);
		}
	}
};

int main()
{
	BSSocket sock;
	sock.open(st_tcp);
	sock.set_nonblock(true);
	BSSockAddr addr = BSSockAddr::make_sock_addr("", 8899);
	if (!BSSocketIO::listen(sock, addr, 5))
	{
		printf("listen error\n");
		return -1;
	}
	printf("listen success.\n");

	BSEventLoop* el = BSEventLoop::CreateEventLoop(2000);
	if (el)
	{
		AcceptableCbk accept_cbk;
		ReadableCbk readable_cbk;
		WriteableCbk writeable_cbk;

		el->SetAcceptCallBack(&accept_cbk);
		el->SetReadableCallBack(&readable_cbk);
		el->SetWriteableCallBack(&writeable_cbk);

		BSFdPartner* fe = new BSFdPartner;
		el->RegisterAcceptEvent(sock.detach(), fe);

		el->RunLoop();
	}
	return 0;
}
