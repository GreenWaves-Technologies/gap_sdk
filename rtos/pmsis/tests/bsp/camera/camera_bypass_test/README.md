# CAMERA BYPASS Test

Test all the modes available when using bypass format:
    * full image mode when pixel size = 1/2/3
    * slice mode when pixel size = 1/2/3   

Input picture:
    * 255*255*pixel_size
    
    * 1byte/pixel => pixel value = (w+h)&0xFF
    * 2bytes/pixel => pixel value = ((w&0xFF)<<8 | (h&0xFF)) 
    * 3bytes/pixel => pixel value = (((w&0xFF)<<16) | (h&0xFF) <<8 | ((w+h)&0xFF))

## Test

1. full image:
make clean all run platform=rtl test=1 pixel=1/2/3 

2. slice image:
make clean all run platform=rtl test=1 pixel=1/2/3 slice=1 

