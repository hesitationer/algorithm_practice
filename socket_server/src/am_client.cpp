#include <time.h>
#include <unistd.h>
#include "am_client.h"

bool AM_Client::running = true;

AM_Client::AM_Client():
	socket_(NULL),
	thread_id_(-1),
	visit_times(0)
{
	sprintf(port_num_,"%d",51767);
	sprintf(server_ip_, "127.0.0.1");

	socket_ = new CSocket();

	memset(ufds_, 0, sizeof(pollfd));



	mutex = PTHREAD_MUTEX_INITIALIZER;
}

AM_Client::~AM_Client()
{
	delete socket_;
	pthread_mutex_destroy(&mutex);
}

int AM_Client::Start()
{
	int ret = socket_->ConnectTo(server_ip_,port_num_);
	if(ret < 0)
	{
		printf("connect to server:port--%s:%s failed!\n",server_ip_,port_num_);
		return -1;
	}

	int conn_fd = socket_->GetSocketID();
	ufds_[0].fd = conn_fd;
	ufds_[0].events = POLLIN;

	int data_len = 640*480*2;
	char data_to_send[640*480*2];
	prev_clock_ = clock();
	while(running)
	{
		usleep(33*1000);//mock time cost of capture from cam

		ret = socket_->SendRawData(conn_fd, data_to_send,data_len);
		if(ret < 0)
		{
			printf("SendRawData fail with ret is %d\n",ret);
		}
		
		check_process_result_();
	}

	printf("exit start with fd:%d\n",conn_fd);
}

int AM_Client::SetServerIP(const char* server_ip)
{
	if(server_ip == NULL)
	{
		printf("server_ip == NULL!\n");
		return -1;
	}

	// TODO: other valid checks
	

	memcpy(server_ip_, server_ip, strlen(server_ip));

	return 0;
}

int AM_Client::SetServerPort(const char* port_num)
{
	// TODO: valide check!!
	
	memcpy(port_num_, port_num, strlen(port_num));

	return 0;
}

int AM_Client::Stop()
{
	printf("AM_Client::Stop\n");
	running = false;

	return 0;
}

int AM_Client::show_recv_interval_()
{
	//pthread_mutex_lock(&mutex);
	printf("%u times:\n",visit_times++);

	clock_t now = clock();
	double t = (double)(now - prev_clock_)*1000./CLOCKS_PER_SEC;
	printf("\tthread %u: now:prev_clock_:t  %u:%u:%g [ms]\n\n",thread_id_,now,prev_clock_,t);

	prev_clock_ = now;

	//pthread_mutex_unlock(&mutex);

	return 0;
}

int AM_Client::check_process_result_()
{
	char result[32] = {0};

	int ret = poll(ufds_, 1, 0);//returns immediately
	if(ret < 0)
	{
		perror("poll");
		return -1;
	}
	if(ufds_[0].revents & POLLIN)
	{
		ret = socket_->RecvRawData(socket_->GetSocketID(), result, 32);
		if(ret < 0)
		{
			printf("RecvRawData fail with ret is %d\n",ret);
		}

		//printf("receive result: %s\n",result);
		show_recv_interval_();
	}


	return 0;
}

int AM_Client::SetThreadID(pthread_t t_id)
{
	thread_id_ = t_id;
}
