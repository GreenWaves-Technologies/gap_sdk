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
#include "jpeg_constants.h"
#include "jpeg_encoder.h"
#include "cluster.h"
#include "dct.h"
#include "bsp/buffer.h"
#include "gaplib/jpeg_encoder.h"

static inline void queue_init(block_queue_t *queue)
{
  queue->first = NULL;
}

static inline void queue_push(block_queue_t *queue, cl_enc_block_t *block)
{
  if (queue->first)
    queue->last->next = block;
  else
    queue->first = block;
  queue->last = block;
  block->next = NULL;
}

static inline void queue_push_ordered(block_queue_t *queue, cl_enc_block_t *block)
{
  cl_enc_block_t *prev = NULL, *current=queue->first;

  while (current && current->id < block->id)
  {
    prev = current;
    current = current->next;
  }

  if (prev)
    prev->next = block;
  else
    queue->first = block;

  block->next = current;

  if (!current)
    queue->last = block;
}

static inline cl_enc_block_t *queue_pop(block_queue_t *queue)
{
  cl_enc_block_t *first = queue->first;
  queue->first = first->next;
  return first;
}

static inline int queue_is_empty(block_queue_t *queue)
{
  return queue->first == NULL;
}

static inline void list_init(block_list_t *list)
{
  list->first = NULL;
}

static inline void list_push(block_list_t *list, cl_enc_block_t *block)
{
  block->next = list->first;
  list->first = block;
}

static inline cl_enc_block_t *list_pop(block_list_t *list)
{
  cl_enc_block_t *result = list->first;
  list->first = result->next;
  return result;
}

static inline int list_is_empty(block_list_t *queue)
{
  return queue->first == NULL;
}

static __attribute__((noinline)) void AppendBytes_cl(cl_enc_t *enc, unsigned char *current, int flush)
{
  int size = current - &enc->l1_bitstream[enc->l1_bitstream_id][0];

  if (size >= L1_BITSTREAM_SIZE - 4 || flush)
  {
    if (enc->pending_dma)
    {
      pi_cl_dma_cmd_wait(&enc->dma_cmd);
    }

    pi_cl_dma_cmd((uint32_t)enc->current_ext_bitstream, (uint32_t)&enc->l1_bitstream[enc->l1_bitstream_id][0], size, PI_CL_DMA_DIR_LOC2EXT, &enc->dma_cmd);
    if (flush) {
      pi_cl_dma_cmd_wait(&enc->dma_cmd);
      enc->pending_dma = 0;
    }
    else
      enc->pending_dma = 1;
    enc->current_ext_bitstream += size;
    enc->l1_bitstream_id ^= 1;
    enc->current_bitstream = &enc->l1_bitstream[enc->l1_bitstream_id][0];
    *enc->current_bitstream = *current;
  }
  else
  {
    enc->current_bitstream = current;
  }
}

static inline int AppendBits_cl(cl_enc_t *enc, unsigned char *BitStream, unsigned int BitPos, unsigned int Val, int Size)

{
  int result = Size;

  //printf("Append 0x%x size %d at %d\n", Val, Size, BitPos);

#if 1
  unsigned char *current = enc->current_bitstream;
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

  AppendBytes_cl(enc, current, 0);

#else

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
#endif

  return result;
}

static inline void push_ready_block(cl_enc_t *enc)
{
  enc->ready_blocks++;
  //printf("Push ready block %d waiting %lx\n", enc->ready_blocks, enc->ready_blocks_waiting_cores);
  uint32_t waiting_cores = enc->ready_blocks_waiting_cores;
  if (waiting_cores)
  {
    int core = __FL1(waiting_cores);
    enc->ready_blocks_waiting_cores = waiting_cores & ~(1<<core);
    eu_evt_trig_from_id(RT_USER_EVENT, 1<<core);
  }
}

static inline int pop_ready_block(cl_enc_t *enc)
{
  //printf("%ld Pop ready block %d\n", pi_core_id(), enc->ready_blocks);
  while(enc->ready_blocks == 0 && enc->nb_block_done)
  {
    pi_cl_team_critical_exit();
    enc->ready_blocks_waiting_cores |= 1 << pi_core_id();
    eu_evt_maskWaitAndClr(1<<RT_USER_EVENT);
    pi_cl_team_critical_enter();
  }
  //printf("%ld Pop ready block done %d %d\n", pi_core_id(), enc->ready_blocks, enc->nb_block_done);
  if (!enc->nb_block_done)
    return 1;

  enc->ready_blocks--;

  return 0;
}


