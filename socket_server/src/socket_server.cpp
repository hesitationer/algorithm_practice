#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "socket_server.h"


bool SocketServer::end_server = false;
SocketServer::SocketServer():
	socket_(NULL),
	cur_place(0),
	listen_fd(-1),
	data_received_(0.0),
	time_elapsed_(0.0)
{
	socket_ = new CSocket();
}

SocketServer::~SocketServer()
{
	delete socket_;
}

int SocketServer::Start()
{
	socket_->AsServer();
	socket_->ListenForConnection();

	listen_fd = socket_->GetSocketID();
	ufds_[0].fd = listen_fd;
	ufds_[0].events = POLLIN;
	cur_place += 1;

	for(int i = 1; i < max_clients + 1; ++i)
	{
		ufds_[i].fd = -1;
		ufds_[i].events = -1;
	}

	//int buf_size = 640*480*2;
	//char res_buf[32] = {0};
	//char recv_buf[640*480*2] = {0};

	int rv = 0;
	start_ = clock();
	while(!end_server)
	{
		rv = poll(ufds_,1024,10);

		if(rv < 0)
		{
			perror("poll()");
			break;
		}

		if(rv == 0)
		{
			printf("rv == 0, nothing happened\n");
		}

		for(int j = 0; j < max_clients + 1; ++j)
		{
			if(ufds_[j].fd == -1)
			{
				continue;
			}

			if(ufds_[j].revents == 0)
			{
				continue;
			}

			if(ufds_[j].revents != POLLIN)
			{
				printf("Error events = %d\n",ufds_[j].revents);
				end_server = true;
				break;
			}

			if(ufds_[j].fd == listen_fd)
			{
				//accept new client
				int new_fd = socket_->AcceptNewConnection();

				routine_para_t para;//NOTE: this struct has no memory need to handle
				para.server_running = this;
				para.fd = new_fd;
				para.thread_id = (unsigned int*)thread_id_array + cur_place;
				//create new thread
				pthread_create(thread_id_array + cur_place, NULL,
						process_data_routine_,(void*)&para);
				cur_place++;//thread number
			}
			else
			{

			}
		}
	}//while(end_server)


	return 0;
}

int SocketServer::show_io_speed()
{
	data_received_ += 614.4;//640*480*2 = 614400

	clock_t end = clock();
	time_elapsed_ += (double)((double)end - (double)start_)/CLOCKS_PER_SEC;
	start_ = end;
	double speed = (double)data_received_/time_elapsed_;
	printf("I/O speed is %f/%f =  %f KB/S\n",data_received_, time_elapsed_, speed);

	return 0;
}

int SocketServer::Stop()
{
	end_server = true;

	return 0;
}

int SocketServer::process_data_(char *&data_received)
{
	int am_time_per_frame = 10*1000;
	usleep(am_time_per_frame);


	return 0;
}

void* SocketServer::process_data_routine_(void* arg)
{
	int buf_size = 640*480*2;
	char res_buf[32] = {0};
	char recv_buf[640*480*2] = {0};

	routine_para_t *para = (routine_para_t*)(arg);
	SocketServer* server_this = para->server_running;
	CSocket *socket_ = server_this->socket_;
	int client_fd = para->fd;
	unsigned int *thread_id = para->thread_id;

	//receive and process until the client hung-up
	bool  client_hung_up = false;
	while(!client_hung_up)
	{
		//receive data from client
		int ret = socket_->RecvRawData(client_fd,recv_buf, buf_size);
		if(ret < 0)
		{
			printf("RecvRawData return %d, so forget client %d\n",ret,client_fd);
			break;//this thread will ends
		}

		server_this->show_io_speed();
		char *temp = (char*)recv_buf;
		if(0 == server_this->process_data_(temp))
		{
			ret = socket_->SendRawData(client_fd, res_buf, 32);
			if(ret < 0)
			{
				printf("SendRawData fail with ret %d\n",ret);
				break;
			}
		}
	}

	printf("thread %u ends\n",*thread_id);

	return NULL;
}
