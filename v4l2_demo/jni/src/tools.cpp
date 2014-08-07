#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//u = YUVdata[UPOS] - 128;
//v = YUVdata[VPOS] - 128;
//rdif = v + ((v * 103) >> 8);
//invgdif = ((u * 88) >> 8) +((v * 183) >> 8);
//bdif = u +( (u*198) >> 8);
//r = YUVdata[YPOS] + rdif;
//g = YUVdata[YPOS] - invgdif;
//b = YUVdata[YPOS] + bdif;

short redAdjust[] = {
-161,-160,-159,-158,-157,-156,-155,-153,
-152,-151,-150,-149,-148,-147,-145,-144,
-143,-142,-141,-140,-139,-137,-136,-135,
-134,-133,-132,-131,-129,-128,-127,-126,
-125,-124,-123,-122,-120,-119,-118,-117,
-116,-115,-114,-112,-111,-110,-109,-108,
-107,-106,-104,-103,-102,-101,-100, -99,
 -98, -96, -95, -94, -93, -92, -91, -90,
 -88, -87, -86, -85, -84, -83, -82, -80,
 -79, -78, -77, -76, -75, -74, -72, -71,
 -70, -69, -68, -67, -66, -65, -63, -62,
 -61, -60, -59, -58, -57, -55, -54, -53,
 -52, -51, -50, -49, -47, -46, -45, -44,
 -43, -42, -41, -39, -38, -37, -36, -35,
 -34, -33, -31, -30, -29, -28, -27, -26,
 -25, -23, -22, -21, -20, -19, -18, -17,
 -16, -14, -13, -12, -11, -10,  -9,  -8,
  -6,  -5,  -4,  -3,  -2,  -1,   0,   1,
   2,   3,   4,   5,   6,   7,   9,  10,
  11,  12,  13,  14,  15,  17,  18,  19,
  20,  21,  22,  23,  25,  26,  27,  28,
  29,  30,  31,  33,  34,  35,  36,  37,
  38,  39,  40,  42,  43,  44,  45,  46,
  47,  48,  50,  51,  52,  53,  54,  55,
  56,  58,  59,  60,  61,  62,  63,  64,
  66,  67,  68,  69,  70,  71,  72,  74,
  75,  76,  77,  78,  79,  80,  82,  83,
  84,  85,  86,  87,  88,  90,  91,  92,
  93,  94,  95,  96,  97,  99, 100, 101,
 102, 103, 104, 105, 107, 108, 109, 110,
 111, 112, 113, 115, 116, 117, 118, 119,
 120, 121, 123, 124, 125, 126, 127, 128,
};
short greenAdjust1[] = {
  34,  34,  33,  33,  32,  32,  32,  31,
  31,  30,  30,  30,  29,  29,  28,  28,
  28,  27,  27,  27,  26,  26,  25,  25,
  25,  24,  24,  23,  23,  23,  22,  22,
  21,  21,  21,  20,  20,  19,  19,  19,
  18,  18,  17,  17,  17,  16,  16,  15,
  15,  15,  14,  14,  13,  13,  13,  12,
  12,  12,  11,  11,  10,  10,  10,   9,
   9,   8,   8,   8,   7,   7,   6,   6,
   6,   5,   5,   4,   4,   4,   3,   3,
   2,   2,   2,   1,   1,   0,   0,   0,
   0,   0,  -1,  -1,  -1,  -2,  -2,  -2,
  -3,  -3,  -4,  -4,  -4,  -5,  -5,  -6,
  -6,  -6,  -7,  -7,  -8,  -8,  -8,  -9,
  -9, -10, -10, -10, -11, -11, -12, -12,
 -12, -13, -13, -14, -14, -14, -15, -15,
 -16, -16, -16, -17, -17, -17, -18, -18,
 -19, -19, -19, -20, -20, -21, -21, -21,
 -22, -22, -23, -23, -23, -24, -24, -25,
 -25, -25, -26, -26, -27, -27, -27, -28,
 -28, -29, -29, -29, -30, -30, -30, -31,
 -31, -32, -32, -32, -33, -33, -34, -34,
 -34, -35, -35, -36, -36, -36, -37, -37,
 -38, -38, -38, -39, -39, -40, -40, -40,
 -41, -41, -42, -42, -42, -43, -43, -44,
 -44, -44, -45, -45, -45, -46, -46, -47,
 -47, -47, -48, -48, -49, -49, -49, -50,
 -50, -51, -51, -51, -52, -52, -53, -53,
 -53, -54, -54, -55, -55, -55, -56, -56,
 -57, -57, -57, -58, -58, -59, -59, -59,
 -60, -60, -60, -61, -61, -62, -62, -62,
 -63, -63, -64, -64, -64, -65, -65, -66,
};
short greenAdjust2[] = {
  74,  73,  73,  72,  71,  71,  70,  70,
  69,  69,  68,  67,  67,  66,  66,  65,
  65,  64,  63,  63,  62,  62,  61,  60,
  60,  59,  59,  58,  58,  57,  56,  56,
  55,  55,  54,  53,  53,  52,  52,  51,
  51,  50,  49,  49,  48,  48,  47,  47,
  46,  45,  45,  44,  44,  43,  42,  42,
  41,  41,  40,  40,  39,  38,  38,  37,
  37,  36,  35,  35,  34,  34,  33,  33,
  32,  31,  31,  30,  30,  29,  29,  28,
  27,  27,  26,  26,  25,  24,  24,  23,
  23,  22,  22,  21,  20,  20,  19,  19,
  18,  17,  17,  16,  16,  15,  15,  14,
  13,  13,  12,  12,  11,  11,  10,   9,
   9,   8,   8,   7,   6,   6,   5,   5,
   4,   4,   3,   2,   2,   1,   1,   0,
   0,   0,  -1,  -1,  -2,  -2,  -3,  -4,
  -4,  -5,  -5,  -6,  -6,  -7,  -8,  -8,
  -9,  -9, -10, -11, -11, -12, -12, -13,
 -13, -14, -15, -15, -16, -16, -17, -17,
 -18, -19, -19, -20, -20, -21, -22, -22,
 -23, -23, -24, -24, -25, -26, -26, -27,
 -27, -28, -29, -29, -30, -30, -31, -31,
 -32, -33, -33, -34, -34, -35, -35, -36,
 -37, -37, -38, -38, -39, -40, -40, -41,
 -41, -42, -42, -43, -44, -44, -45, -45,
 -46, -47, -47, -48, -48, -49, -49, -50,
 -51, -51, -52, -52, -53, -53, -54, -55,
 -55, -56, -56, -57, -58, -58, -59, -59,
 -60, -60, -61, -62, -62, -63, -63, -64,
 -65, -65, -66, -66, -67, -67, -68, -69,
 -69, -70, -70, -71, -71, -72, -73, -73,
};
short blueAdjust[] = {
-276,-274,-272,-270,-267,-265,-263,-261,
-259,-257,-255,-253,-251,-249,-247,-245,
-243,-241,-239,-237,-235,-233,-231,-229,
-227,-225,-223,-221,-219,-217,-215,-213,
-211,-209,-207,-204,-202,-200,-198,-196,
-194,-192,-190,-188,-186,-184,-182,-180,
-178,-176,-174,-172,-170,-168,-166,-164,
-162,-160,-158,-156,-154,-152,-150,-148,
-146,-144,-141,-139,-137,-135,-133,-131,
-129,-127,-125,-123,-121,-119,-117,-115,
-113,-111,-109,-107,-105,-103,-101, -99,
 -97, -95, -93, -91, -89, -87, -85, -83,
 -81, -78, -76, -74, -72, -70, -68, -66,
 -64, -62, -60, -58, -56, -54, -52, -50,
 -48, -46, -44, -42, -40, -38, -36, -34,
 -32, -30, -28, -26, -24, -22, -20, -18,
 -16, -13, -11,  -9,  -7,  -5,  -3,  -1,
   0,   2,   4,   6,   8,  10,  12,  14,
  16,  18,  20,  22,  24,  26,  28,  30,
  32,  34,  36,  38,  40,  42,  44,  46,
  49,  51,  53,  55,  57,  59,  61,  63,
  65,  67,  69,  71,  73,  75,  77,  79,
  81,  83,  85,  87,  89,  91,  93,  95,
  97,  99, 101, 103, 105, 107, 109, 112,
 114, 116, 118, 120, 122, 124, 126, 128,
 130, 132, 134, 136, 138, 140, 142, 144,
 146, 148, 150, 152, 154, 156, 158, 160,
 162, 164, 166, 168, 170, 172, 175, 177,
 179, 181, 183, 185, 187, 189, 191, 193,
 195, 197, 199, 201, 203, 205, 207, 209,
 211, 213, 215, 217, 219, 221, 223, 225,
 227, 229, 231, 233, 235, 238, 240, 242,
};

