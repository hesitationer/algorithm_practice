#include "V4L2_Camera.h"


#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include <string>



V4L2_Camera::V4L2_Camera():
cam_fd_(-1),
cam_name_(),
exposure_value_(900),
pixle_format_(V4L2_PIX_FMT_NV21),
img_width_(640),
img_height_(480),
map_mem_(),
v4l2_buf_read_()
{
	snprintf(cam_name_, 255, "/dev/video0");
}

V4L2_Camera::~V4L2_Camera()
{
	//stop streaming
	enum v4l2_buf_type type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(cam_fd_, VIDIOC_STREAMOFF, &type);


	// Unmap buf
	for(int i = 0; i < V4L2_BUF_NUMBER; i++)	
	{
		munmap(map_mem_.mem[i], map_mem_.length);
	}

	//close device
	close(cam_fd_);
}

int V4L2_Camera::get_abs_exposure()
{
	return exposure_value_;
}

int V4L2_Camera::set_abs_exposure(int exp_value)
{
	return 0;
}

int V4L2_Camera::get_pixel_format()
{

	return pixle_format_;
}

int V4L2_Camera::set_pixel_format(int pix_fmt)
{
	int ret = -1;

	//update pixle_format_
	pixle_format_ = pix_fmt;

	//check if support
	ret = check_support_fmt_(pixle_format_);
	if(ret != 0)
	{
		printf("Sorry,device doesn't support pixel format: %d\n", pixle_format_);
		return -1;
	}

	struct v4l2_format format;
	memset(&format, 0, sizeof(format));

	//assign value
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width  = img_width_;//img_size should be adjustable
    format.fmt.pix.height = img_height_;
	format.fmt.pix.pixelformat = pixle_format_;//
	format.fmt.pix.field = (v4l2_field)1;

	ret = ioctl(cam_fd_, VIDIOC_S_FMT, &format);
	if (ret < 0)
	{
		printf("VIDIOC_S_FMT Failed: %s\n", strerror(errno));
	}

	return ret;
}

int V4L2_Camera::open_cam()
{
	int ret = -1;

	ret = open_cam_();
	if(ret < 0)
	{
		printf("open_cam_ failed with ret %d\n",ret);
		return -1;
	}
	sleep(1);

	ret = set_input_();
	if(ret < 0)
	{
		printf("set_input_ failed with ret %d\n",ret);
		return -2;
	}

	ret = check_capability_();
	printf("check_capability_\n");
	sleep(1);

	ret = set_pixel_format(pixle_format_);
	
	ret = set_abs_exposure(exposure_value_);
	printf("set_abs_exposure()\n");
	sleep(1);

	ret = request_v4l2buffers_();
	printf("request_v4l2buffers_()\n");
	sleep(1);

	ret = query_v4l2buffer_();
	printf("query_v4l2buffer_()\n");
	sleep(1);

	ret = start_streaming_();
	printf("start_streaming_()\n");
	sleep(1);


	return 0;
}

int V4L2_Camera::open_cam_()
{
	//cam_fd_ = open(cam_name_, O_RDWR | O_NONBLOCK,0);	
	cam_fd_ = open("/dev/video0", O_RDWR | O_NONBLOCK,0);	

	if(-1 == cam_fd_)//open fail
	{
		printf("try to open /dev/video1\n");

		snprintf(cam_name_, 255, "/dev/video1");
		cam_fd_ = open(cam_name_,O_RDWR | O_NONBLOCK, 0);

		if(-1 == cam_fd_)//fail again
		{
			printf("open %s fail!\n",cam_name_);

			return -1;
		}
	}

	printf("open %s success with cam_fd_ is %d\n",cam_name_,cam_fd_);

	return 0;
}

int V4L2_Camera::set_input_()
{
	int ret = -1;

	struct v4l2_input inp;
	inp.index = 0;

	ret = ioctl(cam_fd_, VIDIOC_S_INPUT, &inp);
	if(-1 == ret)
	{
		printf("VIDIOC_S_INPUT error!\n");
		ret = -1;
	}
	else
	{
		printf("VIDIOC_S_INPUT success!\n");
		printf("\t inp.index is %d:!\n",inp.index);
		printf("\t inp.name[32] is %s!\n",inp.name);
		printf("\t inp.type is %d!\n",inp.type);
		printf("\t inp.audioset is %d!\n",inp.audioset);
		printf("\t inp.tuner is %d!\n",inp.tuner);
		printf("\t inp.std is %d!\n",(int)inp.std);
		printf("\t inp.status is %d!\n",inp.status);
		//printf("\t inp.capabilities is %d!\n",inp.capabilities);
	}

	return ret;
}

