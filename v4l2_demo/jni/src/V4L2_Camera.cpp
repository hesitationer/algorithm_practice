#include <stdio.h>
#include <string.h>
#include "V4L2_Camera.h"


#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include <string>



//this struct was according the A20 platform {{ --------------
struct v4l2_control_
{
	__u32 id;
	__s32 value;
	__u32 user_pt;  //this was new added by A20 engineer
};

//
//this line is important. Because 'v4l2_control' was used as below
//
#define v4l2_control v4l2_control_ 

#ifdef VIDIOC_G_CTRL
	#undef VIDIOC_G_CTRL
	#define VIDIOC_G_CTRL _IOWR ('V', 27, struct v4l2_control_)
#endif

#ifdef VIDIOC_S_CTRL
	#undef VIDIOC_S_CTRL
	#define VIDIOC_S_CTRL _IOWR ('V', 28, struct v4l2_control_)
#endif

#ifdef __OLD_VIDIOC_
#define VIDIOC_S_CTRL_OLD _IOW ('V', 28, struct v4l2_control_)
#endif
//modify struct end by Eric ----------------}}

V4L2_Camera::V4L2_Camera():
	cam_fd_(-1),
	cam_name_(),
	exposure_value_(900),
	pixle_format_(V4L2_PIX_FMT_NV21),
	img_width_(640),
	img_height_(480),
	map_mem_(),
	v4l2_buf_read_(),
	v4l2_buf_index_(0)
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
	printf("\n========now play with ABS-EXPOSURE========\n");
	struct v4l2_control ctrl;


	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	int ret = ioctl(cam_fd_,VIDIOC_G_CTRL, &ctrl);
	if(ret < 0)
	{
		printf("\tGet abs exposure fail with return is %d!\n",ret);
	}
	else
	{
		printf("\tGet abs exposure success with value is %d!\n",ctrl.value);
	}

	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ctrl.value = exp_value;
	ret = ioctl(cam_fd_, VIDIOC_S_CTRL, &ctrl);
	if(ret < 0)
	{
		printf("\tSet abs exposure fail with return is %d!\n",ret);
	}
	else
	{
		printf("\tSet abs exposure success to value  %d\n",ctrl.value);
	}

	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ret = ioctl(cam_fd_,VIDIOC_G_CTRL, &ctrl);
	if(ret < 0)
	{
		printf("\tAfter set,Get abs exposure fail with return is %d!\n",ret);
	}
	else
	{
		printf("\tAfter set,Get abs exposure success with exp is %d!\n",ctrl.value);
	}

	printf("========play with ABS-EXPOSURE end========\n\n");
	return 0;
}

int V4L2_Camera::get_v4l2_format(v4l2_format *fmt)
{
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	int ret = ioctl(cam_fd_, VIDIOC_G_FMT, fmt);	
	if(ret < 0)
	{
		printf("VIDIOC_G_FMT failed:%s\n",strerror(errno));
	}

	return ret;
}

