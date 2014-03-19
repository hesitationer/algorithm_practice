#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/videodev.h>

#include <stdio.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <errno.h>


#define BUFFER_COUNT 4

//redefine the Macro for ABS-exposure

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

//====================================================
//Google del some of ioctl. now manually add it for v4l2



#define V4L2_CTRL_CLASS_CAMERA 	0x009a0000
#define V4L2_CID_CAMERA_CLASS_BASE 	(V4L2_CTRL_CLASS_CAMERA | 0x900)

#define V4L2_CID_EXPOSURE_ABSOLUTE		(V4L2_CID_CAMERA_CLASS_BASE+2)
//===================================================

//used to visit the mapped mems
typedef struct v4l2_mem_map_t{
	void *	mem[BUFFER_COUNT]; 
	int 	length;
}v4l2_mem_map_t;

v4l2_mem_map_t					mMapMem;

#define UNKNOWN_ERROR -6


char g_pchDevName[255] = "/dev/video0";
int g_nCamFD = 0;// file-descriptor of camera-device
int g_nCaptureFmt = 0; //pix-format

int openDevice()
{

	g_nCamFD = open(g_pchDevName, O_RDWR | O_NONBLOCK,0);

	if(-1 == g_nCamFD)
	{
		char dev0[255];
		sprintf(dev0,"/dev/video0");
		char dev1[255];
		sprintf(dev0,"/dev/video1");
		printf("ERROR opening %s: %s\n",g_pchDevName,strerror(errno));

		if( 0 != strncmp( g_pchDevName, dev0, strlen(dev0) ) )//if not dev0
		{
			strncpy(g_pchDevName,dev0,strlen(dev0));
		}
		else
		{
			strncpy(g_pchDevName,dev1,strlen(dev1));
		}

		printf("Try open another Camera %s\n", g_pchDevName);
		g_nCamFD = open(g_pchDevName, O_RDWR | O_NONBLOCK,0);

		if(-1 == g_nCamFD)
		{
			printf("Try fail, ERROR opening %s: %s\n",g_pchDevName,strerror(errno));

			//OPEN FAIL
			return -1;
		}
	}
	return 0;
}

int checkCapbility()
{

	int ret = -1;
	struct v4l2_capability cap;
	ret = ioctl(g_nCamFD, VIDIOC_QUERYCAP, &cap);
	if(ret < 0)
	{
		printf("Error opening device: unable to query device.\n");
		return -1;
	}

	if(0 == (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		printf("Error opening device: video capture not support.\n");
		return -1;
	}

	if(0 == (cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("Capture device doesn't support streaming i/o\n");
		return -1;
	}

	return 0;

}

int tryFmt(int format)
{
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i = 0; i < 128; i++)
	{
		fmtdesc.index = i;
		if (-1 == ioctl (g_nCamFD, VIDIOC_ENUM_FMT, &fmtdesc))
		{
			break;
		}
		printf("fmt id = %d, name = %s, v4l2pix_format = %d\n",
				i, fmtdesc.description, fmtdesc.pixelformat);

		if (fmtdesc.pixelformat == format)
		{
			return 0;
		}
	}

	return -1;

}

int set_abs_exposure(int exp_value)
{
	printf("\n========now play with ABS-EXPOSURE========\n");
	struct v4l2_control ctrl;


	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	int ret = ioctl(g_nCamFD,VIDIOC_G_CTRL, &ctrl);
	if(ret < 0)
	{
		printf("Get abs exposure fail with return is %d!\n",ret);
	}
	else
	{
		printf("Get abs exposure success with value is %d!\n",ctrl.value);
	}

	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ctrl.value = exp_value;
	ret = ioctl(g_nCamFD, VIDIOC_S_CTRL, &ctrl);
	if(ret < 0)
	{
		printf("Set abs exposure fail with return is %d!\n",ret);
	}
	else
	{
		printf("Set abs exposure success to value  %d\n",ctrl.value);
	}

	ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	ret = ioctl(g_nCamFD,VIDIOC_G_CTRL, &ctrl);
	if(ret < 0)
	{
		printf("After set,Get abs exposure fail with return is %d!\n",ret);
	}
	else
	{
		printf("After set,Get abs exposure success with exp is %d!\n",ctrl.value);
	}

	printf("\n========play with ABS-EXPOSURE end========\n");
	return 0;
}

int queryBuf()
{
	int ret = UNKNOWN_ERROR;
	struct v4l2_buffer buf;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
        memset (&buf, 0, sizeof (struct v4l2_buffer));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;

		ret = ioctl (g_nCamFD, VIDIOC_QUERYBUF, &buf);
        if (ret < 0)
		{
            printf("Unable to query buffer (%s)", strerror(errno));
            return ret;
        }

        mMapMem.mem[i] = mmap (0, buf.length,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED,
                            g_nCamFD,
                            buf.m.offset);
		mMapMem.length = buf.length;
		printf("index: %d, mem: %x, len: %d, offset: %d\n", i, (int)mMapMem.mem[i], buf.length, buf.m.offset);

        if (mMapMem.mem[i] == MAP_FAILED)
		{
			printf("Unable to map buffer (%s)\n", strerror(errno));
            return -1;
        }

		// start with all buffers in queue
        ret = ioctl(g_nCamFD, VIDIOC_QBUF, &buf);
        if (ret < 0)
		{
            printf("VIDIOC_QBUF Failed\n");
            return ret;
        }

	}

	return 0;

}

