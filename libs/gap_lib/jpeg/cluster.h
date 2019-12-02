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

#ifndef _CLUSTER_H_
#define _CLUSTER_H_


#include "gaplib/jpeg_encoder.h"


#define L1_BITSTREAM_SIZE 256    // Size of output buffer for streaming bistream to the L2


// Block state
typedef enum
{
    BLOCK_STATE_FREE,     // Block is free
    BLOCK_STATE_FETCHED,  // DMA copy for input block has been enqueued
    BLOCK_STATE_DCT,      // DCT has been computed but previous first value is not ready
    BLOCK_STATE_DONE      // Block is finished and is waiting to be released
} cl_enc_block_state_e;


// Structure describing an 8x8 block
typedef struct cl_enc_block_s
{
    int id;                          // Block ID, used to sort-out output bitstream
    int x;                           // Width position of the block
    int y;                           // Height position of the block
    struct cl_enc_block_s *prev_du;  // Previous block in the image (in JTAG order)
    struct cl_enc_block_s *next_du;  // Next block in the image. Only set when next block is waiting for first block value;
    struct cl_enc_block_s *next;     // Used for linked-list
    cl_enc_block_state_e state;      // Block state
    short int vc;                    // Block first quantized value
    short int vp;                    // Previous block first quantized value
    int next_vc_done;                // Next block got first quantized value, which means this block can be released
    short int data[64];              // Block data
    int output[64*3];                // Block output data. Contains bitstream symbols which should be encoded.
    pi_cl_dma_cmd_t dma_cmd;         // DMA node.
} cl_enc_block_t;


// Typedef for chaining blocks in FIFO
typedef struct
{
  cl_enc_block_t *first;     // First block of the queue
  cl_enc_block_t *last;      // Last block of the queue
} block_queue_t;


// Typedef for chaining block in LIFO
typedef struct
{
  cl_enc_block_t *first;    // First block of the queue
} block_list_t;


// Encoder structure on cluster side
typedef struct
{
  block_queue_t fetched_blocks;               // List of blocks for which DMA transfer for input data has been queued and are ready for DCT
  block_queue_t dct_blocks;                   // List of blocks for which DCT has been computed and are waiting for previous block first quantized data
  block_queue_t bitstream_blocks;             // List of blocks for which quantization has been done and are ready for bitstream encoding. This list is ordered with the block ID to get the output bistream in right order
  block_list_t free_blocks;                   // List of blocks ready to be used
  cl_enc_block_t *blocks;                     // List of blocks allocated in L1
  cl_enc_block_t *current_du;                 // Last block for which DMA transfer for input data has been queued. Mainly used to chain blocks together to exchanged first quantized value
  unsigned int nb_block;                      // Number of blocks to process
  unsigned int nb_fetched_block;              // Number of blocks already fetched
  unsigned int nb_block_done;                 // Number of blocks done
  int width;                                  // Image width
  int height;                                 // Image height
  uint32_t image;                             // Image address in L2
  int current_du_x;                           // x position of next block to fetch
  int current_du_y;                           // y position of next block to fetch
  uint8_t prev_du;                            // First quantized value of last processed block in the previous encode wave. Used to feed first block of this wave
  int bitstream_lock;                         // Set to 1 when a core is producing the bitstream so that only one core is doing it at the same time
  uint8_t *bitstream;                         // Output bitstream in L2
  int BitStreamIndex;                         // Current position in output bitstream
  int ready_blocks;                           // Number of ready blocks, waiting for a core to process them. This is used to wakeup the cores when a block is ready
  uint32_t ready_blocks_waiting_cores;        // Mask of cores waiting for a ready block
  int current_bitstream_block;                // ID of the next block for which the output bitstream should be produced. This is used to sort-out the blocks for the output bitstream
  JPG_CST *constants;                         // JPEG constants in L2
  uint8_t l1_bitstream[2][L1_BITSTREAM_SIZE]; // Temporary double-buffering in L1 for output bitstream.
  int l1_bitstream_id;                        // Current L1 buffer index used for output bitstream (0 or 1).
  unsigned char *current_bitstream;           // Current position in L1 buffer used for output bitstream (0 or 1).
  unsigned char *current_ext_bitstream;       // Current position in L2 output bistream
  pi_cl_dma_cmd_t dma_cmd;                    // DMA node used for output bistream copy
  int pending_dma;                            // 1 if a DMA copy is pending for output bistream
} cl_enc_t;


// Encode a part of an image on cluster side
void __jpeg_encoder_process_cl(jpeg_encoder_t *enc, pi_buffer_t *image, pi_buffer_t *encoded_bitstream, pi_task_t *task);


// Allocate resources for encoding images on cluster side
int32_t __jpeg_encoder_start_cl(jpeg_encoder_t *enc);


// Free resources once cluster side encoding is no more needed
void __jpeg_encoder_stop_cl(jpeg_encoder_t *enc);

#endif