int V4L2_Camera::check_capability_()
{
	int ret = -1;
	struct v4l2_capability cap;
	ret = ioctl(cam_fd_, VIDIOC_QUERYCAP, &cap);
	if(ret < 0)
	{
		printf("Error opening device: unable to query device.\n");
		return -1;
	}

	if(0 == (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		printf("Error opening device: video capture not support.\n");
		return -2;
	}

	if(0 == (cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("Capture device doesn't support streaming i/o\n");
		return -3;
	}

	return 0;
}

int V4L2_Camera::check_support_fmt_(int format)
{
	int ret = -1;

	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i = 0; i < 128; i++)
	{
		fmtdesc.index = i;
		if (-1 == ioctl (cam_fd_, VIDIOC_ENUM_FMT, &fmtdesc))
		{
			break;
		}
		printf("fmt id = %d, name = %s, v4l2pix_format = %d\n",
				i, fmtdesc.description, fmtdesc.pixelformat);

		if (fmtdesc.pixelformat == format)
		{
			ret = 0;
			break;
		}
	}

	return ret;
}

int V4L2_Camera::request_v4l2buffers_()
{
	int ret = -1;

	struct v4l2_requestbuffers request_buf;
	memset(&request_buf, 0, sizeof(request_buf));	

	request_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	request_buf.memory = V4L2_MEMORY_MMAP;
	request_buf.count = V4L2_BUF_NUMBER;

	ret = ioctl(cam_fd_, VIDIOC_REQBUFS, &request_buf);
	if(ret < 0)
	{
		printf("VIDIOC_REQBUFS failed: %s",strerror(errno));
	}

	return ret;
}

int V4L2_Camera::query_v4l2buffer_()
{
	int ret = 0;
	struct v4l2_buffer buf;

	for (int i = 0; i < V4L2_BUF_NUMBER; i++)
	{
        memset (&buf, 0, sizeof (struct v4l2_buffer));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;

		ret = ioctl (cam_fd_, VIDIOC_QUERYBUF, &buf);
        if (ret < 0)
		{
            printf("Unable to query buffer (%s)", strerror(errno));
            ret = -1;
			break;
        }

        map_mem_.mem[i] = mmap(0, buf.length,
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED,
                              cam_fd_,
                              buf.m.offset);
		map_mem_.length = buf.length;
		printf("index: %d, mem: %x, len: %d, offset: %d\n", 
				i, 
				(int)map_mem_.mem[i],
			   	buf.length,
			   	buf.m.offset);

        if (map_mem_.mem[i] == MAP_FAILED)
		{
			printf("Unable to map buffer (%s)\n", strerror(errno));
			ret = -2;
			break;
        }

		// start with all buffers in queue
        ret = ioctl(cam_fd_, VIDIOC_QBUF, &buf);
        if (ret < 0)
		{
            printf("VIDIOC_QBUF Failed\n");
			ret = -3;
			break;
        }

		ret = 0;

	}

	return ret;
}

int V4L2_Camera::start_streaming_()
{
	int ret = -1;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  	ret = ioctl (cam_fd_, VIDIOC_STREAMON, &type);
	if (ret < 0)
	{
		printf("StartStreaming: Unable to start capture: %s", strerror(errno));
	}

	return ret;

}

int V4L2_Camera::grab_frame()
{
	int ret = -1;
	
	//clear old
	//v4l2_buf_read_.index= n_buf_index;
	memset(&v4l2_buf_read_, 0, sizeof(v4l2_buf_read_));	
	v4l2_buf_read_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf_read_.memory = V4L2_MEMORY_MMAP;
	v4l2_buf_read_.index = 0;

	do
	{
		ret = ioctl(cam_fd_, VIDIOC_DQBUF, &v4l2_buf_read_);	
		if(ret < 0)
		{
			usleep(10*1000);
			printf("VIDIOC_DQBUF failed with ret %d!\n",ret);
		}
	}while(-1 == ret);

	//return ret;
}

int V4L2_Camera::retrieve_frame(char *&out_pixel_buffer)
{
	int ret = -1; 

	//copy the frame
	clock_t begin = clock();
	memcpy(out_pixel_buffer,(char*)map_mem_.mem[0], map_mem_.length);
	clock_t end = clock();
	double elapsed_milli_secs = double(end - begin) / CLOCKS_PER_SEC * 1000;
	printf("copy one frame done, and take %d [ms]\n",elapsed_milli_secs);

	//Queue buffer again
	ret = ioctl(cam_fd_, VIDIOC_QBUF, &v4l2_buf_read_);
	if(ret < 0)
	{
		printf("VIDIOC_QBUF failed with ret: %d\n",ret);
	}

	return ret;
}

int V4L2_Camera::query_frame(char *&out_pixel_buffer)
{
	int ret = -1;

	ret = grab_frame();
	if(ret != 0)
	{
		printf("grab_frame failed with return %d!\n",ret);
		out_pixel_buffer = NULL;
		return -1;
	}
	ret = retrieve_frame(out_pixel_buffer);
	if(ret != 0)
	{
		printf("retrieve_frame failed with return %d!\n",ret);
		return -2;
	}

	return 0;
}