int startStreaming()
{
	int ret = UNKNOWN_ERROR;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  	ret = ioctl (g_nCamFD, VIDIOC_STREAMON, &type);
	if (ret < 0)
	{
		printf("StartStreaming: Unable to start capture: %s", strerror(errno));
		return ret;
	}

	return 0;
}


int getOneFrame(v4l2_buffer *buf)
{
	int ret = UNKNOWN_ERROR;

	buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf->memory = V4L2_MEMORY_MMAP;

	ret = ioctl(g_nCamFD, VIDIOC_DQBUF,buf);
	if(ret < 0)
	{
		printf("Get Frame: VIDIOC_DQBUF Failed!\n");
		return __LINE__;
	}

	return 0;
}


int main()
{
	int ret = -1;

	//Step0. Open device
	if(0 != openDevice())
	{
		printf("open device error!\n");
		return -1;
	}
	else
	{
		printf("open device success with fd is %d!\n",g_nCamFD);
	}

	//Step 0.1 Open then set input
	struct v4l2_input inp;
	inp.index = 0;
	if(-1 == ioctl(g_nCamFD, VIDIOC_S_INPUT,&inp))
	{
		printf("VIDIOC_S_INPUT error!\n");
		return -1;
	}
	else
	{
		printf("VIDIOC_S_INPUT success!\n");
		printf("\t inp.index is %d:\n!",inp.index);
		printf("\t inp.name[32] is %s!\n",inp.name);
		printf("\t inp.type is %d!\n",inp.type);
		printf("\t inp.audioset is %d!\n",inp.audioset);
		printf("\t inp.tuner is %d!\n",inp.tuner);
		printf("\t inp.std is %d!\n",(int)inp.std);
		printf("\t inp.status is %d!\n",inp.status);
		//printf("\t inp.capabilities is %d!\n",inp.capabilities);
		sleep(3);
	}
	
	//step0.2: Check capability
	if(0 != checkCapbility())
	{
		printf("Capbility problem~ pls check\n");
		return -1;	
	}
	//Step 0.3 try to support this format: NV21, YUYV
	//we do not support mjpeg camera now
	if(0 == tryFmt(V4L2_PIX_FMT_NV21))
	{
		g_nCaptureFmt = V4L2_PIX_FMT_NV21;
		printf("capture format: V4L2_PIX_FMT_NV21\n");
	}
	else if(tryFmt(V4L2_PIX_FMT_YUYV) == 0)
	{
		g_nCaptureFmt = V4L2_PIX_FMT_YUYV;		// maybe usb camera
		printf("capture format: V4L2_PIX_FMT_YUYV\n");
	}
	else
	{
		printf("driver should surpport NV21/NV12 or YUYV format, but it not!\n");
		return -1;
	}
	
	sleep(1);


	//Step SettingX: set capture mode from V4L2CameraDevice.cpp line 330
	struct v4l2_streamparm params;
	params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	params.parm.capture.timeperframe.denominator = 1;
	params.parm.capture.timeperframe.numerator = 15;
	//params.parm.capture.capturemode = V4L2_MODE_VIDEO;//can't find defination?
	ret = ioctl(g_nCamFD,VIDIOC_S_PARM,&params);
	if(ret < 0)
	{
		printf("setCaptureParams failed with ret is %d!\n",ret);
		return ret;
	}
	else if(0 == ret)
	{
		//Get the value to check
		printf("ret of VIDIOC_S_PARM is %d!\n",ret);
		ret = ioctl(g_nCamFD,VIDIOC_G_PARM,&params);
		if(0 == ret)
		{
			int nGetNumerator = params.parm.capture.timeperframe.numerator;
			int nGetDenominator = params.parm.capture.timeperframe.denominator;
			if(30 == nGetNumerator && 1 == nGetDenominator)
			{
				printf("v4l2setCaptureParams ok!\n");
			}
			else
			{
				printf("eric: numerator/denominator:%d/%d\n",nGetNumerator,nGetDenominator);
				sleep(5);
			}
		}
	}

	//Step GettingX: Get pix_Fmt //NOT ESSENSITIAL!
	struct v4l2_format Format;
	if(g_nCamFD == 0)
		return -1;
	memset(&Format, 0, sizeof(struct v4l2_format));
	Format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(g_nCamFD, VIDIOC_G_FMT, &Format) < 0)
	{
		printf("\nGet format failed\n");
		return -1;
	}


	//Step SettingX: Format
	struct v4l2_format format;
	memset(&format,0,sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width  = 640;
    format.fmt.pix.height = 480;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_NV21;//
	format.fmt.pix.field = (v4l2_field)1;
	ret = ioctl(g_nCamFD, VIDIOC_S_FMT, &format);
	if (ret < 0)
	{
		printf("VIDIOC_S_FMT Failed: %s\n", strerror(errno));
		return ret;
	}
	else
	{
		printf("set fmt right!\n");
	}

	//Step SettingX: Abs Exposure
	set_abs_exposure(900);
	//sleep(5);

	////////////////////////////////////////////////////////////////////////
	//Step1: Request buffer
	struct v4l2_requestbuffers rb;

	//clear
	memset(&rb,0,sizeof(rb));
	rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	rb.memory = V4L2_MEMORY_MMAP;
	rb.count = BUFFER_COUNT;

	ret = ioctl(g_nCamFD, VIDIOC_REQBUFS, &rb);
	printf("after ret = ioctl(g_nCamFD, VIDIOC_REQBUFS, &rb)\n");
	if(ret < 0)
	{
		printf("Init: VIDIOC_REQBUFS failed: %s", strerror(errno));
		return ret;
	}
	else
	{
		printf("Init: VIDIOC_REQBUFS sucess!\n");
	}

	if(BUFFER_COUNT != rb.count)
	{
		//should change the BUFFER_COUNT

		printf("rb.count is: %d",rb.count);
	}


	////////////////////////////////////////////////////////////////////////
	//Step2  query the Buf and map to 
	printf("before queryBuf!\n");
	queryBuf();
	printf("after queryBuf!\n");
	////////////////////////////////////////////////////////////////////////
	//Step3 start Streaming
	startStreaming();
	printf("after startStreaming!\n");
	////////////////////////////////////////////////////////////////////////
	//Step4 Read Buffer
	v4l2_buffer buf_read;
	int n_buf_index = 0;
	int n_ret = -1;

	int n_try_count = 0;
	int totalFrame = 0;
	char pFrameName[128] = {0};
	
	////////////////////////////////////////////////////////////////////////
	//Step Result: How to Get Data and Use!!!!!
	////////////////////////////////////////////////////////////////////////
	//while(1)
	for(int i = 0; i < 100; ++i)
	{
		memset(&buf_read,0,sizeof(buf_read));
		buf_read.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf_read.memory=V4L2_MEMORY_MMAP;
		buf_read.index= n_buf_index;

		do
		{
			n_ret = ioctl(g_nCamFD, VIDIOC_DQBUF, &buf_read);
			if(-1 == n_ret)
			{
				//printf("Get Frame: VIDIOC_DQBUF Failed!\n");
				usleep(10*1000);
				//if(n_try_count++ > 100)//this break will lead to Re-Que buffer Error!!
					//break;
			}
			else
			{
				totalFrame++;
				printf("get frame successesss!\n");
			}
		}while(-1 == n_ret);

		//if(n_try_count++ > 100)
			//break;

		//////////////////write the //buffer/////////////////////////////
		//if(0 == (i % 10))
		//{
		//	sprintf(pFrameName, "myFrame%d.data", totalFrame);
		//	FILE *pFile = fopen(pFrameName,"wb");
		//	if(pFile != NULL)
		//	{
		//		fwrite ((char*)mMapMem.mem[n_buf_index], mMapMem.length, 1, pFile);
		//		printf("write!\n");
		//		fclose (pFile);
		//	}
		//	else
		//	{
		//		printf("open file error!\n");
		//	}
		//}
		//////////////////write the //buffer/////////////////////////////

		//Re-queue 
		n_ret = ioctl(g_nCamFD,VIDIOC_QBUF,&buf_read);
		if(-1 == n_ret)
		{
			printf("Re-Queue buffer error:%d\n",n_ret);
			return -1;
		}

		n_buf_index = (++n_buf_index) % BUFFER_COUNT;
		n_buf_index = 0;//
	}
	

	//Step End: close the device
	
	//End1.1  Stop streaming
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	n_ret = ioctl(g_nCamFD, VIDIOC_STREAMOFF,&type);
	if(n_ret < 0)
	{
		printf("StopStreaming: Uable to stop capture!\n");
		return n_ret;
	}
	
	//End1.2 Unmap Buf
	for(int i = 0; i < BUFFER_COUNT; i++)
	{
		n_ret = munmap(mMapMem.mem[i],mMapMem.length);
		if(n_ret < 0)
		{
			printf("v4l2CloseBuf Unmap failed!\n");
			return n_ret;
		}
	}

	//End1.3 close the device
	if(0 != g_nCamFD)//close the device
	{
		close(g_nCamFD);
		g_nCamFD = 0;
	}
	
	return 0;
}
