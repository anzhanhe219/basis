#include "basis_var.h"
#include <iostream>
#include "basis_thread.h"
#include "basis_sync_queue.h"
#include "basis_event.h"

using namespace basis;

BSSyncQueue<int> syncqueue;

BSEvent the_event;

class Run1 : public BSRunnable
{
public:
	virtual void run(BSThread* p)
	{
		the_event.wait();
		printf("11");
	}
};

class Run2 : public BSRunnable
{
public:
	virtual void run(BSThread* p)
	{
		the_event.wait();
		printf("22");
	}
};


int main()
{
	BSThread thread1("t1");
	BSThread thread2("t2");

	Run1 r1;
	Run2 r2;

	thread1.start(&r1);
	thread2.start(&r2);

	Sleep(10000);
	
	the_event.set();

	int a = 0;
	syncqueue.pull(a);
	syncqueue.pull(a);
	



	//thread1.stop();
	//thread2.stop();

	Sleep(100000);
	thread1.join();
	thread2.join();

	
	uint32 sz = syncqueue.size();
	

	return 0;
}
	