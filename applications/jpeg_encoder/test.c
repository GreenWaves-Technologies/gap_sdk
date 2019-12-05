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
#include <bsp/fs.h>
#include <bsp/flash/hyperflash.h>
#include "ImgIO.h"



static int test_entry()
{
  jpeg_encoder_t enc;
  unsigned int width, height;
  int image_size;
  int pgm_header_size;


  // First read input image
  unsigned char *image = read_image("imgTest0.pgm", &width, &height, &image_size, &pgm_header_size);
  if (image == NULL)
    return -1;


  // Allocate output jpeg image
  uint8_t *jpeg_image = pi_l2_malloc(image_size);
  if (jpeg_image == NULL)
    return -1;


  // Open JPEG encoder
  printf("Start JPEG encoding\n");

  struct jpeg_encoder_conf enc_conf;

  jpeg_encoder_conf_init(&enc_conf);

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
  buffer.data = &image[pgm_header_size];
  buffer.size = image_size - pgm_header_size;
  buffer.width = width;
  buffer.height = height;
  bitstream.data = &jpeg_image[header_size];

  pi_perf_conf(1<<PI_PERF_CYCLES);
  pi_perf_start();
  pi_perf_reset();

  if (jpeg_encoder_process(&enc, &buffer, &bitstream, &body_size))
    return -1;

  pi_perf_stop();

  printf("Done with %d cycles\n", pi_perf_read(PI_PERF_CYCLES));


  // An finally get the footer
  bitstream.data = &jpeg_image[body_size + header_size];
  if (jpeg_encoder_footer(&enc, &bitstream, &footer_size))
    return -1;

  int bitstream_size = body_size + header_size + footer_size;

  printf("Encoding done at addr %p size %d\n", jpeg_image, bitstream_size);

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

  void *File = pi_fs_open(&host_fs, "imgTest0.jpg", PI_FS_FLAGS_WRITE);

  pi_fs_write(File, jpeg_image, bitstream_size);

  return 0;
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
