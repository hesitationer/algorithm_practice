#ifndef AM_CLIENT_H
#define AM_CLIENT_H

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

private:
	CSocket* socket_;

	char port_num_[16];
	char server_ip_[16];
	static bool running;

};

#endif
