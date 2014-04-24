#include <stdio.h>
#include "event_handler.h"
#include "initiation_dispatcher.h"

#include <unistd.h>
#include "net.h"

EventHandler::EventHandler()
{
	//printf("EventHandler ctor!\n");
}

EventHandler::~EventHandler()
{
	//printf("EventHandler dtor!\n");
}

////////////////////////////////////////////////////////////////
EventHandler_Read::EventHandler_Read(int fd):
	resource_handle(fd)
{

}

EventHandler_Read::~EventHandler_Read()
{
}

int EventHandler_Read::handle_event(Event_Type event_type)
{
	switch(event_type)
	{
case ACCEPT_EVENT:
	printf("EventHandler_Read handle event ACCEPT_EVENT\n");
	break;
case READ_EVENT:
	{
		printf("handle event READ_EVENT\n");

		char msg[256];
		int ret = socket_.RecvRawData(resource_handle,msg,256);
		if(ret < 0)
		{
			sleep(2);
			printf("ret < 0, so call remove_handler...\n");

			InitiationDispatcher::instance()->
				remove_handler(this,READ_EVENT);
		}
		printf("receive from %d: %s\n",resource_handle,msg);
		break;
	}
case CLOSE_EVENT:
	{
		close(resource_handle);
		delete this;

		printf("handle event CLOSE_EVENT\n");
	}
	break;
case TYPE_4:
	printf("handle event TYPE_4\n");
	break;
default:
	printf("unknow event type!\n");
	break;
	}

	return 0;
}

int EventHandler_Read::get_handle() const
{
	return resource_handle;
}

////////////////////////////////////////////////////////////////
ConnAcceptor::ConnAcceptor():
	socket_(),
	listenning_fd(-1)
{
	//socket_.SetPortNum_AsServer(listenning_fd);
	socket_.AsServer();	
	listenning_fd = socket_.GetSocketID();
	socket_.ListenForConnection();

	printf("register ACCEPT_EVENT listenning_fd is :%d\n",listenning_fd);
	InitiationDispatcher::instance()->register_handler(this,ACCEPT_EVENT);
}

ConnAcceptor::~ConnAcceptor()
{
}

int ConnAcceptor::get_handle() const
{
	return listenning_fd;
}

int ConnAcceptor::handle_event(Event_Type event_type)
{
	switch(event_type)
	{
case ACCEPT_EVENT:
	{
		printf("ConnAcceptor handle event ACCEPT_EVENT\n");
		
		int new_fd = socket_.AcceptNewConnection();
		
		EventHandler_Read* handler = new EventHandler_Read(new_fd);
		printf("register READ_EVENT new_fd is :%d %p\n",new_fd,handler);
		InitiationDispatcher::instance()->
			register_handler(handler, READ_EVENT);
		break;
	}
case READ_EVENT:
	{
		printf("ConnAcceptor handle event READ_EVENT\n");
		char msg[256];
		int ret = socket_.RecvRawData(listenning_fd,msg,256);
		if(ret < 0)
		{
			sleep(2);
			printf("ret < 0, so call remove_handler...\n");

			InitiationDispatcher::instance()->
				remove_handler(this,READ_EVENT);
		}
		printf("receive from %d: %s\n",listenning_fd,msg);
		break;
	}
case CLOSE_EVENT:
	printf("handle event TYPE_3\n");
	break;
case TYPE_4:
	printf("handle event TYPE_4\n");
	break;
default:
	printf("unknow event type!\n");
	break;
	}

	return 0;
}
