#include <time.h>
#include <unistd.h>
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

	int buf_size = 640*480*2;
	char res_buf[32] = {0};
	char recv_buf[640*480*2] = {0};

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

				ufds_[cur_place].fd = new_fd;
				ufds_[cur_place].events = POLLIN;
				printf("ufds_[j]: j-fd----%d,%d\n",cur_place,new_fd);

				cur_place += 1;

				//echo the client info for debug
			}
			else
			{
				//receive data from client
				int ret = socket_->RecvRawData(ufds_[j].fd,recv_buf, buf_size);
				if(ret < 0)
				{
					printf("RecvRawData return %d, so forget client %d\n",ret,ufds_[j].fd);
					ufds_[j].fd = -1;
					ufds_[j].events = -1;
					continue;//don't send to process_data_()
				}

				show_io_speed();
				char *temp = (char*)recv_buf;
				if(0 == process_data_(temp))
				{
					ret = socket_->SendRawData(ufds_[j].fd, res_buf, 32);
					if(ret < 0)
					{
						printf("SendRawData fail with ret %d\n",ret);
					}
				}

			}
		}
	}//while(end_server)


	return 0;
}

int SocketServer::show_io_speed()
{
	data_received_ += 614.4;//640*480*2 = 614400

	clock_t end = clock();
	time_elapsed_ += (double)(end - start_)/CLOCKS_PER_SEC;
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
