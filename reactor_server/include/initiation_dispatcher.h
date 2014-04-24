#ifndef INITIATION_DISPATCHER_H
#define INITIATION_DISPATCHER_H

#include "type_ra_server.h"
#include "event_handler.h"

#include <unordered_map>
#include <sys/poll.h>

#include "net.h"

class InitiationDispatcher
{
protected://the singleton necessary
	InitiationDispatcher();
public:
	~InitiationDispatcher();

	static InitiationDispatcher* instance();

	int register_handler(EventHandler *eh, Event_Type et);

	int remove_handler(EventHandler *eh, Event_Type et);

	int handle_events(int timeout);

	int set_port(int port_num);

private:
	int remove_pollfd_array_(int fd);

private:
	// hash-table, store the <key,value>
	// key:   handle
	// value: the handler obj
	std::unordered_map<int, EventHandler*> fd_handler_table_;
	pollfd ufds_[1024];
	int max_clients_;

	CSocket socket_;
	int listen_fd;

	static InitiationDispatcher* single_instance_;

};
#endif
