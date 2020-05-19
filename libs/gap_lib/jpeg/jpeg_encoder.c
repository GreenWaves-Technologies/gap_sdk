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

/*
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <stdio.h>
#include "jpeg_encoder.h"
#include "jpeg_constants.h"
#include "cluster.h"
#include "dct.h"
#include "bsp/buffer.h"
#include "gaplib/jpeg_encoder.h"


unsigned char std_dc_luminance_huff[] =
{
  0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B
};

unsigned char std_ac_luminance_huff[] =
{
  0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03,
  0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
  0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
  0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
  0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08,
  0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
  0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16,
  0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
  0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
  0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
  0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
  0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
  0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
  0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
  0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
  0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
  0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
  0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4,
  0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
  0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
  0xF9, 0xFA
};



#ifdef __pulp__
static unsigned int BitFlip(unsigned int V, int Size)

{
    int i;
    unsigned int V1 = V;
    unsigned int Pos = Size - 1;

    for (i = 0; i < Size; i++)
    {
        V1 = __builtin_pulp_binsert_r(V1, V, Pos);
        Pos--;
        V = V >> 1;
    }
    return V1;

}
#else
#define BIT_INSERT(dst, src, off)     (((dst) & ~(1<<(off))) | (((src) & 1)<<(off)))
static unsigned int BitFlip(unsigned int V, int Size)

{
    int i;
    unsigned int V1 = V;

    for (i = 0; i < Size; i++)
    {
        unsigned int B = (V >> i) & 0x1;
        unsigned int Pos = Size - i - 1;
        V1 = BIT_INSERT(V1, B, Pos);
    }
    return V1;

}
#endif
static char *BitImage(unsigned char *BitStream, int From, int Size, char *Str)

{
    int i, BytePos, BitPos;
    int Pos = 0;
    for (i = From; i < (From + Size); i++)
    {
        BytePos = i / 8;
        BitPos = i % 8;
        if ((BitStream[BytePos] >> BitPos) & 0x1) Str[Pos] = '1';
        else Str[Pos] = '0';
        Pos++;
    }
    Str[Pos] = 0;
    return Str;
}

static int AppendBits(unsigned char *BitStream, unsigned int BitPos, unsigned int Val, int Size)

{
  int result = Size;

  //printf("Append 0x%x size %d at %d\n", Val, Size, BitPos);

#if 1

  unsigned char *current = BitStream + BitPos / 8;
  int done = BitPos % 8;

  if (done)
  {
    int rest = 8 - done;
    if (rest > Size)
      rest = Size;
    Size -= rest;
    *current = ((*current) << rest) | ((Val >> Size) & 0xff);

    if (done + rest < 8)
    {
      return result;
    }

    if (*current == 0xff)
    {
      current++;
      *current = 0x00;
      result += 8;
    }
    current++;
  }
  while (Size >= 8)
  {
    Size -= 8;
    *current = (Val >> Size) & 0xff;
    if (*current == 0xff)
    {
      current++;
      *current = 0x00;
      result += 8;
    }
    current++;

  }

  if (Size)
  {
    *current = (Val) & 0xff;
  }

#else
    unsigned int *WordStream = (unsigned int *) (BitStream + BitPos / 8);
    *WordStream = gap8_bitinsert_r(*WordStream, Val, Size, BitPos % 8);
    printf("PUT BITS %x current word %2.2x pos %d/%d len %d\n", Val, (unsigned char)*WordStream, BitPos / 8, BitPos % 8, Size);
#endif

  return result;
}



int process_du(
    short int *DU,
    const signed short *FDctFactors,
    const unsigned char *ZigZagLUT,
    const unsigned char *QuantLUT,
    const unsigned short *RLE_DC_LUT,
    const unsigned char *RLE_DC_Size_LUT,
    const unsigned short *RLE_AC_LUT,
    const unsigned char *RLE_AC_Size_LUT,
    unsigned char *BitStream,
    short int *PrevDU,
    int BitStreamIndex)
{
#ifdef DUMP_DU
  short int DU2[64];
#endif
    int i, Pos = 1;
    int BitStreamIndex_param = BitStreamIndex;

    dump_du("before DCT", DU);

    Dct8x8(DU, FDctFactors);

    dump_du("after DCU", DU);

    {
        int Vc = DU[0] / QuantLUT[0];
        int Vp = PrevDU ? PrevDU[0] : 0;

        int V = Vc - Vp;
        *PrevDU = Vc;
#ifdef DUMP_DU
        DU2[0] = V;
#endif

        int RLE_Index, RLE_Size, SymbolSize, SymbolIndex;

        SizeAndIndex(V, &SymbolSize, &SymbolIndex);
        RLE_Index = SymbolSize;
        RLE_Size = RLE_DC_Size_LUT[RLE_Index];
        BitStreamIndex += AppendBits(BitStream, BitStreamIndex, RLE_DC_LUT[RLE_Index], RLE_Size);
        BitStreamIndex += AppendBits(BitStream, BitStreamIndex, SymbolIndex, SymbolSize);
    }

    while (Pos < 64)
    {
        int V, ZeroLength;

        for (i = Pos; i < 64; i++)
        {
            V = DU[ZigZagLUT[i]] / QuantLUT[ZigZagLUT[i]];

#ifdef DUMP_DU
           DU2[i] = V;
#endif

            if (V != 0) break;
        }

        /* [Pos .. i-1] = 0, i non zero Item */
        if (V)
        {
            int RLE_Index, RLE_Size, SymbolSize, SymbolIndex;
            int ZeroLength = i - Pos;
            int ZRL_Seq = ZeroLength / 16;
            ZeroLength = ZeroLength % 16;

            if (ZRL_Seq)
            {
                BitStreamIndex += AppendBits(BitStream, BitStreamIndex, ZRL, ZRL_LEN);
                ZRL_Seq--;
            }
            if (ZRL_Seq)
            {
                BitStreamIndex += AppendBits(BitStream, BitStreamIndex, ZRL, ZRL_LEN);
                ZRL_Seq--;
            }
            if (ZRL_Seq)
            {
                BitStreamIndex += AppendBits(BitStream, BitStreamIndex, ZRL, ZRL_LEN);
                ZRL_Seq--;
            }
            SizeAndIndex(V, &SymbolSize, &SymbolIndex);
            RLE_Index = (ZeroLength * 16) + SymbolSize;
            RLE_Size = RLE_AC_Size_LUT[RLE_Index];
            BitStreamIndex += AppendBits(BitStream, BitStreamIndex, RLE_AC_LUT[RLE_Index], RLE_Size);
            BitStreamIndex += AppendBits(BitStream, BitStreamIndex, SymbolIndex, SymbolSize);
        }
        else
        {
            BitStreamIndex += AppendBits(BitStream, BitStreamIndex, EOB, EOB_LEN);
            /* End Of Block */
            break;
        }
        Pos = i + 1;
    }

    dump_du("after quantization", DU2);

    return BitStreamIndex - BitStreamIndex_param;
}

