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

#include "pmsis_tools.hpp"
#include "tools/frame_streamer_constants.h"
#ifdef __USE_OPENCV__
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif
#include <stdio.h>
#include <string.h>
#if defined(__USE_SDL__)
#include <SDL.h>
#endif

#ifdef __USE_OPENCV__
using namespace cv;
#endif
using namespace std;

class Frame_display
{
public:
  Frame_display(std::string name, int width, int weight, int channels);
  ~Frame_display();
  void display(void *frame, int size);

private:
  int width;
  int height;
  int depth;
#if defined(__USE_SDL__)
  SDL_Surface *screen;
  SDL_Texture * texture;
  SDL_Renderer *renderer;
  SDL_Window *window;
#endif
  uint32_t *pixels;
};



class Frame_stream : public Transport_channel
{
public:
  Frame_stream(Transport *transport, char *name, int type, int channel, int width, int height, int depth, int show);
  void *get_frame();
  void handle_req(Transport_req_t *req);
  void close();

  int width;
  int height;
  int depth;
  int type;

private:
  Frame_display *display;
  int channel;
  int port;
  std::queue<void *> frames;
  std::mutex mutex;
  std::condition_variable cond;
  Transport *transport;
  unsigned char *current_buffer;
  int current_size;
  unsigned char *jpeg_header;
  unsigned int jpeg_header_size;
  unsigned char *jpeg_footer;
  unsigned int jpeg_footer_size;
};



void Frame_display::display(void *frame, int size)
{
#if defined(__USE_SDL__)
  uint8_t *buffer = (uint8_t *)frame;
  SDL_Event event;

  for (int j=0; j<this->height; j++)
  {
    for (int i=0; i<this->width; i++)
    {
      int index = j*width+i;
      if (index >= size)
        goto end;
      unsigned int value = buffer[index];
      this->pixels[j*this->width + i] = (0xff << 24) | (value << 16) | (value << 8) | value;
    }
  }

end:
  SDL_UpdateTexture(texture, NULL, this->pixels, this->width*sizeof(Uint32));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
#endif
}



Frame_display::Frame_display(std::string name, int width, int height, int depth)
: width(width), height(height), depth(depth)
{
#if defined(__USE_SDL__)
  this->pixels = new uint32_t[this->width*this->height];
  SDL_Init(SDL_INIT_VIDEO);

  this->window = SDL_CreateWindow(name.c_str(),
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->width, this->height, 0);

  this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

  this->texture = SDL_CreateTexture(this->renderer,
      SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, this->width, this->height);

  memset(this->pixels, 255, this->width * this->height * sizeof(Uint32));

  SDL_UpdateTexture(this->texture, NULL, this->pixels, this->width * sizeof(Uint32));

  SDL_RenderClear(this->renderer);
  SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
  SDL_RenderPresent(this->renderer);
#endif
}



Frame_display::~Frame_display()
{
#if defined(__USE_SDL__)
  SDL_DestroyWindow(window);
  SDL_Quit();
#endif  
}



void *Frame_stream::get_frame()
{
  void *frame;
  std::unique_lock<std::mutex> lock(this->mutex);

  while(this->frames.empty())
  {
    this->cond.wait(lock);
  }

  frame = this->frames.front();
  this->frames.pop();

  lock.unlock();


  return frame;
}



Frame_stream::Frame_stream(Transport *transport, char *name, int type, int channel, int width, int height, int depth, int show)
: transport(transport), channel(channel), width(width), height(height), depth(depth), jpeg_header(NULL)
{
  if (show)
    this->display = new Frame_display(name, width, height, depth);
  else
    this->display = NULL;

  this->type = type;
  this->current_buffer = NULL;
  this->current_size = 0;
}




void Frame_stream::close()
{
  if (this->display)
    delete(this->display);
}



