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

#include <pmsis.h>
#include "gaplib/jpeg_encoder.h"
#include <bsp/fs.h>
#include <bsp/flash/hyperflash.h>

static jpeg_encoder_t enc;
static unsigned char *image;

static unsigned int SkipComment(unsigned char *Img, unsigned int Ind)

{
  static int Debug=0;
  while (Img[Ind] == '#') {
    while (Img[Ind] != '\n') {if(Debug) printf("%c", Img[Ind]);Ind++;}
    Ind++;
  }
  return Ind;
}

static unsigned int ReadPPMHeader(unsigned char *ImgIn, unsigned int *W, unsigned int *H, unsigned int *IsRGB)

{
#define IS_DIGIT(C) (((C) >= '0') && ((C) <= '9'))
  unsigned int Val, Ind = 0;


  if      (ImgIn[0] == 'P' && ImgIn[1] == '5' && ImgIn[2] == '\n') *IsRGB = 0;
  else if (ImgIn[0] == 'P' && ImgIn[1] == '6' && ImgIn[2] == '\n') *IsRGB = 1;
  else return 0;
  Ind = 3;

  Ind=SkipComment(ImgIn, Ind);
  while (!IS_DIGIT(ImgIn[Ind])) Ind++;
    Val = 0;
  while (IS_DIGIT(ImgIn[Ind])) {
    Val = Val*10 + (ImgIn[Ind] - 0x30);
    Ind++;
  }
  *W = Val;
  Ind=SkipComment(ImgIn, Ind);
  while (!IS_DIGIT(ImgIn[Ind])) Ind++;
    Val = 0;
  while (IS_DIGIT(ImgIn[Ind])) {
    Val = Val*10 + (ImgIn[Ind] - 0x30);
    Ind++;
  }
  *H = Val;

  Ind=SkipComment(ImgIn, Ind);
  while (!IS_DIGIT(ImgIn[Ind])) Ind++;
    Val = 0;
  while (IS_DIGIT(ImgIn[Ind])) {
    Val = Val*10 + (ImgIn[Ind] - 0x30);
    Ind++;
  }
  if (Val != 255) return 0;
  while (ImgIn[Ind] != 0xA) Ind++;

  return (Ind+1);

#undef IS_DIGIT
}

static int test_entry()
{
  struct pi_device fs;
  struct pi_device flash;
  struct pi_fs_conf conf;
  struct pi_hyperflash_conf flash_conf;
  unsigned int width, height, is_rgb;

  printf("Opening FS\n");

  pi_hyperflash_conf_init(&flash_conf);
  pi_open_from_conf(&flash, &flash_conf);

  if (pi_flash_open(&flash))
    return -1;
  
  pi_fs_conf_init(&conf);
  conf.flash = &flash;
  pi_open_from_conf(&fs, &conf);

  if (pi_fs_mount(&fs))
    return -2;

  printf("Opening input image\n");

  pi_fs_file_t *file = pi_fs_open(&fs, "imgTest0.pgm", 0);
  if (file == NULL) return -3;

  image = pi_l2_malloc(file->size);
  if (image == NULL)
    return -1;

  if (pi_fs_read(file, image, file->size) != (int)file->size)
    return -1;

  int pgm_header_size = ReadPPMHeader(image, &width, &height, &is_rgb);
  if (pgm_header_size == 0)
    return -1;

  printf("Read image header (with: %d, height: %d, is_rgb: %d)\n", width, height, is_rgb);

  uint8_t *jpeg_image = pi_l2_malloc(file->size);
  if (jpeg_image == NULL)
    return -1;

  struct jpeg_encoder_conf enc_conf;

  jpeg_encoder_conf_init(&enc_conf);

  enc_conf.flags = JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD;
  enc_conf.width = width;
  enc_conf.height = height;

  printf("Start JPEG encoding\n");

  if (jpeg_encoder_open(&enc, &enc_conf))
    return -1;

  if (jpeg_encoder_start(&enc))
    return -1;

  pi_buffer_t bitstream;
  bitstream.data = jpeg_image;
  bitstream.size = file->size;
  uint32_t header_size, footer_size, body_size;

  if (jpeg_encoder_header(&enc, &bitstream, &header_size))
    return -1;

  pi_buffer_t buffer;
  buffer.data = &image[pgm_header_size];
  buffer.size = file->size - pgm_header_size;
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

  bitstream.data = &jpeg_image[body_size + header_size];
  if (jpeg_encoder_footer(&enc, &bitstream, &footer_size))
    return -1;

  int bitstream_size = body_size + header_size + footer_size;

  printf("Encoding done at addr %p size %d\n", jpeg_image, bitstream_size);

  jpeg_encoder_stop(&enc);
  jpeg_encoder_close(&enc);

  printf("Opening reference jpeg image\n");

  pi_fs_file_t *jpg_file = pi_fs_open(&fs, "imgTest0.jpg", 0);
  if (jpg_file == NULL) return -3;

  char *jpg_image = pi_l2_malloc(jpg_file->size);
  if (jpg_image == NULL)
    return -1;

  if (pi_fs_read(jpg_file, jpg_image, jpg_file->size) != (int)jpg_file->size)
    return -1;

  printf("Comparing results\n");

  if ((int)jpg_file->size != bitstream_size)
  {
    printf("Wrong bitstream size (expected: %d, got: %d)\n", jpg_file->size, bitstream_size);
    return -1;
  }

  unsigned char *output = jpeg_image;
  unsigned char *ref = (unsigned char *)jpg_image;

  for (int i=0; i<bitstream_size; i++)
  {
    if (output[i] != ref[i])
    {
      printf("Error detected at index %d, expected 0x%2x, got 0x%2x\n", i, ref[i], output[i]);
      return -1;
    }
  }


  printf("SUCCESS\n");

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
