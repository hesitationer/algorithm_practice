#include "type_ra_server.h"
#include "initiation_dispatcher.h"
#include "event_handler.h"

int main()
{
	ConnAcceptor handler_accept;//create this handler 
	for(;;)
	{
		InitiationDispatcher::instance()->handle_events(10);
	}

	return 0;

}