void Frame_stream::handle_req(Transport_req_t *req)
{
  unsigned char *buffer;

  if (this->current_buffer)
  {
    buffer = this->current_buffer;
  }
  else
  {
    buffer = new unsigned char[this->width*this->height*this->depth];
  }

  if (this->transport->recv((void *)&buffer[this->current_size], req->packet_size))
    return;

  if (this->type == FRAME_STREAMER_FORMAT_JPEG)
  {
    if (this->current_buffer == NULL)
      this->current_buffer = buffer;

    this->current_size += req->packet_size;

    if (req->info == 0)
      return;

    if (this->jpeg_header == NULL)
    {
      this->jpeg_header = new unsigned char[this->current_size];
      this->jpeg_header_size = this->current_size;
      ::memcpy(this->jpeg_header, this->current_buffer, this->current_size);
      this->current_buffer = NULL;
      this->current_size = 0;
      return;
    }
    else if (this->jpeg_footer == NULL)
    {
      this->jpeg_footer = new unsigned char[this->current_size];
      this->jpeg_footer_size = this->current_size;
      ::memcpy(this->jpeg_footer, this->current_buffer, this->current_size);
      this->current_buffer = NULL;
      this->current_size = 0;
      return;
    }
    else
    {
#ifdef __USE_OPENCV__
      int jpeg_buffer_size = this->jpeg_header_size + this->jpeg_footer_size + this->current_size;
      uint8_t *jpeg_buffer = new uint8_t[jpeg_buffer_size];
      memcpy(&jpeg_buffer[0], this->jpeg_header, this->jpeg_header_size);
      memcpy(&jpeg_buffer[this->jpeg_header_size], buffer, this->current_size);
      memcpy(&jpeg_buffer[this->jpeg_header_size + this->current_size], this->current_buffer, this->jpeg_footer_size);

      #if 0
      FILE *file = fopen("image.jpeg", "w");
      fwrite(jpeg_buffer, 1, jpeg_buffer_size, file);
      fclose(file);
      #endif


      std::vector<uint8_t> jpeg_image(&jpeg_buffer[0], &jpeg_buffer[jpeg_buffer_size]);
      InputArray input_array(jpeg_image);
      Mat image = imdecode(input_array, CV_LOAD_IMAGE_GRAYSCALE);

      memcpy(buffer, image.data, this->width*this->height);
      delete jpeg_buffer;
#endif

    }

    if (this->display)
      this->display->display(buffer, this->width*this->height);
  }
  else
  {
    if (this->display)
      this->display->display(buffer, req->packet_size);
  }

  this->current_buffer = NULL;
  this->current_size = 0;

  this->mutex.lock();

  if (this->frames.size() < 5)
  {
    this->frames.push(buffer);

    this->cond.notify_all();
  }
  else
  {
    delete buffer;
  }

  this->mutex.unlock();
}



Frame_streamer::Frame_streamer(Transport *transport, int show)
: transport(transport), show(show)
{
  transport->register_service(TRANSPORT_SERVICE_FRAME_STREAMER, this);
}



void Frame_streamer::handle_connect(Transport_req_t *req)
{
  frame_streamer_open_req_t open_req;
  if (this->transport->recv((void *)&open_req, sizeof(open_req)))
    return;

  Frame_stream *stream = new Frame_stream(this->transport, open_req.name, open_req.type, open_req.channel, open_req.width, open_req.height, open_req.depth, this->show);

  this->mutex.lock();
  this->streams[open_req.name] = stream;
  this->cond.notify_all();
  this->mutex.unlock();

  this->transport->register_channel(open_req.channel, stream);
}


void *Frame_streamer::get_stream(char *name, int *width, int *height, int *depth)
{
  Frame_stream *stream;
  std::unique_lock<std::mutex> lock(this->mutex);

  while(this->streams[name] == NULL)
  {
    this->cond.wait(lock);
  }

  stream = this->streams[name];

  lock.unlock();

  *width = stream->width;
  *height = stream->height;
  *depth = stream->depth;

  return (void *)stream;
}



extern "C" void *frame_streamer_open(void *transport, int show)
{
  return (void *)new Frame_streamer((Transport *)transport, show);
}



extern "C" void *frame_stream_get_frame(void *instance, unsigned char *py_frame)
{
  Frame_stream *stream = (Frame_stream *)instance;

  void *frame = stream->get_frame();

  memcpy(py_frame, frame, stream->width*stream->height*stream->depth);
}


extern "C" void *frame_streamer_get_stream(void *instance, char *name, int *width, int *height, int *depth)
{
  Frame_streamer *streamer = (Frame_streamer *)instance;

  return streamer->get_stream(name, width, height, depth);
}