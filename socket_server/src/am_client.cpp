#include <unistd.h>
#include "am_client.h"

bool AM_Client::running = true;

AM_Client::AM_Client():
	socket_(NULL)
{
	sprintf(port_num_,"%d",51767);
	sprintf(server_ip_, "127.0.0.1");

	socket_ = new CSocket();
}

AM_Client::~AM_Client()
{
	delete socket_;
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

	int data_len = 640*480*2;
	char data_to_send[640*480*2];
	while(running)
	{
		usleep(33*1000);//mock time cost of capture from cam

		ret = socket_->SendRawData(conn_fd, data_to_send,data_len);
	}
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
	running = false;

	return 0;
}
