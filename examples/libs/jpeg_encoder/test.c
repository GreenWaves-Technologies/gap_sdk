/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This is an example of JPEG encoding

#include <pmsis.h>
#include "gaplib/jpeg_encoder.h"
#include "gaplib/ImgIO.h"


static int test_entry()
{
  jpeg_encoder_t enc;
  unsigned int width=324, height=244;
  int image_size = width*height;
  int pgm_header_size;
  uint8_t *image = pmsis_l2_malloc(width*height);

  // First read input image
  if (ReadImageFromFile("../../../imgTest0.pgm", width, height, 1, image, width*height, IMGIO_OUTPUT_CHAR, 0)){
    printf("Error reading input image");
    pmsis_exit(-1);
  }
  
  // Allocate output jpeg image
  uint8_t *jpeg_image = pi_l2_malloc(1024*20);
  if (jpeg_image == NULL)
    return -1;


  // Open JPEG encoder
  printf("Start JPEG encoding\n");

  struct jpeg_encoder_conf enc_conf;

  jpeg_encoder_conf_init(&enc_conf);

  #if RUN_ENCODER_ON_CLUSTER
  enc_conf.flags=JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD;
  #else
  enc_conf.flags=0x0;
  #endif
  enc_conf.width = width;
  enc_conf.height = height;
  
  if (jpeg_encoder_open(&enc, &enc_conf))
    return -1;
  
  if (jpeg_encoder_start(&enc))
    return -1;

  // Get the header so that we can produce full JPEG image
  pi_buffer_t bitstream;
  bitstream.data = jpeg_image;
  bitstream.size = image_size;
  uint32_t header_size, footer_size, body_size;

  if (jpeg_encoder_header(&enc, &bitstream, &header_size))
    return -1;
  
  // Now get the encoded image
  pi_buffer_t buffer;
  buffer.data =image;
  buffer.size = image_size;
  buffer.width = width;
  buffer.height = height;
  bitstream.data = &jpeg_image[header_size];

  pi_perf_conf(1<<PI_PERF_CYCLES);
  pi_perf_start();
  pi_perf_reset();

  if (jpeg_encoder_process(&enc, &buffer, &bitstream, &body_size))
    return -1;

  pi_perf_stop();
  printf("Jpeg encoding done! Performance: %d Cycles\n", pi_perf_read(PI_PERF_CYCLES));


  // An finally get the footer
  bitstream.data = &jpeg_image[body_size + header_size];
  if (jpeg_encoder_footer(&enc, &bitstream, &footer_size))
    return -1;

  int bitstream_size = body_size + header_size + footer_size;

  printf("Encoding done at addr %p size %d bytes\n", jpeg_image, bitstream_size);

  jpeg_encoder_stop(&enc);
  jpeg_encoder_close(&enc);


  // Now flush the image to the workstation using semi-hosting
  printf("Writing jpeg image\n");

  struct pi_fs_conf host_fs_conf;
  pi_fs_conf_init(&host_fs_conf);
  struct pi_device host_fs;

  host_fs_conf.type = PI_FS_HOST;

  pi_open_from_conf(&host_fs, &host_fs_conf);

  if (pi_fs_mount(&host_fs))
    return -1;

  void *File = pi_fs_open(&host_fs, "../../../imgTest0.jpg", PI_FS_FLAGS_WRITE);

  pi_fs_write(File, jpeg_image, bitstream_size);

  int32_t check_sum = 0;
  for(int i=0;i<bitstream_size;i++){
    check_sum+=jpeg_image[i];
  }
  
  if(check_sum==625537)
  //if(check_sum==653653)
  {
    printf("Check sum success! %d \n",check_sum);
    return 0;
  }
  else{
    printf("Check sum not success! %d \n",check_sum);
    return -1;
  } 
}


static void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
