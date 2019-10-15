/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include <stdio.h>
#include <thread>
#include "debug_bridge/debug_bridge.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <signal.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>

#include "reqserver.h"

#include <condition_variable>

#if defined(__USE_SDL__)
#include <SDL.h>
#endif



class Target_req
{
public:
  bool done;

  hal_bridge_req_t target_req;
};



class Reqloop
{
public:
  Reqloop(unsigned int debug_struct_addr);
  bool connect(int m_port);
  int stop(bool kill);
  bool access(bool write, unsigned int addr, int size, char* buffer);

private:
  void reqloop_routine();
  void listener_routine();
  bool handle_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_connect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  void notif_target(hal_debug_struct_t *debug_struct);
  void reply_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_reply(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  void handle_bridge_to_target_reqs(hal_debug_struct_t *debug_struct);
  void handle_target_req(hal_debug_struct_t *debug_struct, Target_req *target_req);
  bool handle_req_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_read(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_write(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_close(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_fb_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_fb_update(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_target_status_sync(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_disconnect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  void update_target_status(hal_debug_struct_t *debug_struct);
  void efuse_access(bool write, int id, uint32_t value, uint32_t mask);
  int eeprom_access(uint32_t itf, uint32_t cs, bool write, uint32_t addr, uint32_t buffer, uint32_t size);
  int flash_access(int32_t type, uint32_t itf, uint32_t cs, bool write, uint32_t addr, uint32_t buffer, uint32_t size);
  int flash_erase_sector(int32_t type, uint32_t itf, uint32_t cs, uint32_t addr);
  int flash_erase_chip(int32_t type, uint32_t itf, uint32_t cs);
  void buffer_free(uint32_t addr, uint32_t size);
  uint32_t buffer_alloc(uint32_t size);
  void recv(int sockfd, void *buf, size_t len, int flags);
  void send(int sockfd, void *buf, size_t len, int flags);

  int m_socket;
  unsigned int debug_struct_addr;
  bool end;
  std::thread *thread;
  std::thread *rcv_thread;
  int status = 0;
  hal_debug_struct_t *debug_struct = NULL;
  bool pending_req;
  int pending_req_error;
  char *pending_buffer;
  bool connected;  
  std::queue<Target_req *> target_reqs;
  int target;

  std::mutex mutex;
  std::condition_variable cond;
};


Reqloop::Reqloop(unsigned int debug_struct_addr) : debug_struct_addr(debug_struct_addr)
{
}



bool Reqloop::access(bool write, unsigned int addr, int size, char* buffer)
{
  bool result;

  std::unique_lock<std::mutex> lock(this->mutex);

  reqserver_req_t req;
  req.trans_id = 1;
  req.addr = addr;
  req.len = size;
  req.type = write ? REQSERVER_WRITEMEM_REQ : REQSERVER_READMEM_REQ;

  this->pending_req = true;
  this->pending_buffer = buffer;

  this->send(this->m_socket, (void *)&req, sizeof(req), 0);

  if (write)
  {
    this->send(this->m_socket, (void *)buffer, size, 0);
  }

  while(this->pending_req)
  {
    this->cond.wait(lock);
  }

  result = !this->pending_req_error;

  lock.unlock();

  return result;
}


void Reqloop::recv(int sockfd, void *buf, size_t len, int flags)
{
  while (len > 0)
  {
    int err = ::recv(sockfd, buf, len, flags);

    if (err == 0 || (err == -1 && errno != EAGAIN))
      exit(0);

    if (err > 0)
    {
      len -= err;
      buf = ((char *)buf) + err;
    }
  }
}

void Reqloop::send(int sockfd, void *buf, size_t len, int flags)
{
  while (len > 0)
  {
    int err = ::send(sockfd, buf, len, flags);
    if (err == 0 || (err == -1 && errno != EAGAIN))
      exit(0);

    if (err > 0)
    {
      len -= err;
      buf = ((char *)buf) + err;
    }
  }
}

void Reqloop::listener_routine()
{
  while(!this->end)
  {
    reqserver_rsp_payload_t rsp;

    this->recv(this->m_socket, (void *)&rsp.rsp, sizeof(rsp.rsp), 0);

    if (rsp.rsp.type == REQSERVER_READMEM_RSP || rsp.rsp.type == REQSERVER_WRITEMEM_RSP|| rsp.rsp.type == REQSERVER_ERROR_RSP)
    {
      if (rsp.rsp.type == REQSERVER_READMEM_RSP)
      {
        this->recv(this->m_socket, (void *)&rsp.len, sizeof(rsp.len), 0);
        this->recv(this->m_socket, (void *)this->pending_buffer, rsp.len, 0);
      }

      this->mutex.lock();
      this->pending_req = false;
      this->pending_req_error = rsp.rsp.type == REQSERVER_ERROR_RSP;
      this->cond.notify_all();
      this->mutex.unlock();
    }
    else if (rsp.rsp.type == REQSERVER_ALERT_RSP)
    {
      //rintf("Received ALERT\n");
    }
    else if (rsp.rsp.type == REQSERVER_RESET_RSP)
    {
      //printf("Received RESET\n");
    }


  }
}



void Reqloop::notif_target(hal_debug_struct_t *debug_struct)
{
  uint32_t notif_req_addr;
  uint32_t notif_req_value;
  access(false, (unsigned int)(long)&debug_struct->notif_req_addr, 4, (char*)&notif_req_addr);
  access(false, (unsigned int)(long)&debug_struct->notif_req_value, 4, (char*)&notif_req_value);
  access(true, (unsigned int)(long)notif_req_addr, 4, (char*)&notif_req_value);
}



void Reqloop::reply_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req)
{
  uint32_t value = 1;
  this->access(true, (unsigned int)(long)&target_req->done, sizeof(target_req->done), (char*)&value);

  this->notif_target(debug_struct);
}



bool Reqloop::handle_req_connect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  this->connected = true;
  this->reply_req(debug_struct, target_req, req);
  return false;
}



bool Reqloop::handle_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  //printf("HANDLING REQ %d\n", req->type);

  switch (req->type)
  {
    case HAL_BRIDGE_REQ_CONNECT:    return this->handle_req_connect(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_DISCONNECT: return this->handle_req_disconnect(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_OPEN:       return this->handle_req_open(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_READ:       return this->handle_req_read(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_WRITE:      return this->handle_req_write(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_CLOSE:      return this->handle_req_close(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_FB_OPEN:    return this->handle_req_fb_open(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_FB_UPDATE:  return this->handle_req_fb_update(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_TARGET_STATUS_SYNC:    return this->handle_req_target_status_sync(debug_struct, req, target_req);
    case HAL_BRIDGE_REQ_REPLY:    return this->handle_req_reply(debug_struct, req, target_req);
    //this->log->print(LOG_ERROR, "Received unknown request from target (type: %d)\n", req->type);
  }
  return false;
}



void Reqloop::handle_bridge_to_target_reqs(hal_debug_struct_t *debug_struct)
{
  if (!this->connected)
    return;

  while(this->target_reqs.size())
  {
    // Runtime can only handle one request, first check if no request is already
    // pushed.
    uint32_t target_req;
    this->access(false, (unsigned int)(long)&debug_struct->target_req, 4, (char*)&target_req);
    if (target_req)
      break;

    this->mutex.lock();
    Target_req *bridge_target_req = this->target_reqs.front();
    this->target_reqs.pop();
    this->handle_target_req(debug_struct, bridge_target_req);
    this->mutex.unlock();
  }
}



void Reqloop::handle_target_req(hal_debug_struct_t *debug_struct, Target_req *target_req)
{
  // First get a request from the target
  hal_bridge_req_t *req = NULL;

  this->access(false, (unsigned int)(long)&debug_struct->first_bridge_free_req, 4, (char*)&req);

  if (req == NULL)
  {
    //this->log->error("Unable to allocate bridge to target request");
    throw std::logic_error("Unable to allocate bridge to target request");
  }

  uint32_t next;
  this->access(false, (unsigned int)(long)&req->next, 4, (char*)&next);
  this->access(true, (unsigned int)(long)&debug_struct->first_bridge_free_req, 4, (char*)&next);
  this->access(true, (unsigned int)(long)req, sizeof(hal_bridge_req_t), (char*)&target_req->target_req);
  this->access(true, (unsigned int)(long)&req->bridge_data, sizeof(target_req), (char*)&target_req);


  // Store it to the debug structure
  this->access(true, (unsigned int)(long)&debug_struct->target_req, 4, (char*)&req);

  // And notify the target so that it is processed
  this->notif_target(debug_struct);
}



void Reqloop::reqloop_routine()
{
  if (this->debug_struct_addr)
  {
    this->access(false, debug_struct_addr, 4, (char*)&this->debug_struct);

    //printf("GOT DEBUG STRUCT %x\n", this->debug_struct);

    while(!this->end)
    {
      // Handle target to bridge requests
      while(1) {
        hal_bridge_req_t *first_bridge_req=NULL, *last_req=NULL, *next=NULL, *next_next=NULL;

        if (!this->access(false, (unsigned int)(long)&debug_struct->first_bridge_req, 4, (char*)&first_bridge_req)) goto end;

        if (first_bridge_req == NULL)
          break;

        hal_bridge_req_t req;
        if (!this->access(false, (unsigned int)(long)first_bridge_req, sizeof(hal_bridge_req_t), (char*)&req)) goto end;

        uint64_t value = 1;
        if (!this->access(true, (unsigned int)(long)&first_bridge_req->popped, sizeof(first_bridge_req->popped), (char*)&value)) goto end;
        if (!this->access(true, (unsigned int)(long)&debug_struct->first_bridge_req, 4, (char*)&req.next)) goto end;

        if (this->handle_req(debug_struct, &req, first_bridge_req))
          return;
      }

      // Handle bridge to target requests
      this->handle_bridge_to_target_reqs(debug_struct);
    }
  }
end:
  printf("Got access error in reqloop\n");
}



bool Reqloop::connect(int m_port)
{
  struct sockaddr_in addr;
  struct hostent *he;

  char *m_server = (char *)"localhost";

  if((m_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Unable to create socket (%s)\n", strerror(errno));
    return false;
  }

  if((he = gethostbyname(m_server)) == NULL) {
    perror("gethostbyname");
    return false;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(m_port);
  addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if(::connect(m_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    fprintf(stderr, "Unable to connect to %s port %d (%s)\n", m_server, m_port,
            strerror(errno));
    return false;
  }

  this->connected = false;
  this->end = false;
  this->thread = new std::thread(&Reqloop::reqloop_routine, this);
  this->rcv_thread = new std::thread(&Reqloop::listener_routine, this);

  return true;
}


int Reqloop::stop(bool kill)
{
  if (kill) this->end = true;
  this->thread->join();
  return this->status;
}



static void init_sigint_handler(int) {
  raise(SIGTERM);
}


extern "C" void *rtbridge_reqloop_open(unsigned int debug_struct_addr)
{
  // This should be the first C method called by python.
  // As python is not catching SIGINT where we are in C world, we have to
  // setup a  sigint handler to exit in case control+C is hit.
  signal (SIGINT, init_sigint_handler);

  Reqloop *reqloop = new Reqloop(debug_struct_addr);
  reqloop->connect(9999);
  return (void *)reqloop;
}

extern "C" void rtbridge_reqloop_close(void *arg, int kill)
{
  Reqloop *reqloop = (Reqloop *)arg;
  reqloop->stop(kill);
}





#if 0
class Reqloop
{
public:
  Reqloop(Cable *cable, unsigned int debug_struct_addr);
  void reqloop_routine();
  int stop(bool kill);
  void activate();

  void efuse_access(bool write, int id, uint32_t value, uint32_t mask);
  int eeprom_access(uint32_t itf, uint32_t cs, bool write, uint32_t addr, uint32_t buffer, uint32_t size);
  int flash_access(int32_t type, uint32_t itf, uint32_t cs, bool write, uint32_t addr, uint32_t buffer, uint32_t size);
  int flash_erase_sector(int32_t type, uint32_t itf, uint32_t cs, uint32_t addr);
  int flash_erase_chip(int32_t type, uint32_t itf, uint32_t cs);
  void buffer_free(uint32_t addr, uint32_t size);
  uint32_t buffer_alloc(uint32_t size);

private:
  void reply_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_connect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_read(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_write(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_close(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_fb_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_fb_update(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_target_status_sync(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req);
  bool handle_req_disconnect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req_reply(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  bool handle_req(hal_debug_struct_t *debug_struct, hal_bridge_req_t *target_req, hal_bridge_req_t *req);
  void update_target_status(hal_debug_struct_t *debug_struct);
  void wait_target_available(hal_debug_struct_t *debug_struct);

  void notif_target(hal_debug_struct_t *debug_struct);
  void handle_target_req(hal_debug_struct_t *debug_struct, Target_req *target_req);
  void handle_bridge_to_target_reqs(hal_debug_struct_t *debug_struct);

  bool wait_target_request();
  unsigned int get_target_state();
  void send_target_ack();
  void clear_target_ack();

  Log *log;
  std::thread *thread;
  Cable *cable;
  bool end = false;
  unsigned int debug_struct_addr;
  int status = 0;
  bool connected = false;   // Set to true once the applicatoin has sent the connect request
  bool target_connected;

  hal_target_state_t target;

  std::queue<Target_req *> target_reqs;

  std::mutex mutex;
  std::condition_variable cond;

  target_sync_fsm_state_e target_sync_fsm_state;
  unsigned int jtag_val;

  hal_debug_struct_t *debug_struct = NULL;

  bool target_jtag_sync;
};
#endif

class Framebuffer
{
public:
  Framebuffer(Reqloop *cable, std::string name, int width, int height, int format);
  void update(uint32_t addr, int posx, int posy, int width, int height);
  bool open();

private:
  void fb_routine();

  std::string name;
  int width;
  int height;
  int format;
  int pixel_size = 1;
  Reqloop *cable;
  std::thread *thread;
  uint32_t *pixels;
#if defined(__USE_SDL__)
  SDL_Surface *screen;
  SDL_Texture * texture;
  SDL_Renderer *renderer;
  SDL_Window *window;
#endif
};

Framebuffer::Framebuffer(Reqloop *cable, std::string name, int width, int height, int format)
: name(name), width(width), height(height), format(format), cable(cable)
{
}

void Framebuffer::fb_routine()
{
#if defined(__USE_SDL__)
  bool quit = false;
  SDL_Event event;

  while (!quit)
  {
    SDL_WaitEvent(&event);
    switch (event.type)
    {
      case SDL_QUIT:
      quit = true;
      break;
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
#endif
}


bool Framebuffer::open()
{
#if defined(__USE_SDL__)

  if (format == HAL_BRIDGE_REQ_FB_FORMAT_GRAY)
  {
    pixel_size = 1;
  }
  else if (format == HAL_BRIDGE_REQ_FB_FORMAT_RGB)
  {
    pixel_size = 4;
  }
  else if (format == HAL_BRIDGE_REQ_FB_FORMAT_RAW)
  {
    pixel_size = 1;
  }
  else
  {
    printf("Unsupported format: %d\n", format);
  }

  pixels = new uint32_t[width*height];
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow(name.c_str(),
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  texture = SDL_CreateTexture(renderer,
      SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

  memset(pixels, 255, width * height * sizeof(Uint32));

  SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(Uint32));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  thread = new std::thread(&Framebuffer::fb_routine, this);
  return true;

#else
  printf("Trying to open framebuffer while bridge has not been compiled with SDL support\n");
  return false;
#endif
}

void Framebuffer::update(uint32_t addr, int posx, int posy, int width, int height)
{
#if defined(__USE_SDL__)

  if (posx == -1)
  {
    posx = posy = 0;
    width = this->width;
    height = this->height;
  }

  int size = width*height*pixel_size;

  if (this->format == HAL_BRIDGE_REQ_FB_FORMAT_GRAY)
  {
    uint8_t buffer[size];
    this->cable->access(false, addr, size, (char*)buffer);

    for (int j=0; j<height; j++)
    {
      for (int i=0; i<width; i++)
      {
        unsigned int value = buffer[j*width+i];
        pixels[(j+posy)*this->width + i + posx] = (0xff << 24) | (value << 16) | (value << 8) | value;
      }
    }
  }
  else if (this->format == HAL_BRIDGE_REQ_FB_FORMAT_RAW)
  {
    uint8_t buffer[size];
    this->cable->access(false, addr, size, (char*)buffer);

    for (int j=0; j<height; j++)
    {
      for (int i=0; i<width; i++)
      {
        int shift;
        int line = height - j - 1;
        if (line & 1)
        {
          if (i & 1)
            shift = 16;
          else
            shift = 8;
        }
        else
        {
          if (i & 1)
            shift = 8;
          else
            shift = 0;
        }

        unsigned int value = buffer[j*width+i];

        pixels[(j+posy)*this->width + i + posx] = (0xff << 24) | (value << shift);
      }
    }
  }
  else
  {
    uint32_t buffer[size];
    this->cable->access(false, addr, size*4, (char*)buffer);

    for (int j=0; j<height; j++)
    {
      for (int i=0; i<width; i++)
      {
        unsigned int value = buffer[j*width+i];
        pixels[(j+posy)*this->width + i + posx] = (0xff << 24) | value;
      }
    }
  }

  SDL_UpdateTexture(texture, NULL, pixels, this->width * sizeof(Uint32));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
#endif
}




static int transpose_code(int code)
{
  int alt = 0;

  if ((code & 0x0) == 0x0) alt |= O_RDONLY;
  if ((code & 0x1) == 0x1) alt |= O_WRONLY;
  if ((code & 0x2) == 0x2) alt |= O_RDWR;
  if ((code & 0x8) == 0x8) alt |= O_APPEND;
  if ((code & 0x200) == 0x200) alt |= O_CREAT;
  if ((code & 0x400) == 0x400) alt |= O_TRUNC;
  if ((code & 0x800) == 0x800) alt |= O_EXCL;
  if ((code & 0x2000) == 0x2000) alt |= O_SYNC;
  if ((code & 0x4000) == 0x4000) alt |= O_NONBLOCK;
  if ((code & 0x8000) == 0x8000) alt |= O_NOCTTY;

  return alt;
}

bool Reqloop::handle_req_reply(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  Target_req *bridge_req = (Target_req *)req->bridge_data;

  this->mutex.lock();
  bridge_req->done = true;
  this->notif_target(debug_struct);
  this->cond.notify_all();

  // Copy the target information to the local request in case some returned
  // information are needed
  memcpy((void *)&bridge_req->target_req, (void *)req, sizeof(hal_bridge_req_t));

  this->mutex.unlock();
  return false;
}

bool Reqloop::handle_req_disconnect(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  this->connected = false;
  this->reply_req(debug_struct, target_req, req);
  return true;
}

bool Reqloop::handle_req_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  char name[req->open.name_len+1];
  this->access(false, (unsigned int)(long)req->open.name, req->open.name_len+1, (char*)name);

  int res = open(name, req->open.flags, req->open.mode);

  this->access(true, (unsigned int)(long)&target_req->open.retval, 4, (char*)&res);

  this->reply_req(debug_struct, target_req, req);
  return false;
}

bool Reqloop::handle_req_read(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  char buffer[4096];
  int size = req->read.len;
  char *ptr = (char *)(long)req->read.ptr;
  int res = 0;
  while (size)
  {
    int iter_size = size;
    if (iter_size > 4096)
      iter_size = 4096;

    iter_size = read(req->read.file, (void *)buffer, iter_size);

    if (iter_size <= 0) {
      if (iter_size == -1 && res == 0) res = -1;
      break;
    }

    this->access(true, (unsigned int)(long)ptr, iter_size, (char*)buffer);

    res += iter_size;
    ptr += iter_size;
    size -= iter_size;
  }

  this->access(true, (unsigned int)(long)&target_req->read.retval, 4, (char*)&res);

  this->reply_req(debug_struct, target_req, req);
  return false;
}

bool Reqloop::handle_req_write(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  char buffer[4096];
  int size = req->write.len;
  char *ptr = (char *)(long)req->write.ptr;
  int res = 0;
  while (size)
  {
    int iter_size = size;
    if (iter_size > 4096)
      iter_size = 4096;

    this->access(false, (unsigned int)(long)ptr, iter_size, (char*)buffer);

    iter_size = write(req->write.file, (void *)buffer, iter_size);

    if (iter_size <= 0)
      break;

    res += iter_size;
    ptr += iter_size;
    size -= iter_size;
  }

  if (res == 0)
    res = -1;

  this->access(true, (unsigned int)(long)&target_req->write.retval, 4, (char*)&res);

  this->reply_req(debug_struct, target_req, req);
  return false;
}

bool Reqloop::handle_req_close(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  int res = close(req->close.file);
  this->access(true, (unsigned int)(long)&target_req->write.retval, 4, (char*)&res);
  this->reply_req(debug_struct, target_req, req);
  return false;
}

bool Reqloop::handle_req_fb_open(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  char name[req->fb_open.name_len+1];
  this->access(false, (unsigned int)(long)req->fb_open.name, req->fb_open.name_len+1, (char*)name);

  int res = 0;
  Framebuffer *fb = new Framebuffer(this, name, req->fb_open.width, req->fb_open.height, req->fb_open.format);



  if (!fb->open()) 
  {
    res = -1;
    delete fb;
    fb = NULL;
  }

  this->access(true, (unsigned int)(long)&target_req->fb_open.screen, 8, (char*)&fb);

  this->reply_req(debug_struct, target_req, req);
  return false;
}

void Reqloop::update_target_status(hal_debug_struct_t *debug_struct)
{
  this->access(false, (unsigned int)(long)&debug_struct->target, sizeof(this->target), (char *)&this->target);
}

bool Reqloop::handle_req_target_status_sync(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
  this->update_target_status(debug_struct);
  this->reply_req(debug_struct, target_req, req);
  return false;
}

bool Reqloop::handle_req_fb_update(hal_debug_struct_t *debug_struct, hal_bridge_req_t *req, hal_bridge_req_t *target_req)
{
#if defined(__USE_SDL__)
  Framebuffer *fb = (Framebuffer *)req->fb_update.screen;

  fb->update(
    req->fb_update.addr, req->fb_update.posx, req->fb_update.posy, req->fb_update.width, req->fb_update.height
  );
#endif

  this->reply_req(debug_struct, target_req, req);
  return false;
}

void Reqloop::efuse_access(bool write, int id, uint32_t value, uint32_t mask)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_EFUSE_ACCESS;
  req->target_req.efuse_access.is_write = write;
  req->target_req.efuse_access.index = id;
  req->target_req.efuse_access.value = value;
  req->target_req.efuse_access.mask = mask;

  std::unique_lock<std::mutex> lock(this->mutex);

  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  lock.unlock();
}

int Reqloop::eeprom_access(uint32_t itf, uint32_t cs, bool write, uint32_t addr, uint32_t buffer, uint32_t size)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_EEPROM_ACCESS;
  req->target_req.eeprom_access.itf = itf;
  req->target_req.eeprom_access.cs = cs;
  req->target_req.eeprom_access.is_write = write;
  req->target_req.eeprom_access.addr = addr;
  req->target_req.eeprom_access.buffer = buffer;
  req->target_req.eeprom_access.size = size;

  std::unique_lock<std::mutex> lock(this->mutex);
  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  int retval = req->target_req.eeprom_access.retval;

  delete req;

  lock.unlock();

  return retval;
}

int Reqloop::flash_access(int type, uint32_t itf, uint32_t cs, bool write, uint32_t addr, uint32_t buffer, uint32_t size)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_FLASH_ACCESS;
  req->target_req.flash_access.type = type;
  req->target_req.flash_access.itf = itf;
  req->target_req.flash_access.cs = cs;
  req->target_req.flash_access.is_write = write;
  req->target_req.flash_access.addr = addr;
  req->target_req.flash_access.buffer = buffer;
  req->target_req.flash_access.size = size;

  std::unique_lock<std::mutex> lock(this->mutex);
  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  int retval = req->target_req.flash_access.retval;

  delete req;

  lock.unlock();

  return retval;
}

int Reqloop::flash_erase_sector(int type, uint32_t itf, uint32_t cs, uint32_t addr)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_FLASH_ERASE_SECTOR;
  req->target_req.flash_erase_sector.type = type;
  req->target_req.flash_erase_sector.itf = itf;
  req->target_req.flash_erase_sector.cs = cs;
  req->target_req.flash_erase_sector.addr = addr;

  std::unique_lock<std::mutex> lock(this->mutex);
  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  int retval = req->target_req.flash_erase_sector.retval;

  delete req;

  lock.unlock();

  return retval;
}

int Reqloop::flash_erase_chip(int type, uint32_t itf, uint32_t cs)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_FLASH_ERASE_CHIP;
  req->target_req.flash_erase_chip.type = type;
  req->target_req.flash_erase_chip.itf = itf;
  req->target_req.flash_erase_chip.cs = cs;

  std::unique_lock<std::mutex> lock(this->mutex);
  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  int retval = req->target_req.flash_erase_chip.retval;

  delete req;

  lock.unlock();

  return retval;
}

void Reqloop::buffer_free(uint32_t addr, uint32_t size)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_BUFFER_FREE;
  req->target_req.buffer_free.size = size;
  req->target_req.buffer_free.buffer = addr;

  std::unique_lock<std::mutex> lock(this->mutex);
  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  delete req;

  lock.unlock();
}

uint32_t Reqloop::buffer_alloc(uint32_t size)
{
  Target_req *req = new Target_req();
  req->done = false;

  req->target_req.type = HAL_BRIDGE_TARGET_REQ_BUFFER_ALLOC;
  req->target_req.buffer_alloc.size = size;

  std::unique_lock<std::mutex> lock(this->mutex);
  this->target_reqs.push(req);

  while(!req->done)
  {
    this->cond.wait(lock);
  }

  uint32_t addr = req->target_req.buffer_alloc.buffer;

  delete req;

  lock.unlock();

  return addr;
}
