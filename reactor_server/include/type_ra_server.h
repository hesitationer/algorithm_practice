#ifndef TYPE_RA_SERVER_H
#define TYPE_RA_SERVER_H
typedef enum Event_Type
{
	READ_EVENT   = 01,
	ACCEPT_EVENT = 02,
	CLOSE_EVENT  = 04,
	TYPE_4 = 010
}Event_Type;
#endif
