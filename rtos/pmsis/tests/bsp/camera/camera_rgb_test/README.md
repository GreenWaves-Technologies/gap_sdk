# CAMERA BYPASS Test

Test all the modes available when using RGB format:
    * full image mode when format = rgb565/555/444/888
    * slice mode when format = rgb565/555/444/888   

Input picture:
    * 255*255*pixel_size
    
    * 2bytes/pixel => pixel value = ((w&0xFF)<<8 | (h&0xFF)) 
    * 3bytes/pixel => pixel value = (((w&0xFF)<<16) | (h&0xFF) <<8 | ((w+h)&0xFF))

    * RGB565: 
            s_r_pix = {r_data_msb[7:3],3'b000};
            s_g_pix = {r_data_msb[2:0],cam_data_i[7:5],2'b00};
            s_b_pix = {cam_data_i[4:0],3'b000};
    * RGB555: 
            s_r_pix = {r_data_msb[6:2],3'b000};
            s_g_pix = {r_data_msb[1:0],cam_data_i[7:5],3'b000};
            s_b_pix = {cam_data_i[4:0],3'b000};
    * RGB444: 
            s_r_pix = {r_data_msb[3:0],4'b0000};
            s_g_pix = {cam_data_i[7:4],4'b0000};
            s_b_pix = {cam_data_i[3:0],4'b0000};
    * RGB888: 
            s_r_pix = r_data_msb;
            s_g_pix = r_data_msb1;
            s_b_pix = cam_data_i;


3'h0 - RGB, 3'h1 - RBG, 3'h2 - GRB, 3'h3 - GBR, 3'h4 - BRG, 3'h5 - BGR

## Test

1. full image:
make clean all run platform=rtl format=rgb565/555/444/888 seq=rgb/rbg/grb/gbr/brg/bgr

2. slice image:
make clean all run platform=rtl format=rgb565/555/444/888 seq=rgb/rbg/grb/gbr/brg/bgr slice=1 