static int exec_dct(cl_enc_t *enc, cl_enc_block_t *block)
{
  const signed short *FDctFactors = enc->constants->FDCT_FACTORS;
  const unsigned char *QuantLUT = enc->constants->QUANT_TAB_LUMIN;
  short int *DU = block->data;
  //printf("[%p] DCT\n", block);

  dump_du("before DCT", DU);
  Dct8x8(DU, FDctFactors);
  dump_du("after DCU", DU);

  int Vc = DU[0] / QuantLUT[0];
  int Vp;
  block->vc = Vc;

  pi_cl_team_critical_enter();

  block->state = BLOCK_STATE_DCT;

  cl_enc_block_t *prev_du = block->prev_du;
  if (prev_du != NULL)
  {
    if (prev_du->state < BLOCK_STATE_DCT)
    {
      //printf("[%p] DCT BREAK prev %p state %d\n", block, prev_du, prev_du->state);
      prev_du->next_du = block;
      pi_cl_team_critical_exit();
      return 1;
    }

    block->vp = prev_du->vc;
    //printf("[%d] SET VP %d\n", block->id, block->vp);
    if (prev_du->state == BLOCK_STATE_DONE)
    {
      list_push(&enc->free_blocks, prev_du);
      push_ready_block(enc);
    }
    else
    {
      prev_du->next_vc_done = 1;
    }
  }

  pi_cl_team_critical_exit();

  return 0;
}

static void exec_quantization(cl_enc_t *enc, cl_enc_block_t *block)
{
  const unsigned char *ZigZagLUT = enc->constants->ZIGZAG_LUT;
  const unsigned char *QuantLUT = enc->constants->QUANT_TAB_LUMIN;
  short int *DU = block->data;
    //printf("[%p] Quantization\n", block);

  pi_cl_team_critical_enter();

  cl_enc_block_t *next_du = block->next_du;
  if (next_du)
  {
    //printf("[%p] DCT resume %p\n", block, next_du);
    next_du->vp = block->vc;
    //printf("[%d] PREV SET VP %d\n", next_du->id, next_du->vp);
    block->next_du = NULL;
    block->next_vc_done = 1;
    queue_push(&enc->dct_blocks, next_du);
    push_ready_block(enc);
  }
  pi_cl_team_critical_exit();
  int output_index = 0;
  int i, Pos = 1;
  //printf("[%d] USE VP %d\n", block->id, block->vp);

  int V = block->vc - block->vp;
#ifdef DUMP_DU
  short int DU2[64];
  DU2[0] = V;
#endif
  int SymbolSize, SymbolIndex;
  SizeAndIndex(V, &SymbolSize, &SymbolIndex);
  block->output[output_index++] = SymbolSize;
  block->output[output_index++] = SymbolIndex;

  while (Pos < 64)
  {
    int V, ZeroLength;

    for (i = Pos; i < 64; i++)
    {
      signed short du_val = DU[ZigZagLUT[i]];
      V = 0;
      if (du_val)
        V = du_val / QuantLUT[ZigZagLUT[i]];
#ifdef DUMP_DU
        DU2[i] = V;
#endif
        if (V != 0) break;
    }

    if (V)
    {
        int SymbolSize, SymbolIndex;
        int ZeroLength = i - Pos;
        SizeAndIndex(V, &SymbolSize, &SymbolIndex);
        block->output[output_index++] = ZeroLength;
        block->output[output_index++] = SymbolSize;
        block->output[output_index++] = SymbolIndex;
    }
    Pos = i + 1;
  }
  block->output[output_index++] = -1;
  dump_du("after quantization", DU2);
}

