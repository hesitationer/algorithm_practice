#include <unistd.h>
#include <utility>
#include "initiation_dispatcher.h"


InitiationDispatcher* InitiationDispatcher::single_instance_ = NULL;

InitiationDispatcher::InitiationDispatcher():
	max_clients_(0),
	socket_(),
	listen_fd(-1)
{
	for(int i = 0; i < 1024; ++i)
	{
		ufds_[i].fd = -1;
		ufds_[i].events = -1;	
	}
}


InitiationDispatcher::~InitiationDispatcher()
{
}

int InitiationDispatcher::register_handler(EventHandler *eh, Event_Type et)
{
	int fd = eh->get_handle();
	if(et == ACCEPT_EVENT)
	{
		listen_fd = fd;//record the listenning fd from Acceptor
		printf("%p record listen_fd: %d\n",this,listen_fd);
	}

	printf("%p register handler for fd: %d %p\n",this,fd,eh);
	fd_handler_table_.emplace(std::make_pair(fd,eh));	

	//
	// send to ufds array for poll
	// it's the synchronous Demultiplexer
	ufds_[max_clients_].fd = fd;
	ufds_[max_clients_].events = POLLIN;
	max_clients_++;
	return 0;
}

int InitiationDispatcher::remove_handler(EventHandler *eh, Event_Type et)
{
	// 
	int fd = eh->get_handle();
	printf("remove handler for fd %d %p\n",fd,eh);
	fd_handler_table_.erase(fd);

	//erase the fd from poll array
	remove_pollfd_array_(fd);

	return 0;
}

int InitiationDispatcher::handle_events(int timeout)
{
	//poll
	int rv = poll(ufds_,1024,timeout);
	if(rv < 0)
	{
		perror("InitiationDispatcher::handle_events poll");
		return -1;
	}

	if(rv == 0)
	{
		//printf("rv == 0, nothing happened\n");
	}

	for(int i = 0; i < max_clients_; ++i)
	{
		if(ufds_[i].revents == 0)
		{
			continue;
		}

		int fd = ufds_[i].fd;
		printf("%p event occured on handle %d\n",this,fd);
		if(ufds_[i].revents == POLLIN)
		{
			Event_Type type;
			if(fd == listen_fd)//ACCEPT_EVENT
			{
				type = ACCEPT_EVENT;
			}
			else//READ_EVENT
			{
				type = READ_EVENT;
				printf("type is READ_EVENT\n");
			}

			printf("%p POLLIN event occured on handle %d %p\n",this, fd,fd_handler_table_[fd]);
			fd_handler_table_[fd]->handle_event(type);

		}else if(ufds_[i].revents == POLLRDHUP)
		{
			printf("type is POLLRDHUP\n");
			fd_handler_table_[fd]->handle_event(CLOSE_EVENT);
		}
		else
		{
			printf("ufds_[%d].revents unknow type!\n",i);
		}
	}

	return 0;
}

InitiationDispatcher* InitiationDispatcher::instance()
{
	if(NULL != single_instance_)
	{
		return single_instance_;
	}
	else
	{
		single_instance_ = new InitiationDispatcher();
		return single_instance_;
	}
}

int InitiationDispatcher::set_port(int port)
{
	return socket_.SetPortNum_AsServer(port);
}

int InitiationDispatcher::remove_pollfd_array_(int fd)
{
	int pos = 0;
	//find the fd and reset to invalid(-1)
	for(int i = 0; i < max_clients_; ++i)
	{
		if(ufds_[i].fd != fd)	
		{
			continue;
		}

		ufds_[i].fd = -1;
		ufds_[i].fd = -1;
		pos = i;
		break;
	}

	//compress the poll-array
	for(int j = pos; j + 1 < max_clients_; ++j)
	{
		ufds_[j].fd = ufds_[j+1].fd;
		ufds_[j].events = ufds_[j+1].events;
	}

	//update the max_clients_
	max_clients_ = max_clients_ - 1;
	
	return 0;
}