#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
#define FIXQ 11

void rgb_to_yuv(unsigned char r, unsigned char g, unsigned char b, int *y, int *u, int *v)
{
    *y = FLOAT2FIX( 0.2990f) * r + FLOAT2FIX(0.5870f) * g + FLOAT2FIX(0.1140f) * b - (128 << FIXQ);
    //*u = FLOAT2FIX(-0.1678f) * r - FLOAT2FIX(0.3313f) * g + FLOAT2FIX(0.5000f) * b;
    //*v = FLOAT2FIX( 0.5000f) * r - FLOAT2FIX(0.4187f) * g - FLOAT2FIX(0.0813f) * b;
    *y >>= FIXQ - 2;
    //*u >>= FIXQ - 2;
    //*v >>= FIXQ - 2;
}

void gray_to_y(int r, int *y)
{
  #if 0
    *y = FLOAT2FIX(1.0f) * r - (128 << FIXQ);
    *y >>= FIXQ - 2;
  #else
    *y = (r - 128) << 2;
  #endif
}


int32_t jpeg_encoder_process_status(uint32_t *encoded_size, pi_task_t *task)
{
  *encoded_size = task->arg[2];
  return task->arg[3];
}

void jpeg_encoder_process_fc(jpeg_encoder_t *enc, pi_buffer_t *image, pi_buffer_t *encoded_bitstream, pi_task_t *task)
{
  int image_size = image->width * image->height;
  unsigned int bitstream_nb_bits = encoded_bitstream->size*8;
  unsigned char *data = image->data;
  unsigned int width = image->width;
  unsigned int height = image->height;
  int BitStreamIndex = 0;
  unsigned char *bitstream = encoded_bitstream->data;
  int retval = 1;

  if (enc->pending_bits)
  {
    *bitstream = enc->pending_bitstream_char;
    BitStreamIndex = enc->pending_bits;
  }

  while (bitstream_nb_bits >= MAX_BYTES_IN_DU*8)
  {
    short int du[64];

    if (enc->current_du_x == 0 && enc->current_du_y == 0)
      enc->prev_du = 0;

    for (int i=0; i<8; i++)
    {
      for (int j=0; j<8; j++)
      {
        if (enc->current_du_x + j < width && enc->current_du_y + i < height)
          du[i*8 + j] = data[(enc->current_du_y + i)*width + enc->current_du_x + j];
        else
          du[i*8 + j] = 0x00;

#if 1
        int val;
        gray_to_y(du[i*8 + j], &val);
        du[i*8 + j] = val;
#endif

      }
    }

    //printf("Handling block (%d, %d)\n", enc->current_du_x, enc->current_du_y);

    int du_bitstream_nb_bits = process_du(
      du,
      JpegConstants.FDCT_FACTORS,
      JpegConstants.ZIGZAG_LUT,
      JpegConstants.QUANT_TAB_LUMIN,
      JpegConstants.HUFTAB_LUMIN_DC_Code,
      JpegConstants.HUFTAB_LUMIN_DC_Size,
      JpegConstants.HUFTAB_LUMIN_AC_Code,
      JpegConstants.HUFTAB_LUMIN_AC_Size,
      bitstream,
      &enc->prev_du,
      BitStreamIndex
    );

    BitStreamIndex += du_bitstream_nb_bits;
    bitstream_nb_bits -= du_bitstream_nb_bits;

    enc->current_du_x += 8;
    if (enc->current_du_x >= width)
    {
      enc->current_du_y += 8;
      enc->current_du_x = 0;
      if (enc->current_du_y >= height)
      {
        enc->current_du_y = 0;
        retval = 0;
        break;
      }
    }
  }

  unsigned int nb_bits = encoded_bitstream->size*8 - bitstream_nb_bits + enc->pending_bits;
  unsigned int rounded_nb_bits = nb_bits & ~7;
  unsigned int result = rounded_nb_bits >> 3;

  enc->pending_bits = nb_bits - rounded_nb_bits;

  if (enc->pending_bits)
  {
    if (retval == 0)
    {
      bitstream[result] = bitstream[result] << (8 - enc->pending_bits);
      result++;
      enc->pending_bits = 0;
    }
    else
      enc->pending_bitstream_char = bitstream[result];
  }

  task->arg[2] = result;
  task->arg[3] = retval;

  pi_task_push(task);
}

