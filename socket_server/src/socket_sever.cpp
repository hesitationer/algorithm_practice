#include "socket_server.h"

SocketServer::SocketServer():
	socket_(NULL),
	listen_fd(-1),
	end_server(false)
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

	ufds_[0].fd = socket_->GetSocketID();
	ufds_[0].events = POLLIN;

	for(int i = 1; i < max_clients + 1; ++i)
	{
		ufds_[i].fd = -1;
		ufds_[i].events = -1;
	}

	int rv = 0;
	while(!end_server)
	{
		rv = poll(ufds_,5,500);

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
			}
			else
			{
				//receive data from client
			}
		}
	}//while(end_server)


	return 0;
}
