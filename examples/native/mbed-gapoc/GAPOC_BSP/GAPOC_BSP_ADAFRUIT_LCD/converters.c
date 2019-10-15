

void gray8_to_RGB565_crop(unsigned char *input,unsigned short *output,int w, int h,int nw, int nh)
{
  int a=0;
  for(int i=2;i<h-2;i++)
  {
    for(int j=2;j<w-2;j++)
    {
      output[a++]=((input[i*w+j] >> 3 ) << 3) | ((input[i*w+j] >> 5) ) | (((input[i*w+j] >> 2 ) << 13) )|   ((input[i*w+j] >> 3) <<8);
    }
  }

}


void gray8_to_RGB565(unsigned char *input,unsigned short *output,int width, int height)
{

    for(int i=0;i<width*height;i++)
    {
        //output[i] = ((input[i] >> 3 ) << 11) | ((input[i] >> 2) << 5) | (input[i] >> 3);
        //output[i] = output[i] << 8 |  output[i] >> 8;
        output[i] = ((input[i] >> 3 ) << 3) | ((input[i] >> 5) ) | (((input[i] >> 2 ) << 13) )|   ((input[i] >> 3) <<8);
    }

}

void gray8_to_RGB565_upscale(unsigned char *input,unsigned short *output,int width, int height, int upscale_factor)
{
    int pixin_cnt =0, pixout_cnt = 0;
    for (int l=0;l<height;l++)    
    {
        for (int c=0;c<width;c++)   
        {
            // Grey levels to RGB:
            output[pixout_cnt] = ((input[pixin_cnt] >> 3 ) << 3) | ((input[pixin_cnt] >> 5) ) | (((input[pixin_cnt] >> 2 ) << 13) )|   ((input[pixin_cnt] >> 3) <<8);
            pixin_cnt++;
            pixout_cnt++;

            // Repeat pixel if needed :
            for ( int rep=0; rep <upscale_factor-1; rep++)  // -1 because we've already created the first pixel of the series
            {
                output[pixout_cnt] = output[pixout_cnt-1];   // current pixel = previous pixel, then incrementpixel index
                pixout_cnt++;
            }
        }

        // Repeat line if needed :
        for ( int rep=0; rep <upscale_factor -1; rep++)  // -1 because we've already created the first line of the series
        {
            for (int pixindex=0; pixindex <width*upscale_factor; pixindex++)   // scan one full line
            {
                output[pixout_cnt] = output[pixout_cnt -(width*upscale_factor) ];   // current pixel = pixel just above
                pixout_cnt++;
            }
        }        
    }

}