void jpeg_encoder_process_async(jpeg_encoder_t *enc, pi_buffer_t *image, pi_buffer_t *encoded_bitstream, pi_task_t *task)
{
  if (enc->flags & JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD)
  {
    __jpeg_encoder_process_cl(enc, image, encoded_bitstream, task);
  }
  else
  {
    jpeg_encoder_process_fc(enc, image, encoded_bitstream, task);
  }
}

int32_t jpeg_encoder_process(jpeg_encoder_t *enc, pi_buffer_t *image, pi_buffer_t *encoded_bitstream, uint32_t *encoded_size)
{
  pi_task_t task;

  jpeg_encoder_process_async(enc, image, encoded_bitstream, pi_task_block(&task));
  pi_task_wait_on(&task);
  return jpeg_encoder_process_status(encoded_size, &task);
}

int32_t jpeg_encoder_open(jpeg_encoder_t *enc, struct jpeg_encoder_conf *conf)
{
  enc->current_du_x = 0;
  enc->current_du_y = 0;
  enc->pending_bits = 0;
  enc->width = conf->width;
  enc->height = conf->height;
  enc->flags = conf->flags;
  enc->prev_du = 0;

  return 0;
}

int32_t jpeg_encoder_start(jpeg_encoder_t *enc)
{
    if (enc->flags & JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD)
    {
        if (__jpeg_encoder_start_cl(enc))
            return -1;
    }

    return 0;
}

void jpeg_encoder_stop(jpeg_encoder_t *enc)
{
    if (enc->flags & JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD)
    {
        __jpeg_encoder_stop_cl(enc);
    }
}

