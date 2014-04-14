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

private:
	CSocket *socket_;
	struct pollfd ufds_[max_clients + 1];
	int cur_place;

	int listen_fd;
	static bool end_server;
};
#endif
