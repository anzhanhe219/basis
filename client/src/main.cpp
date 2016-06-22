#include "basis_socket.h"
#include "basis_event_loop.h"
#include "basis_socket_io.h"

using namespace basis;

// can write
class CBK3 : public BSWriteableCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd)
	{
		if (BSSocketIO::write(fd, "abcd", 4) <= 0)
		{
			el->DeleteFileEvent(fd, ae_writeable);
			printf("write error\n");
			return;
		}
		el->DeleteFileEvent(fd, ae_writeable);
	}

	static CBK3 m_instance;
};
CBK3 CBK3::m_instance;

class CBK2 : public BSReadableCallBack
{
public:
	virtual void onReadable(BSEventLoop *el, int fd)
	{
		char buff[1024] = {0};
		int result = BSSocketIO::read(fd, buff, 1024);
		if (result <= 0)
		{
			el->DeleteFileEvent(fd, ae_readable);
			printf("socket %d is closed.\n", fd);
			return;
		}
		buff[result] = 0;
		//printf("recv remote(%d), buff(%s).\n", fd, buff);
		el->CreateFileEvent(fd, ae_writeable, &CBK3::m_instance);
		return;
	}

	static CBK2 m_instance;
};
CBK2 CBK2::m_instance;

// connect success
class CBK4 : public BSWriteableCallBack
{
public:
	virtual void onWriteable(BSEventLoop *el, int fd)
	{
		el->DeleteFileEvent(fd, ae_writeable);
		BSSocket sock(fd);
		int err = BSSocket::sock_error(sock);
		if (err != 0)
		{			
			return;
		}
		el->CreateFileEvent(fd, ae_writeable, &CBK3::m_instance);
		el->CreateFileEvent(fd, ae_readable, &CBK2::m_instance);
		sock.detach();
	}

	static CBK4 m_instance;
};
CBK4 CBK4::m_instance;

int main()
{
	//BSSocket sock;
	//sock.open(st_tcp);
	//sock.set_nonblock(true);
	//BSSockAddr addr = BSSockAddr::make_sock_addr("", 8899);
	//if (!BSSocketIO::listen(sock, addr, 5))
	//{
	//	printf("listen error\n");
	//	return -1;
	//}

	//CBK1* c = new CBK1();
	//BSEventLoop* el = BSEventLoop::CreateEventLoop(2000);
	//if (el)
	//{
	//	el->CreateFileEvent(sock, ae_readable, c);
	//	el->RunLoop();
	//}

	BSEventLoop* el = BSEventLoop::CreateEventLoop(2000);
	for (uint32 i = 0; i < 20; ++i)
	{
		//client
		BSSockAddr addr = BSSockAddr::make_sock_addr("192.168.10.73", 8899);
		
		bool is_success = false;
		int fd = BSSocketIO::connect_asyn(addr, is_success);
		if (fd > 0 && is_success)
		{
			el->CreateFileEvent(fd, ae_writeable, &CBK3::m_instance);
			el->CreateFileEvent(fd, ae_readable, &CBK2::m_instance);
		}
		else if(fd > 0)
		{
			el->CreateFileEvent(fd, ae_writeable, &CBK4::m_instance);
		}
	}
	el->RunLoop();


	return 0;
}