void jpeg_encoder_close(jpeg_encoder_t *enc)
{
}

void jpeg_encoder_conf_init(struct jpeg_encoder_conf *conf)
{
    conf->flags = JPEG_ENCODER_FLAGS_CLUSTER_OFFLOAD;
    conf->width = 0;
    conf->height = 0;
}


int32_t jpeg_encoder_header(jpeg_encoder_t *enc, pi_buffer_t *encoded_bitstream, uint32_t *encoded_size)
{
  int cnt = 0;
  uint8_t *output = encoded_bitstream->data;

  // JFIF header
  // SOI, Start Of Image
  output[cnt++] = 0xFF;
  output[cnt++] = 0xD8;
  // DQT, Define Quantization Table
  // DQT
  output[cnt++] = 0xFF;
  output[cnt++] = 0xDB;
  // Lq(Length) = 2+65 bytes
  output[cnt++] = 0x00;
  output[cnt++] = 0x43;
  // ID:0, Quantization table for luminance
  output[cnt++ ]= 0x00;
  for(int i = 0; i < 64; i++){
    output[cnt++] = JpegConstants.QUANT_TAB_LUMIN[JpegConstants.ZIGZAG_LUT[i]];
  }
  // DHT, Define Huffman Table
  // DHT
  output[cnt++] = 0xFF;
  output[cnt++] = 0xC4;
  // Lh(Length) = 2+29+179 bytes
  output[cnt++] = 0x00;
  output[cnt++] = 0xD2;
  // ID:01, Huffman code table for DC luminance.
  output[cnt++] = 0x00;
  for(uint16_t i = 0; i < sizeof(std_dc_luminance_huff); i++){
    output[cnt++] = std_dc_luminance_huff[i];
  }
  // ID:10, Huffman code table for AC luminance.
  output[cnt++] = 0x10;
  for(uint16_t i = 0; i < sizeof(std_ac_luminance_huff); i++){
    output[cnt++] = std_ac_luminance_huff[i];
  }
  // SOF0, Start Of Frame 0
  // SOF0
  output[cnt++] = 0xFF;
  output[cnt++] = 0xC0;
  // Lf(Length) = 11 bytes
  output[cnt++] = 0x00;
  output[cnt++] = 0x0b;
  // Sampling accuracy = 8 bits
  output[cnt++] = 0x08;
  // Height
  //output[cnt++] = (uint8_t)(FRAME_HEIGHT >> 8);
  //output[cnt++] = (uint8_t)(FRAME_HEIGHT & 0xff);

  output[cnt++] = (uint8_t)(enc->height >> 8);
  output[cnt++] = (uint8_t)(enc->height & 0xff);
  // Width
  //output[cnt++] = (uint8_t)(FRAME_WIDTH >> 8);
  //output[cnt++] = (uint8_t)(FRAME_WIDTH & 0xff);
  output[cnt++] = (uint8_t)(enc->width >> 8);
  output[cnt++] = (uint8_t)(enc->width & 0xff);
  // Number of image components = 1(Grayscale)
  output[cnt++] = 0x01;
  // Parameter of specified luminance component (sampling coefficient=2/2, quantization table=0)
  output[cnt++] = 0x00;
  output[cnt++] = 0x22;
  output[cnt++] = 0x00;
  // SOS, Start Of Scan
  // SOS
  output[cnt++] = 0xFF;
  output[cnt++] = 0xDA;
  // Ls(Length) = 8 bytes
  output[cnt++] = 0x00;
  output[cnt++] = 0x08;
  // Number of image components = 1 (grayscale)
  output[cnt++] = 0x01;
  // Parameter of specified luminance component
  output[cnt++] = 0x00;
  output[cnt++] = 0x00;
  // Reserved
  output[cnt++] = 0x00;
  output[cnt++] = 0x3F;
  output[cnt++] = 0x00;

  *encoded_size = cnt;

  return 0;
}

int32_t jpeg_encoder_footer(jpeg_encoder_t *enc, pi_buffer_t *encoded_bitstream, uint32_t *encoded_size)
{
  int cnt = 0;
  uint8_t *output = encoded_bitstream->data;

  // EOI, End Of Image
  output[cnt++] = 0xFF;
  output[cnt++] = 0xD9;

  *encoded_size = cnt;

  return 0;
}