static void exec_bitstream(cl_enc_t *enc, cl_enc_block_t *block)
{
  const unsigned short *RLE_DC_LUT = enc->constants->HUFTAB_LUMIN_DC_Code;
  const unsigned char *RLE_DC_Size_LUT = enc->constants->HUFTAB_LUMIN_DC_Size;
  const unsigned short *RLE_AC_LUT = enc->constants->HUFTAB_LUMIN_AC_Code;
  const unsigned char *RLE_AC_Size_LUT = enc->constants->HUFTAB_LUMIN_AC_Size;

  // As only 1 core at the same time can produce the bitstream, loop until we can't
  // produce it anymore to free blocks as fast as possible.
  while(1)
  {
    int RLE_Index, RLE_Size;
    int output_index = 0;
    int BitStreamIndex = enc->BitStreamIndex;
    unsigned char *BitStream = enc->bitstream;

    //printf("[%d] Handling bitstream\n", block->id);

    // First handle AC.
    // Reload symbol info from buffer, as it was produced after quantization
    // and produce the bistream.
    int SymbolSize = block->output[output_index++];
    int SymbolIndex = block->output[output_index++];
    RLE_Index = SymbolSize;
    RLE_Size = RLE_DC_Size_LUT[RLE_Index];
    BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, RLE_DC_LUT[RLE_Index], RLE_Size);
    BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, SymbolIndex, SymbolSize);

    // Then handle DC.
    // Reload symbol info until we read -1
    while(1)
    {
      int ZeroLength = block->output[output_index++];
      if (ZeroLength == -1)
        break;

      int SymbolSize = block->output[output_index++];
      int SymbolIndex = block->output[output_index++];

      int ZRL_Seq = ZeroLength / 16;
      ZeroLength = ZeroLength % 16;

      if (ZRL_Seq)
      {
          BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, ZRL, ZRL_LEN);
          ZRL_Seq--;
      }
      if (ZRL_Seq)
      {
          BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, ZRL, ZRL_LEN);
          ZRL_Seq--;
      }
      if (ZRL_Seq)
      {
          BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, ZRL, ZRL_LEN);
          ZRL_Seq--;
      }
      RLE_Index = (ZeroLength * 16) + SymbolSize;
      RLE_Size = RLE_AC_Size_LUT[RLE_Index];
      BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, RLE_AC_LUT[RLE_Index], RLE_Size);
      BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, SymbolIndex, SymbolSize);
    }

    BitStreamIndex += AppendBits_cl(enc, BitStream, BitStreamIndex, EOB, EOB_LEN);

    pi_cl_team_critical_enter();
    enc->BitStreamIndex = BitStreamIndex;
    enc->nb_block_done--;
    enc->current_bitstream_block++;

    //printf("DONE BLOCK %d\n", enc->nb_block_done);

    if (enc->nb_block_done == 0)
    {
      //printf("PUSH ALL\n");
      eu_evt_trig_from_id(RT_USER_EVENT, 0);
    }

    if (block->next_vc_done)
    {
      block->next_vc_done = 0;
      list_push(&enc->free_blocks, block);
      push_ready_block(enc);
    }
    else
    {
      block->state = BLOCK_STATE_DONE;
    }

    if (queue_is_empty(&enc->bitstream_blocks) || enc->bitstream_blocks.first->id != enc->current_bitstream_block)
    {
      enc->bitstream_lock = 0;
      pi_cl_team_critical_exit();
      break;
    }
    block = queue_pop(&enc->bitstream_blocks);
    pi_cl_team_critical_exit();
  }
}

static void exec_check_bitstream(cl_enc_t *enc, cl_enc_block_t *block)
{
  pi_cl_team_critical_enter();
  if (enc->bitstream_lock || block->id != enc->current_bitstream_block)
  {
    queue_push_ordered(&enc->bitstream_blocks, block);
    pi_cl_team_critical_exit();
    return;
  }
  enc->bitstream_lock = 1;
  pi_cl_team_critical_exit();

  exec_bitstream(enc, block);
}

void process_du_cluster(cl_enc_t *enc, cl_enc_block_t *block, int step)
{
  if (step <= 0)
  {
    if (exec_dct(enc, block))
      return;
  }

  if (step <= 1)
  {
    exec_quantization(enc, block);
  }
  
  exec_check_bitstream(enc, block);
}

static void alloc_du(cl_enc_t *enc, int *x, int *y)
{
  *x = enc->current_du_x;
  *y = enc->current_du_y;
  enc->current_du_x += 8;
  if (enc->current_du_x >= enc->width)
  {
    enc->current_du_x = 0;
    enc->current_du_y += 8;
  }
}

static int check_fetch_block(cl_enc_t *enc)
{
  if (list_is_empty(&enc->free_blocks) || enc->current_du_y >= enc->height || enc->nb_fetched_block == enc->nb_block)
    return -1;

  cl_enc_block_t *block = list_pop(&enc->free_blocks);

  block->id = enc->nb_fetched_block++;
  alloc_du(enc, &block->x, &block->y);
  block->state = BLOCK_STATE_FETCHED;
  block->prev_du = enc->current_du;
  enc->current_du = block;
  if (!block->prev_du)
    block->vp = enc->prev_du;
  pi_cl_team_critical_exit();
  uint32_t ext = enc->image + enc->width * block->y + block->x;

  //printf("[%p] Fetching block (id: %d, x: %d, y: %d, ext: 0x%lx, loc: 0x%lx)\n", block, block->id, block->x, block->y, ext, (uint32_t)block->data);

  pi_cl_dma_cmd_2d(ext, (uint32_t)block->data, 64, enc->width, 8, PI_CL_DMA_DIR_EXT2LOC, &block->dma_cmd);

  pi_cl_team_critical_enter();

  queue_push(&enc->fetched_blocks, block);
  push_ready_block(enc);

  return 0;
}

