#include "../include/pick_up_test_case.h"
void NV21_2_RGB888(unsigned char* nv21, unsigned char* rgb, int w, int h);
int bmp_write(unsigned char *image, int xsize, int ysize, char *filename);
void yuv2rgb422(unsigned char *buf_yuv, unsigned char *buf_rgb, int yuv_size);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

ERIC_TEST(v4l2,yuyv2rgb_422)
{
	printf("enter yuyv test\n");
	int w = 640;
	int h = 480;
	int len = w*h*2;
	unsigned char* yuyv = new unsigned char[len];

	char *file_path = "yuyv.bin";
	FILE *fd = fopen(file_path,"rb");
	if(fd == NULL)
	{
		printf("open file %s failed!\n",file_path);
	}

	fread(yuyv, len, 1, fd);
	fclose(fd);

	//cvt
	unsigned char* rgb = new unsigned char[w*h*3];
	yuv2rgb422(yuyv, rgb, w*h);

	bmp_write(rgb,  w,  h, "yuyv");

	delete [] yuyv;
	delete [] rgb;
}

ERIC_TEST(v4l2, nv21)
{
    printf("enter nv21 test\n");
    int w = 640;
    int h = 480;
    int len = w*h*3/2;
    unsigned char* nv21 = new unsigned char[len];

    char *file_path = "/home/eric/workspace_android/CameraSample/jni/raw.bin";
    FILE *fd = fopen(file_path,"rb");
    if(fd == NULL)
    {
        printf("open file %s failed!\n",file_path);
    }

    fread(nv21, len, 1, fd);
    fclose(fd);

    //cvt
    unsigned char* rgb = new unsigned char[w*h*3];
    NV21_2_RGB888(nv21, rgb, w, h);

    bmp_write(rgb,  w,  h, "check");

	delete [] nv21;
	delete [] rgb;
}
