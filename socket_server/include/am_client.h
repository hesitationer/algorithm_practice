#ifndef AM_CLIENT_H
#define AM_CLIENT_H

#include <pthread.h>
#include <sys/poll.h>
#include "net.h"

class AM_Client
{
public:
	AM_Client();
	~AM_Client();

	int Start();
	static int Stop();

	int SetServerIP(const char* server_ip);
	int SetServerPort(const char* port_num);

	//for debug, record which thread owns this class
	int SetThreadID(pthread_t t_id);

private:
	int check_process_result_();
	int show_recv_interval_();

private:
	CSocket* socket_;

	char port_num_[16];
	char server_ip_[16];
	static bool running;


	struct pollfd ufds_[1];
	clock_t prev_clock_;

	pthread_mutex_t  mutex;//

	pthread_t thread_id_;
	
	int visit_times;

};

#endif
