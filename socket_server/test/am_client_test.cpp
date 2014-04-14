#include <pthread.h>
#include <unistd.h>
#include "am_client.h"

int g_stop = false;
void stop_all_connections(int arg)
{
	printf("stop_all_connections\n");
	AM_Client::Stop();
	g_stop = false;
}

void* thread_routine(void *arg)
{
	AM_Client client;
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
	int thread_array[65546] = {0};

	int i = 0;
	while(!g_stop)
	{
		for(; i < 100; ++i) 
		{
			pthread_t new_thread;
			pthread_create(&new_thread, NULL, thread_routine, NULL);

			thread_array[i] = new_thread;
		}
		usleep(10*1000);
	}

	return 0;
}