unsigned char clip(int val)
{
    if(val > 255)
    {
        return 255;
    }
    else if(val > 0)
    {
        return val;
    }
    else
    {
        return 0;
    }
}
void yuv422_2_rgb565(unsigned char *buf_yuv422, unsigned short *buf_rgb565, 
        int nWidth, int nHeight)
{
    short y1, y2, u, v,y3,y4,u_,v_;
    unsigned char *pYUV = buf_yuv422;
    unsigned short *pRGB = buf_rgb565;
    int count = nWidth * nHeight / 8;
    for(int i = 0; i < count; i++)
    {
        y1 = *pYUV++ ;
        u = *pYUV++ ;
        y2 = *pYUV++ ;
        v = *pYUV++ ;

        y3 = *pYUV++ ;
        u_ = *pYUV++ ;
        y4 = *pYUV++ ;
        v_ = *pYUV++ ;

        short r1 = clip(y1 + redAdjust[v]);
        short g1 = clip(y1 + greenAdjust1[u] + greenAdjust2[v]);
        short b1 = clip(y1 + blueAdjust[u]);
        short r2 = clip(y2 + redAdjust[v]);
        short g2 = clip(y2 + greenAdjust1[u] + greenAdjust2[v]);
        short b2 = clip(y2 + blueAdjust[u]);

        short r3 = clip(y3 + redAdjust[v_]);
        short g3 = clip(y3 + greenAdjust1[u_] + greenAdjust2[v_]);
        short b3 = clip(y3 + blueAdjust[u_]);
        short r4 = clip(y4 + redAdjust[v_]);
        short g4 = clip(y4 + greenAdjust1[u_] + greenAdjust2[v_]);
        short b4 = clip(y4 + blueAdjust[u_]);

        //24bit  --> 16bit
        *pRGB++ = (unsigned short)( ((r1 << 8) & 0xF800) |
                ((g1 << 3) & 0x7E0)  |  ((b1 >> 3)) );

        *pRGB++ = (unsigned short)( ((r2 << 8) & 0xF800) |
                ((g2 << 3) & 0x7E0)  |  ((b2 >> 3)) );

        *pRGB++ = (unsigned short)( ((r1 << 8) & 0xF800) |
                ((g1 << 3) & 0x7E0)  |  ((b1 >> 3)) );

        *pRGB++ = (unsigned short)( ((r2 << 8) & 0xF800) |
                ((g2 << 3) & 0x7E0)  |  ((b2 >> 3)) );
    }
    //rgb565 = (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}
void yuv2rgb422(unsigned char *buf_yuv, unsigned char *buf_rgb, int yuv_size)
{
    short y1, y2, u, v;
    unsigned char *pYUV = buf_yuv;
    unsigned char *pBGR = buf_rgb; //NOTE the order: B, G, R for BMP
    int count = yuv_size / 2;
    for(int i = 0; i < count; i++)
    {
        y1 = *pYUV++ ;
        u = *pYUV++ ;
        y2 = *pYUV++ ;
        v = *pYUV++ ;
        *pBGR++ = clip(y1 + blueAdjust[u]);
        *pBGR++ = clip(y1 + greenAdjust1[u] + greenAdjust2[v]);
        *pBGR++ = clip(y1 + redAdjust[v]);

        *pBGR++ = clip(y2 + blueAdjust[u]);
        *pBGR++ = clip(y2 + greenAdjust1[u] + greenAdjust2[v]);
        *pBGR++ = clip(y2 + redAdjust[v]);
    }
    //rgb565 = (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

void NV21_2_RGB888(unsigned char* nv21, unsigned char* rgb, int w, int h)
{
    unsigned char *pUV = nv21 + w*h;
    unsigned char *pRGB = rgb;

    for(int row = 0; row < h; row = row + 2)
    {
        for(int col = 0; col < w; col = col + 2)
        {
            unsigned char y00 = nv21[w*row + col];
            unsigned char y01 = nv21[w*row + col + 1];
            unsigned char y10 = nv21[w*(row + 1) + col];
            unsigned char y11 = nv21[w*(row + 1) + col + 1];

            unsigned char u = pUV[w*(row/2) + col];
            unsigned char v = pUV[w*(row/2) + col + 1];

            unsigned char* rgb00 = rgb + 3*w*row + 3*col; 
            unsigned char* rgb01 = rgb00 + 3; 
            unsigned char* rgb10 = rgb + 3*w*(row+1) + 3*col; 
            unsigned char* rgb11 = rgb10 + 3; 

            rgb00[0] = clip(y00 + redAdjust[v]);
            rgb00[1] = clip(y00 + greenAdjust1[u] + greenAdjust2[v]);
            rgb00[2] = clip(y00 + blueAdjust[u]);

            rgb01[0] = clip(y01 + redAdjust[v]);
            rgb01[1] = clip(y01 + greenAdjust1[u] + greenAdjust2[v]);
            rgb01[2] = clip(y01 + blueAdjust[u]);

            rgb10[0] = clip(y10 + redAdjust[v]);
            rgb10[1] = clip(y10 + greenAdjust1[u] + greenAdjust2[v]);
            rgb10[2] = clip(y10 + blueAdjust[u]);

            rgb11[0]     = clip(y11 + redAdjust[v]);
            rgb11[1] = clip(y11 + greenAdjust1[u] + greenAdjust2[v]);
            rgb11[2] = clip(y11 + blueAdjust[u]);
        }
    }
}
void YUV422toRGB888(int width, int height, unsigned char *src, unsigned char *dst)
{
    int line, column;
    unsigned char *py, *pu, *pv;
    unsigned char *tmp = dst;
    
    py = src;
    pu = src + 1;
    pv = src + 3;

#define CLIP(x) ( (x)>=0xFF ? 0xFF : ( (x) <= 0x00 ? 0x00 : (x) ) )
    int cnt=0;
    for(line = 0; line < height; ++line)
    {
        for(column = 0; column < width; ++column)
        {
            //*tmp++ = CLIP((double)*py + 1.402*((double)*pv-128.0));
            //*tmp++ = CLIP((double)*py - 0.344*((double)*pu-128.0) - 0.714*((double)*pv-128.0));
            //*tmp++ = CLIP((double)*py + 1.772*((double)*pu-128.0));

            //*tmp++ =  CLIP((double)*py + 1.772*((double)*pu-128.0));
            //*tmp++ = CLIP((double)*py - 0.344*((double)*pu-128.0) - 0.714*((double)*pv-128.0));
            //*tmp++ =CLIP((double)*py + 1.402*((double)*pv-128.0));


            int u = (int)*pu - 128;
            int v = (int)*pv - 128;

            int rdif = v + ((v * 103) >> 8);
            int invgdif = ((u * 88) >> 8) +((v * 183) >> 8);
            int bdif = u +( (u*198) >> 8);
            *tmp++ =  CLIP(py[0] + rdif);
            *tmp++ = CLIP(py[0] - invgdif);
            *tmp++ =CLIP(py[0] + bdif);

            //for debug
            //*tmp++ =  CLIP(py[0]);
            //*tmp++ = CLIP(pu[0]);
            //*tmp++ =CLIP(pv[0]);

            // increase py every time
            py += 2;
            // increase pu,pv every second time
            if ((column & 1)==1)
            {
                pu += 4;
                pv += 4;
            }
            cnt++;
        }
    }
}

void rgb_2_rgb565(int width, int height, unsigned char* src, unsigned short*
        dst)
{
    //no padding,640/480 is ok
    assert(width/4 == 0);
    assert(height/4 == 0);

    int widht_step = width * 3;//rgb
    for (size_t iRow = 0; iRow < height; ++iRow)
    {
        unsigned char* p_row = src + iRow * widht_step;
        unsigned short *p_565_row = dst + iRow * width;

        for (size_t iCol = 0; iCol < width; ++iCol)
        {
            int red   = (int)p_row[iCol * 3 + 0];   
            int green = (int)p_row[iCol * 3 + 1];   
            int blue  = (int)p_row[iCol * 3 + 2];   

            //unsigned short  B =  (blue  >> 3)        & 0x001F;
            //unsigned short  G = ((green >> 2) <<  5) & 0x07E0; // not <
            //unsigned short  R = ((red   >> 3) << 11) & 0xF800; // not <

            //return (unsigned short int) (R | G | B);

            //24bit  --> 16bit
            p_565_row[iCol] = (unsigned short)( ((red << 8) & 0xF800) |
                    ((green << 3) & 0x7E0)  |  ((blue >> 3)) );
        }
    }

}
int bmp_write(unsigned char *image, int xsize, int ysize, char *filename) 
{
    unsigned char header[54] = {
      0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
        54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0
    };
    
    long file_size = (long)xsize * (long)ysize * 3 + 54;
    header[2] = (unsigned char)(file_size &0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;
    
    long width = xsize;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) &0x000000ff;
    header[20] = (width >> 16) &0x000000ff;
    header[21] = (width >> 24) &0x000000ff;
    
    long height = ysize;
    header[22] = height &0x000000ff;
    header[23] = (height >> 8) &0x000000ff;
    header[24] = (height >> 16) &0x000000ff;
    header[25] = (height >> 24) &0x000000ff;

    char fname_bmp[128];
    sprintf(fname_bmp, "%s.bmp", filename);
    
    FILE *fp;
    if (!(fp = fopen(fname_bmp, "wb"))) 
      return -1;
      
    fwrite(header, sizeof(unsigned char), 54, fp);
    fwrite(image, sizeof(unsigned char), (size_t)(long)xsize * ysize * 3, fp);
    
    fclose(fp);
    return 0;
}