int V4L2_Camera::get_support_frame_size()
{
	int ret;
	struct v4l2_frmsizeenum fsize;

	memset(&fsize, 0, sizeof(fsize));
	fsize.index = 0;
	fsize.pixel_format = pixle_format_;

	while((ret = ioctl(cam_fd_, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0)
	{
		if(fsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
		{
			printf("type is V4L2_FRMSIZE_TYPE_DISCRETE\n");
		}
		else if(fsize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS)
		{
			printf("type is V4L2_FRMSIZE_TYPE_CONTINUOUS\n");
		}
		else if(fsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
		{
			printf("type is V4L2_FRMSIZE_TYPE_STEPWISE\n");
		}
		else
		{
			printf("no~no\n");
		}	

		fsize.index++;
	}

	if(ret !=0 && errno != EINVAL)
	{
		printf("ERROR enumerating frame size: %s\n",strerror(errno));
	}

	printf("index-ret:%d-%d\n",fsize.index,ret);
	printf("frame size wxh: %dx%d\n",fsize.discrete.width,fsize.discrete.height);

	return 0;
}

int V4L2_Camera::get_frame_rate()
{
	printf("get_frame_rate\n");

	v4l2_frmivalenum frame_intervals;
	memset(&frame_intervals, 0, sizeof(struct v4l2_frmivalenum));	

	frame_intervals.index = 0;
	frame_intervals.pixel_format = pixle_format_;
	frame_intervals.width = img_width_;
	frame_intervals.height = img_height_;

	int ret = -1;
	float interval = 0.0;
	float frm_rate = 0.0;

	while((ret = ioctl(cam_fd_, VIDIOC_ENUM_FRAMEINTERVALS, &frame_intervals)) == 0)
	{

		ret = ioctl(cam_fd_, VIDIOC_ENUM_FRAMEINTERVALS, &frame_intervals);

		if(V4L2_FRMIVAL_TYPE_DISCRETE == frame_intervals.type) 
		{
			printf("type is V4L2_FRMIVAL_TYPE_DISCRETE\n");

			printf("\tnumerator is %d, denominator is %d\n",
					frame_intervals.discrete.numerator,
					frame_intervals.discrete.denominator);

			interval = (float)frame_intervals.discrete.numerator/frame_intervals.discrete.denominator;
			frm_rate = 1./interval;

			printf("\t%f---%f\n",interval, frm_rate);
		}
		else
		{
			if(V4L2_FRMIVAL_TYPE_CONTINUOUS == frame_intervals.type) 
			{
				printf("type is V4L2_FRMIVAL_TYPE_CONTINUOUS\n");
			}
			if(V4L2_FRMIVAL_TYPE_STEPWISE == frame_intervals.type) 
			{
				printf("type is V4L2_FRMIVAL_TYPE_STEPWISE\n");
			}

			float min_interval = frame_intervals.stepwise.min.numerator/frame_intervals.stepwise.min.denominator;
			float max_interval = frame_intervals.stepwise.max.numerator/frame_intervals.stepwise.max.denominator;
			float step_interval = frame_intervals.stepwise.step.numerator/frame_intervals.stepwise.step.denominator;

			printf("min-max-step:%f-%f-%f",min_interval,max_interval,step_interval);
		}


		frame_intervals.index += 1;
	}

	return 0;
}

int V4L2_Camera::get_pixel_format()
{
	struct v4l2_format format;

	get_v4l2_format(&format);

	if(pixle_format_ != format.fmt.pix.pixelformat)
	{
		printf("Warning: pixle_format_ not same with the VIDIOC_G_FMT result!\n");
		pixle_format_ = format.fmt.pix.pixelformat;
	}

	return pixle_format_;
}

int V4L2_Camera::set_pixel_format(int pix_fmt)
{
	printf("set_pixel_format\n");
	int ret = -1;

	//update pixle_format_
	pixle_format_ = pix_fmt;

	//check if support
	ret = check_support_fmt_(pixle_format_);
	if(ret != 0)
	{
		printf("\tSorry,device doesn't support pixel format: ");
		char *teller = (char*)&pixle_format_;
		for(int i = 0; i < 4; ++i)
		{
			printf("%c", teller[i]);
		}
		printf("\n\n\n");

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
	if(ret < 0)
	{
		printf("check_capability_ failed with ret %d\n",ret);
	}
	sleep(1);

	ret = set_pixel_format(V4L2_PIX_FMT_NV21);
	if(ret < 0)
	{
		printf("set_pixel_format fail with ret %d\n",ret);
	}
	
	ret = set_abs_exposure(exposure_value_);
	if(ret < 0)
	{
		printf("set_abs_exposure fail with ret %d\n",ret);
	}
	sleep(1);

	ret = request_v4l2buffers_();
	if(ret < 0)
	{
		printf("request_v4l2buffers_ fail with ret %d\n",ret);
	}
	sleep(1);

	ret = query_v4l2buffer_();
	if(ret < 0)
	{
		printf("query_v4l2buffer_ fail with ret %d\n",ret);
	}
	sleep(1);

	ret = start_streaming_();
	if(ret < 0)
	{
		printf("start_streaming_ fail with ret %d\n",ret);
	}
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

int V4L2_Camera::get_input_()
{
	int ret = -1;

	struct v4l2_input inp;
	
	ret = ioctl(cam_fd_, VIDIOC_G_INPUT, &inp);
	if(-1 == ret)
	{
		printf("VIDIOC_G_INPUT error!\n");
		ret = -1;
	}
	else
	{
		printf("VIDIOC_G_INPUT success!\n");
		printf("\t inp.index is: %d!\n",inp.index);
		printf("\t inp.name[32] is %s!\n",inp.name);
		printf("\t inp.type is %d!\n",inp.type);
		printf("\t inp.audioset is %d!\n",inp.audioset);
		printf("\t inp.tuner is %d!\n",inp.tuner);
		printf("\t inp.std is %d!\n",(int)inp.std);
		printf("\t inp.status is %d!\n",inp.status);
		//printf("\t inp.capabilities is %d!\n",inp.capabilities);
		//
		printf("\n\n");
	}

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
		printf("\t inp.index is: %d!\n",inp.index);
		printf("\t inp.name[32] is %s!\n",inp.name);
		printf("\t inp.type is %d!\n",inp.type);
		printf("\t inp.audioset is %d!\n",inp.audioset);
		printf("\t inp.tuner is %d!\n",inp.tuner);
		printf("\t inp.std is %d!\n",(int)inp.std);
		printf("\t inp.status is %d!\n",inp.status);
		//printf("\t inp.capabilities is %d!\n",inp.capabilities);
		printf("\n\n");
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
	printf("check_support_fmt_\n");

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
		printf("\tfmt id = %d, name = %s, v4l2pix_format = %d--",
				i, fmtdesc.description, fmtdesc.pixelformat);
		char *teller = (char*)(&fmtdesc.pixelformat);
		for(int i = 0; i < 4; ++i)
		{
			printf("%c",teller[i]);
		}
		printf("\n");

		if (fmtdesc.pixelformat == format)
		{
			ret = 0;
			break;
		}
	}
	printf("\n\n");

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
	printf("query_v4l2buffer_ and mmap\n");

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
		printf("\t index: %d, mem: %x, len: %d, offset: %d\n", 
				i, 
				*(int*)map_mem_.mem[i],
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
	printf("\n\n");

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
	memset(&v4l2_buf_read_, 0, sizeof(v4l2_buf_read_));	
	v4l2_buf_read_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf_read_.memory = V4L2_MEMORY_MMAP;

	do
	{
		ret = ioctl(cam_fd_, VIDIOC_DQBUF, &v4l2_buf_read_);	
		if(ret < 0)
		{
			usleep(100*1000);
			printf("grab_frame() VIDIOC_DQBUF failed with ret %d!\n",ret);
		}
	}while(-1 == ret);

	printf("grab_frame() success with index is %d!\n",v4l2_buf_read_.index);

	//record the buf index
	v4l2_buf_index_ = v4l2_buf_read_.index;

	return ret;
}

int V4L2_Camera::retrieve_frame(char *&out_pixel_buffer)
{
	int ret = -1; 

	//copy the frame
	clock_t start, end;
	start = clock();
	memcpy(out_pixel_buffer,(char*)map_mem_.mem[v4l2_buf_index_], map_mem_.length);
	end = clock();
	printf("copy once, take %f [ms]\n",(end - start)*1000./CLOCKS_PER_SEC);

	//Queue buffer again
	v4l2_buf_read_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf_read_.memory = V4L2_MEMORY_MMAP;
	v4l2_buf_read_.index= v4l2_buf_index_;
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
