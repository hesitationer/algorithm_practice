#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "type_ra_server.h"
#include "net.h"

//interface
class EventHandler
{
public:
	EventHandler();
	virtual ~EventHandler();

	virtual int handle_event(Event_Type event_type) = 0;

	//return the underlying handle
	virtual int get_handle(void) const = 0;
};

class EventHandler_Read : public EventHandler
{
public:
	EventHandler_Read(int fd);
	~EventHandler_Read();

	virtual int handle_event(Event_Type event_type);
	virtual int get_handle() const;

private:
	//it's socket descriptor
	CSocket socket_;
	int resource_handle;
};

class ConnAcceptor : public EventHandler
{
public:
	ConnAcceptor();
	~ConnAcceptor();


	virtual int handle_event(Event_Type event_type);
	virtual int get_handle() const;

private:
	CSocket socket_;
	int listenning_fd;
};
#endif