static void __jpeg_encoder_process_pe_entry(void *arg)
{
  cl_enc_t *enc = (cl_enc_t *)arg;

  while(1)
  {
    cl_enc_block_t *block;

    pi_cl_team_critical_enter();

    if (pop_ready_block(enc))
    {
      pi_cl_team_critical_exit();
      return;
    }

    if (check_fetch_block(enc) == 0)
    {
      pi_cl_team_critical_exit();
    }
    else if (!queue_is_empty(&enc->dct_blocks))
    {
      block = queue_pop(&enc->dct_blocks);
    
      pi_cl_team_critical_exit();

      //printf("[%p] %ld Handling DCT block %d (%d, %d) state %d data %p\n", block, pi_core_id(), block->id, block->x, block->y, block->state, block->data);

      process_du_cluster(enc, block, 1);
    }
    else if (!queue_is_empty(&enc->fetched_blocks))
    {
      block = queue_pop(&enc->fetched_blocks);
    
      pi_cl_team_critical_exit();

      //printf("[%p] %ld Handling fetched block %d (%d, %d) state %d data %p\n", block, pi_core_id(), block->id, block->x, block->y, block->state, block->data);

      //printf("[%p] Waiting DMA\n", block);
      pi_cl_dma_cmd_wait(&block->dma_cmd);

      unsigned char *data = (unsigned char *)block->data;
      signed short *data_s = block->data;

      if (block->y + 8 > enc->height || block->x + 8 > enc->width)
      {
        int x = 8;
        if (block->x + 8 > enc->width)
          x = enc->width - block->x;
        int y = 8;
        if (block->y + 8 > enc->height)
          y = enc->height - block->y;

        for (int i=0; i<8; i++)
        {
          for (int j=0; j<8; j++)
          {
            if (i >= y || j >= x)
            {
              data[i*8+j] = 0x1;
            }
          }      
        }
      }

      for (int i=63; i>=0; i--)
      {
        data_s[i] = ((signed short)data[i] - 128) << 2;
      }

      process_du_cluster(enc, block, 0);
    }
    else
    {
      pi_cl_team_critical_exit();
    }
  }
}

