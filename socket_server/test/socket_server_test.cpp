#include <signal.h>
#include "socket_server.h"

void ctrl_c_handler(int arg)
{
	//how to stop the Server
	//global variant
	printf("eric ctrl_c_handler\n");
	SocketServer::Stop();
}

int main()
{
	signal(SIGINT,ctrl_c_handler);

	SocketServer server;
	server.Start();

	return 0;
}
