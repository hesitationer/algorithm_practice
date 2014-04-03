#include "../include/pick_up_test_case.h"
void NV21_2_RGB888(unsigned char* nv21, unsigned char* rgb, int w, int h);
int bmp_write(unsigned char *image, int xsize, int ysize, char *filename);


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
}
