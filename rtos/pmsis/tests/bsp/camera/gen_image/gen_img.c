#include <stdio.h>
#include <stdlib.h>


// Generate an image for rtl test
// ./a.out test_img0.raw 255 255 0

enum {
    RAW_BAYER = 1,  // GRAY SCALE or RGB RAW, each pixel = 1 byte
    RGB565 = 2,
    RGB555 = 2,
    RGB444 = 2,
    RGB888 = 3
};

int pixel_value (int format, int w, int h)
{
    if (format == RGB565 || format == RGB555 || format == RGB444)
    {
        // construct the image 16bits with (x,y)
        return ((w&0xFF)<<8 | (h&0xFF));
    }
    else if (format == RGB888)
    {
        // construct the image 24 bits with (x, y, z), where z = x+y;
        return (((w&0xFF)<<16) | (h&0xFF) <<8 | ((w+h)&0xFF));
    }
    else
        printf("error: no this format\n");
}


/* main.c */
int main(int argc, char *argv[]) {

    char *file_name = argv[1];
    int img_w = atoi(argv[2]);
    int img_h = atoi(argv[3]);
    int format = atoi(argv[4]);
    int frame_number = atoi(argv[5]);

    FILE * image = fopen(file_name, "w");

    int pixel = 0;

    printf ("the command you are running: \n");
    printf ("./a.out %s, %d, %d, %d, %d\n", file_name, img_w, img_h, format, frame_number);
 
    for (int h=0; h<img_h; h++)
    {
        for (int w=0; w<img_w; w++)
        {
            if(frame_number && w==0 && h==0)
                pixel = frame_number;
            else{
                if (format > 1)
                    pixel = pixel_value(format, w, h); 
                else
                    pixel = (w+h)&0xFF;
            }

            fwrite ((void *) &pixel, 1, format, image);
        }
    }
    fclose(image);
}
