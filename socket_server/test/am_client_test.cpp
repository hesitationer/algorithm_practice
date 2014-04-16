#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "am_client.h"

int g_stop = false;
void stop_all_connections(int arg)
{
	printf("stop_all_connections\n");
	g_stop = true;
	//AM_Client::Stop();
	usleep(50*1000);
}

void* thread_routine(void *arg)
{
	printf("thread_id from pthread_create:%u\n",*(long*)arg);
	printf("thread_id from pthread_self():%u\n",pthread_self());
	//printf("thread_id from getid():%d\n",gettid());//can't find gettid() in //Fedora20

	AM_Client client;
	client.SetThreadID(*(int*)arg);
	client.SetServerIP("127.0.0.1");
	client.SetServerPort("51767");
	client.Start();
}

int help()
{
	printf("usage: main arg1 arg2 arg3\n");
	printf("\targ1: the ip of server\n");
	printf("\targ2: the port of server\n");
	printf("\targ3: the client number will created\n");

	return 0;
}

int main(int argc, char* argv[])
{
	signal(SIGINT,stop_all_connections);
	pthread_t thread_array[1024];

	static int i = 0;
	while(!g_stop)
	{
		for(; i < 2; ++i) 
		{
			pthread_create(thread_array + i, NULL, thread_routine, thread_array + i);
			sleep(1);
		}
		usleep(10*1000);
	}

	return 0;
}
