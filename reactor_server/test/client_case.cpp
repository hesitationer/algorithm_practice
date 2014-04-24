#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wwrite-strings"
#pragma GCC diagnostic warning "-Wunused-variable"
#include "pick_up_test_case.h"

#include <unistd.h>
#include "net.h"

ERIC_TEST(client_case, server_can_accept)
{
	CSocket client;
	client.ConnectTo("127.0.0.1","51767");
	int fd = client.GetSocketID();

	sleep(1);
	char message[256];
	snprintf(message,256,"hello");
	client.SendRawData(fd,message,256);
	sleep(2);
	snprintf(message,256,"after hello");
	client.SendRawData(fd,message,256);

	//
	close(fd);
}
#pragma GCC diagnostic pop