static void __jpeg_encoder_process_cl_entry(void *arg)
{
  jpeg_encoder_t *enc = (jpeg_encoder_t *)arg;
  cl_enc_t cl_enc;
  int retval = 0;

  int pending_bits = enc->pending_bits;
  int nb_bits = pending_bits;

  // Init L1 encoder structure to avoid accessing L2 one from cluster
  cl_enc.blocks = enc->cl_blocks;
  cl_enc.width = enc->width;
  cl_enc.height = enc->height;
  cl_enc.nb_block = ((cl_enc.width + 7) / 8) * ((cl_enc.height + 7) / 8);
  cl_enc.nb_fetched_block = 0;
  cl_enc.image = (uint32_t)enc->cl_image->data;
  cl_enc.current_du_x = enc->current_du_x;
  cl_enc.current_du_y = enc->current_du_y;
  cl_enc.prev_du = enc->prev_du;
  cl_enc.current_du = NULL;
  cl_enc.bitstream_lock = 0;
  cl_enc.BitStreamIndex = nb_bits;
  cl_enc.bitstream = enc->bitstream;
  cl_enc.nb_block_done = cl_enc.nb_block;
  cl_enc.ready_blocks = 0;
  cl_enc.ready_blocks_waiting_cores = 0;
  cl_enc.current_bitstream_block = 0;
  cl_enc.constants = enc->l1_constants;
  cl_enc.l1_bitstream_id = 0;
  cl_enc.current_bitstream = &cl_enc.l1_bitstream[0][0];
  cl_enc.current_ext_bitstream = cl_enc.bitstream;
  cl_enc.pending_dma = 0;

  queue_init(&cl_enc.fetched_blocks);
  queue_init(&cl_enc.dct_blocks);
  queue_init(&cl_enc.bitstream_blocks);
  list_init(&cl_enc.free_blocks);

  // Init all blocks and make them available
  for (unsigned int i=0; i<pi_cl_cluster_nb_pe_cores() * 2; i++)
  {
    cl_enc_block_t *block = &cl_enc.blocks[i];
    block->state = BLOCK_STATE_FREE;
    block->next_du = NULL;
    block->next_vc_done = 0;
    list_push(&cl_enc.free_blocks, block);
    push_ready_block(&cl_enc);
  }

  // Use all of them to fetch data
  pi_cl_team_critical_enter();
  while(1)
  {
    if (check_fetch_block(&cl_enc))
      break;
    else
      pop_ready_block(&cl_enc);
  }
  pi_cl_team_critical_exit();

  // Now start parallel processing
  pi_cl_team_fork(8, __jpeg_encoder_process_pe_entry, &cl_enc);

  // Remember few information so that the encoding can be resumed after cluster
  // has been stopped/started, as the current encoding may have stopped in the middle of the image
  nb_bits = cl_enc.BitStreamIndex;
  enc->prev_du = cl_enc.prev_du;
  enc->current_du_x = cl_enc.current_du_x;
  enc->current_du_y = cl_enc.current_du_y;

  // Flush local output data and take care of the remaining byte so that we can
  // handle it in the next encoding
  unsigned int rounded_nb_bits = nb_bits & ~7;
  unsigned int result = rounded_nb_bits >> 3;

  pending_bits = nb_bits - rounded_nb_bits;

  AppendBytes_cl(&cl_enc, cl_enc.current_bitstream, 1);

  if (pending_bits)
  {
    if (retval == 0)
    {
      cl_enc.bitstream[result] = *cl_enc.current_bitstream << (8 - pending_bits);
      result++;
      pending_bits = 0;
    }
    else
      enc->pending_bitstream_char = *cl_enc.current_bitstream;
  }

  enc->pending_bits = pending_bits;

  // Report the status in the task
  enc->end_task->arg[2] = result;
  enc->end_task->arg[3] = 0;
}

void __jpeg_encoder_process_cl(jpeg_encoder_t *enc, pi_buffer_t *image, pi_buffer_t *encoded_bitstream, pi_task_t *task)
{
  enc->cl_image = image;
  enc->bitstream = encoded_bitstream->data;
  enc->bitstream_size = encoded_bitstream->size;
  enc->end_task = task;

  pi_cluster_task(&enc->cluster_task, __jpeg_encoder_process_cl_entry, enc);

  pi_cluster_send_task_to_cl_async(&enc->cluster_dev, &enc->cluster_task, task);
}



static void __jpeg_encoder_init_constants(void *arg)
{
    jpeg_encoder_t *enc = (jpeg_encoder_t *)arg;
    pi_cl_dma_cmd_t dma_cmd;

    pi_cl_dma_cmd((uint32_t)&JpegConstants, (uint32_t)enc->l1_constants, sizeof(JpegConstants), PI_CL_DMA_DIR_EXT2LOC, &dma_cmd);
    pi_cl_dma_cmd_wait(&dma_cmd);
}



int32_t __jpeg_encoder_start_cl(jpeg_encoder_t *enc)
{
    // Allocate all L1 resources so that the encoding can be quickly launched
    struct pi_cluster_conf conf;

    pi_cluster_conf_init(&conf);

    pi_open_from_conf(&enc->cluster_dev, &conf);
    
    if (pi_cluster_open(&enc->cluster_dev))
      goto error0;

    void *l1_mem = pi_l1_malloc(&enc->cluster_dev, sizeof(cl_enc_block_t) * pi_cl_cluster_nb_pe_cores() * 2 + sizeof(JpegConstants));
    if (l1_mem == NULL)
      goto error1;

    enc->cl_blocks = (void *)((uint32_t)l1_mem + sizeof(JpegConstants));

    enc->l1_constants = l1_mem;

    pi_cluster_send_task_to_cl(&enc->cluster_dev, pi_cluster_task(&enc->cluster_task, __jpeg_encoder_init_constants, enc));

    return 0;

error1:
    pi_cluster_close(&enc->cluster_dev);
error0:
    return -1;
}



void __jpeg_encoder_stop_cl(jpeg_encoder_t *enc)
{
    // Free all L1 resources
    pi_l1_free(&enc->cluster_dev, enc->l1_constants, sizeof(cl_enc_block_t) * pi_cl_cluster_nb_pe_cores() * 2 + sizeof(JpegConstants));
    pi_cluster_close(&enc->cluster_dev);
}