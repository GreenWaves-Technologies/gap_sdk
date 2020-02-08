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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */


#include "tools/frame_streamer.h"
#include "bsp/transport.h"
#include "gaplib/jpeg_encoder.h"

#define JPEG_BITSTREAM_SIZE (1024)



typedef struct
{
  jpeg_encoder_t encoder;
  pi_buffer_t bitstream;
} frame_streamer_jpeg_t;

typedef struct 
{
  struct pi_transport_header header;
  frame_streamer_open_req_t req;
} frame_streamer_open_req_full_t;


struct frame_streamer_s {
  struct pi_device *transport;
  frame_streamer_format_e format;
  int channel;
  struct pi_transport_header header;
  frame_streamer_open_req_full_t req;
  frame_streamer_jpeg_t *jpeg;
  unsigned int height;
  unsigned int width;
};



int frame_streamer_conf_init(struct frame_streamer_conf *conf)
{
  conf->transport = NULL;
  conf->format = FRAME_STREAMER_FORMAT_RAW;
  conf->name = "None";
  return 0;
}



static frame_streamer_jpeg_t *__frame_streamer_open_jpeg_encoder(frame_streamer_t *streamer)
{
  frame_streamer_jpeg_t *jpeg = pmsis_l2_malloc(sizeof(frame_streamer_jpeg_t));
  if (jpeg == NULL)
    goto error0;

  jpeg_encoder_t *enc = &jpeg->encoder;

  struct jpeg_encoder_conf enc_conf;

  jpeg_encoder_conf_init(&enc_conf);

  enc_conf.width = streamer->width;
  enc_conf.height = streamer->height;
  enc_conf.flags = 0;

  if (jpeg_encoder_open(enc, &enc_conf))
    goto error1;

  if (jpeg_encoder_start(enc))
    goto error2;

  jpeg->bitstream.size = JPEG_BITSTREAM_SIZE;
  jpeg->bitstream.data = pmsis_l2_malloc(JPEG_BITSTREAM_SIZE);
  if (jpeg->bitstream.data == NULL)
    goto error3;

  return jpeg;

error3:
  jpeg_encoder_stop(enc);

error2:
  jpeg_encoder_close(enc);

error1:
  pmsis_l2_malloc_free(jpeg, sizeof(frame_streamer_jpeg_t));

error0:
  return NULL;
};



static void __frame_streamer_close_jpeg_encoder(frame_streamer_jpeg_t *jpeg)
{
  jpeg_encoder_t *enc = &jpeg->encoder;
  pmsis_l2_malloc_free(jpeg->bitstream.data, JPEG_BITSTREAM_SIZE);
  jpeg_encoder_stop(enc);
  jpeg_encoder_close(enc);
  pmsis_l2_malloc_free(jpeg, sizeof(frame_streamer_jpeg_t));
}



frame_streamer_t *frame_streamer_open(struct frame_streamer_conf *conf)
{
  frame_streamer_t *streamer = pmsis_l2_malloc(sizeof(frame_streamer_t));
  if (streamer == NULL) return NULL;

  streamer->transport = conf->transport;
  streamer->format = conf->format;
  streamer->width = conf->width;
  streamer->height = conf->height;

  if (conf->format == FRAME_STREAMER_FORMAT_JPEG)
  {
    streamer->jpeg = __frame_streamer_open_jpeg_encoder(streamer);
    if (streamer->jpeg == NULL)
      goto error0;
  }

  streamer->channel = pi_transport_connect(streamer->transport, NULL, NULL);
  if (streamer->channel == -1)
    goto error1;

  streamer->req.header.channel = TRANSPORT_SERVICE_FRAME_STREAMER;
  streamer->req.header.packet_size = sizeof(frame_streamer_open_req_full_t) - sizeof(struct pi_transport_header);

  streamer->req.req.type = conf->format;
  streamer->req.req.channel = streamer->channel;
  streamer->req.req.width = conf->width;
  streamer->req.req.height = conf->height;
  streamer->req.req.depth = conf->depth;

  for (int i=0; i<NAME_LENGTH-1; i++)
  {
    streamer->req.req.name[i] = conf->name[i];
    if (conf->name[i] == 0)
      break;
  }
  streamer->req.req.name[NAME_LENGTH-1] = 0;

  if (pi_transport_send(streamer->transport, (void *)&streamer->req, sizeof(streamer->req)))
    goto error2;

  if (conf->format == FRAME_STREAMER_FORMAT_JPEG)
  {
    uint32_t header_size;
    if (jpeg_encoder_header(&streamer->jpeg->encoder, &streamer->jpeg->bitstream, &header_size))
      goto error2;

    streamer->header.info = 1;
    if (pi_transport_send_header(streamer->transport, &streamer->header, streamer->channel, header_size))
      goto error2;

    if (pi_transport_send(streamer->transport, streamer->jpeg->bitstream.data, header_size))
      goto error2;

    if (jpeg_encoder_footer(&streamer->jpeg->encoder, &streamer->jpeg->bitstream, &header_size))
      goto error2;

    streamer->header.info = 1;
    if (pi_transport_send_header(streamer->transport, &streamer->header, streamer->channel, header_size))
      goto error2;

    if (pi_transport_send(streamer->transport, streamer->jpeg->bitstream.data, header_size))
      goto error2;
  }

  return streamer;

error2:
  // TODO should close connection

error1:
  if (conf->format == FRAME_STREAMER_FORMAT_JPEG)
    __frame_streamer_close_jpeg_encoder(streamer->jpeg);

error0:
  pmsis_l2_malloc_free(streamer, sizeof(frame_streamer_t));
  return NULL;
}


int frame_streamer_send_async(frame_streamer_t *streamer, pi_buffer_t *buffer, pi_task_t *task)
{
  uint8_t *frame = buffer->data;

  if (streamer->format == FRAME_STREAMER_FORMAT_RAW)
  {
    int size = pi_buffer_size(buffer);
    if (pi_transport_send_header(streamer->transport, &streamer->header, streamer->channel, size))
      return -1;

    if (pi_transport_send_async(streamer->transport, buffer->data, size, task))
      return -1;
  }
  else if (streamer->format == FRAME_STREAMER_FORMAT_JPEG)
  {
    uint32_t size;
    while(1)
    {
      int err = jpeg_encoder_process(&streamer->jpeg->encoder, buffer, &streamer->jpeg->bitstream, &size);

      streamer->header.info = err ? 0 : 1;

      if (pi_transport_send_header(streamer->transport, &streamer->header, streamer->channel, size))
        return -1;

      if (pi_transport_send(streamer->transport, streamer->jpeg->bitstream.data, size))
        return -1;

      if (err == 0)
        break;
    }

    pi_task_push(task);
  }
  else
  {
    return -1;
  }

  return 0;
}



int frame_streamer_send(frame_streamer_t *streamer, pi_buffer_t *buffer)
{
  pi_task_t task;
  frame_streamer_send_async(streamer, buffer, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}