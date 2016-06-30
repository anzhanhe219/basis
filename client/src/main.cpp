#include "basis_socket.h"
#include "basis_event_loop.h"
#include "basis_socket_io.h"

using namespace basis;

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
		//printf("recv remote(%d), buff(%s).\n", fd, buff);
		el->RegisterWriteEvent(fd, partner);
		return;
	}
};

class WriteableCbk : public BSWriteableCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd, BSFdPartner* partner)
	{
		el->UnregisterWriteEvent(fd, partner);
		if (BSSocketIO::write(fd, "abcd", 4) <= 0)
		{
			printf("write error\n");
		}
	}
};

class ConnectedCbk: public BSWriteableCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd, BSFdPartner* partner)
	{
		el->UnregisterConnectedEvent(fd, partner);

		BSSocket sock(fd);
		int err = BSSocket::sock_error(sock);
		if (err != 0)
		{			
			return;
		}
		el->RegisterReadEvent(fd, partner);
		el->RegisterWriteEvent(fd, partner);
		sock.detach();
	}
};

int main()
{
	BSEventLoop* el = BSEventLoop::CreateEventLoop(2000);
	ConnectedCbk connect_cbk;
	WriteableCbk writeable_cbk;
	ReadableCbk readable_cbk;
	el->SetConnctedCallBack(&connect_cbk);
	el->SetWriteableCallBack(&writeable_cbk);
	el->SetReadableCallBack(&readable_cbk);

	for (uint32 i = 0; i < 20; ++i)
	{
		BSSockAddr addr = BSSockAddr::make_sock_addr("127.0.0.1", 8899);
		bool is_success = false;
		int fd = BSSocketIO::connect_asyn(addr, is_success);
		BSFdPartner* fe = new BSFdPartner;
		if (fd > 0 && is_success)
		{
			el->RegisterReadEvent(fd, fe);
			el->RegisterWriteEvent(fd, fe);
		}
		else if(fd > 0)
		{
			el->RegisterConnectedEvent(fd, fe);
		}
	}
	el->RunLoop();


	return 0;
}