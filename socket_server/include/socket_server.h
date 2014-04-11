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

private:
	CSocket *socket_;
	struct pollfd ufds_[max_clients + 1];

	int listen_fd;
	bool end_server;
};
#endif
