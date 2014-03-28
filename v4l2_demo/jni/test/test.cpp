#include "V4L2_Camera.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/poll.h>



int socket_server_tansfer_cam_frame()
{
	//=====================
	//socket part	
	struct addrinfo hints, *servinfo, *ptr;
	int yes = 1;
	int socket_fd = -1;
	const char *port_num = "51767";

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	printf("select port:%s\n",port_num);//for debug
	int ret = getaddrinfo(NULL, port_num, &hints, &servinfo);
	if ( ret != 0)
	{
		printf("selectserver: %s\n", gai_strerror(ret));

		return -1;
	}

	for(ptr = servinfo; ptr != NULL; ptr = ptr->ai_next) 
	{
		socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (socket_fd < 0)
		{ 
			continue;
		}

		// lose the pesky "address already in use" error message
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		ret = bind(socket_fd, ptr->ai_addr, ptr->ai_addrlen); 
		if (ret < 0)
		{
			close(socket_fd);
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this

	// if we got here, it means we didn't get bound
	if (ptr == NULL) 
	{
		printf("selectserver: failed to bind\n");
		return -2;
	}

	//listen
	ret = listen(socket_fd, 4);
	if(ret < 0)
	{
		printf("listen for connection failed!\n");
		return -3;
	}
	
	//socket part end
	//========================

	//poll part
	//========================
	struct pollfd ufds[5];

	int cur_index = 0;

	ufds[cur_index].fd = socket_fd;
	ufds[cur_index].events = POLLIN;

	cur_index++;	

	//========================
	//cam part
	V4L2_Camera cam;
	cam.open_cam();
	//cam.set_abs_exposure(987);

	char *frame_buf = (char*)malloc(640*480*2*sizeof(char));

	int a = 1000;
	while(--a)
	{
		cam.query_frame(frame_buf);
		if(NULL == frame_buf)
		{
			printf("query_frame failed!\n");
			break;
		}

		//poll
		int poll_ret = poll(ufds,5,100);

		if(poll_ret == -1)
		{
			perror("poll");
			continue;
		}
		else if(poll_ret == 0)
		{
			printf("poll success but no data\n");
			continue;
		}

		if(ufds[0].revents & POLLIN)
		{
			//accept
			struct sockaddr_storage remote_addr_t;
			socklen_t addr_len;
			int new_sock_fd = accept(socket_fd,
					(struct sockaddr *) &remote_addr_t,
					&addr_len);

			ufds[cur_index].fd = new_sock_fd;
			ufds[cur_index].events = POLLOUT;

			cur_index++;
		}

		//send data to connected client
		for(int i = 1; i <= cur_index; ++i)
		{
			if(ufds[i].revents & POLLHUP)
			{
				close(ufds[i].fd);
				ufds[i].fd = -1;
				ufds[i].events = -1;

			}

			if(ufds[i].revents & POLLOUT)//send data
			{
				int pos = 0;
				int sent_len = 0;
				int size_to_send = 640*480*2;
				while(pos < size_to_send)
				{
					sent_len = send(ufds[i].fd, frame_buf + pos, size_to_send - pos, 0);
					if(sent_len < 0)
					{
						perror("send()");
				  	}

					pos += sent_len;
				}
			}
		}
		
	}

	return 0;
}

int test_cam()
{
	V4L2_Camera cam;
	cam.open_cam();

	return 0;
}

int test_get_v4l2_format()
{
	V4L2_Camera cam;
	cam.set_pixel_format(V4L2_PIX_FMT_NV21);
	cam.open_cam();

	struct v4l2_format v4l2_fmt;
	memset(&v4l2_fmt, 0, sizeof(v4l2_fmt));
	int ret = cam.get_v4l2_format(&v4l2_fmt);
	if(ret < 0)
	{
		printf("cam.get_v4l2_format() failed\n");
		return ret;
	}

	printf("type: %d\n",v4l2_fmt.type);
	printf("img width: %d\n",v4l2_fmt.fmt.pix.width);
	printf("img height: %d\n",v4l2_fmt.fmt.pix.height);

	printf("img pix format %d--: ",v4l2_fmt.fmt.pix.height);
	int pix_fmt = v4l2_fmt.fmt.pix.pixelformat;
	char* teller = (char*)(&pix_fmt); 
	for(int i = 0; i < 4; ++i)
	{
		printf("%c",teller[i]);
	}
	printf("\n");

	return 0;
}

int test_grab_frame()
{
	V4L2_Camera cam;
	cam.open_cam();

	char *frame_buf = (char*)malloc(800*600*3/2*sizeof(char));
	cam.query_frame(frame_buf);

	int count = 30;
	while(--count)
	{
		int ret = cam.query_frame(frame_buf);
		if(ret < 0)
		{
			printf("cam.query_frame() failed\n");
			break;
		}
	}

	return 0;

}

int test_get_rate()
{
	V4L2_Camera cam;
	cam.open_cam();

	printf("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	cam.get_frame_rate();

	//cam.get_support_frame_size();
}

int test_get_input()
{
	V4L2_Camera cam;
	cam.open_cam();
	cam.get_input_();
}

int test_set_abs_exp()
{
	V4L2_Camera cam;
	cam.open_cam();
	//cam.set_abs_exposure(853);
}

static char key[32] = {};
static char v1[256] = {};
static char v2[256] = {};

static char filter_key[32] = {};
static char filter_v1[256] = {};
static char filter_v2[256] = {};
static char* op_filter[3] = {filter_key,filter_v1,filter_v2};

int parse_options(char* filter_str);
int store_options();

int main(int argc, char** argv )
{
	//socket_server_tansfer_cam_frame();	
	//test_cam();
	//test_get_v4l2_format();
	//test_grab_frame();
	
	//test_get_rate();
	//test_get_input();
	
	//test_set_abs_exp();

	if(argc == 1)	
	{
		printf("no filter\n");
	}
	else if(argc == 2)
	{
		parse_options(argv[1]);
		store_options();

		for(int i = 0; i < 3; ++i)
		{
			printf("%s\n",op_filter[i]);
		}
	}
	return 0;
}

int run_case()
{
	RUN_ERIC_CASE();
	return 0;
}

int parse_options(char* filter_str)
{

	char *key_ptr = key;
	char *temp = filter_str + 2;
	char *value_1st = v1;
	char *value_2nd = v2;


	if(filter_str[0] == '-' && filter_str[1] == '-')
	{
		//get the key 
		while(*temp != '=' && temp != NULL)
		{
			*key_ptr++ = *temp++;
		}

		printf("\nkey:%s\n",key);


		//get the 1st part of value
		temp++;//move to the key start
		while(*temp != '.' && temp != NULL)
		{
			*value_1st++ = *temp++;
		}

		printf("\nvalue_1st:%s\n",v1);

		//get the 2nd part of value
		temp++;
		while(*temp != 0)
		{
			*value_2nd++ = *temp++;
		}

		printf("value_2nd:%s\n",v2);

	}
	else
	{
		printf("invalid option: %s!\n",filter_str);
	}


	//reset pointer
	key_ptr = NULL;
	temp = NULL;
	value_1st = NULL;
	value_2nd = NULL;	

	return 0;
}


int store_options()
{
	if(strcmp(key,"filter") == 0)
	{
		//copy
		strncpy(filter_key,key,sizeof(key));
		strncpy(filter_v1,v1,sizeof(v1));
		strncpy(filter_v2,v2,sizeof(v2));

		memset(key, 0, sizeof(key));
		memset(v1, 0, sizeof(v1));
		memset(v2, 0, sizeof(v2));

	}
	else if(strcmp(key,"repeat") == 0)
	{
	}
	else
	{
		printf("invalid options:%s\n",key);
	}

	return 0;
}
