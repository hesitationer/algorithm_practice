#ifndef V4L2_CAMERA_H
#define V4L2_CAMERA_H

#include <sys/time.h>
#include <linux/videodev2.h>

const int V4L2_BUF_NUMBER = 4;

class V4L2_Camera
{
public:
	V4L2_Camera();
	~V4L2_Camera();

	int open_cam();
	int get_pixel_format();
	int set_pixel_format(int pix_fmt);

	int get_abs_exposure();
	int set_abs_exposure(int exp_value);

	int grab_frame();
	int retrieve_frame(char *&out_pixel_buffer);
	int query_frame(char *&out_pixel_buffer);

private:
	int open_cam_();
	int set_input_();
	int check_capability_();
	int check_support_fmt_(int pix_fmt);
	int request_v4l2buffers_();
	int query_v4l2buffer_();

	int start_streaming_();

private:
	int cam_fd_;
	char cam_name_[255];
	int exposure_value_;
	
	int pixle_format_;
	int img_width_;
	int img_height_;

	//used to visit the mapped mems
	typedef struct v4l2_mem_map
	{
		void *	mem[V4L2_BUF_NUMBER]; 
		int 	length;
	}v4l2_mem_map_t;
	v4l2_mem_map_t map_mem_;

	v4l2_buffer v4l2_buf_read_;

};
#endif
