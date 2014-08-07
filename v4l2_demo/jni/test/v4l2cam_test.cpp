#include "V4L2_Camera.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/poll.h>

#include "pick_up_test_case.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

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

//int test_get_v4l2_format()
ERIC_TEST(v4l2, check_fmt)
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
		return ;
	}

	printf("type: %d\n",v4l2_fmt.type);
	printf("img width: %d\n",v4l2_fmt.fmt.pix.width);
	printf("img height: %d\n",v4l2_fmt.fmt.pix.height);

	printf("img pix format %d--: ",v4l2_fmt.fmt.pix.pixelformat);
	int pix_fmt = v4l2_fmt.fmt.pix.pixelformat;
	char* teller = (char*)(&pix_fmt); 
	for(int i = 0; i < 4; ++i)
	{
		printf("%c",teller[i]);
	}
	printf("\n");
}


void yuv2rgb422(unsigned char *buf_yuv, unsigned char *buf_rgb, int yuv_size);
int bmp_write(unsigned char *image, int xsize, int ysize, char *filename); 
void NV21_2_RGB888(unsigned char* nv21, unsigned char* rgb, int w, int h);

// Need to set the Pixfmt to yuyv
ERIC_TEST(v4l2,yuyv_get_one_frame)
{
	V4L2_Camera cam;
	cam.open_cam(); 

	char *frame_buf = (char*)malloc(640*480*2*sizeof(char));
	for (int i = 0; i < 20; i++){ // skip the 20 frames
		cam.query_frame(frame_buf);
	}
	
	printf("after query_frame\n");

	FILE *fd = fopen("/data/eric/yuyv.bin","wb");
	if(fd == NULL) {
	    printf("open file failed\n");
	}
	fwrite(frame_buf, 640*480*2, 1, fd);
	fclose(fd);

	printf("save to: *yuyv.bin*\n");
	free(frame_buf);
}

//int test_grab_frame()
ERIC_TEST(v4l2,nv21_get_one_frame)
{
	V4L2_Camera cam;
	cam.open_cam();

	char *frame_buf = (char*)malloc(640*480*3/2*sizeof(char));
	for (int i = 0; i < 20; i++){
		cam.query_frame(frame_buf);
	}
	

    printf("after query_frame\n");
    FILE *fd = fopen("/data/eric/nv21.bin","wb");
    if(fd == NULL)
    {
        printf("open file failed\n");
    }
    fwrite(frame_buf, 640*480*3/2, 1, fd);
    fclose(fd);

	//unsigned char rgb_buf[640*480*3] = {0};
	//NV21_2_RGB888((unsigned char*)frame_buf,rgb_buf,640,480);
	//bmp_write(rgb_buf,640,480,"nv21_temp");
}

ERIC_TEST(v4l2,test_get_rate)
{
	V4L2_Camera cam;
	cam.open_cam();

	printf("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	cam.get_frame_rate();

	//cam.get_support_frame_size();
}

//int test_get_input()
ERIC_TEST(v4l2,test_get_input)
{
	V4L2_Camera cam;
	cam.open_cam();
	cam.get_input_();
}

//int test_set_abs_exp()
ERIC_TEST(v4l2, set_abs_exp)
{
	V4L2_Camera cam;
	cam.open_cam();
	//cam.set_abs_exposure(853);
}


