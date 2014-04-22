#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <sys/poll.h>
#include "net.h"

const int max_clients = 1024;
class SocketServer
{
public:
	SocketServer();
	~SocketServer();

	int Start();
	static int Stop();

private:
	int process_data_(char *&data_received);

	int show_io_speed();

	static void* process_data_routine_(void *arg);

private:
	CSocket *socket_;
	struct pollfd ufds_[max_clients + 1];
	int cur_place;

	int listen_fd;
	static bool end_server;

	double data_received_;
	double time_elapsed_;//in seconds
	clock_t start_;

	typedef struct{
		SocketServer* server_running;
		//CSocket *socket_;
		int fd;
		unsigned int *thread_id;
	}routine_para_t;

	pthread_t thread_id_array[max_clients + 1];
};
#endif
