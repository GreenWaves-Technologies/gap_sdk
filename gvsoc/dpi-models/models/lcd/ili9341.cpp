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

#include "dpi/models.hpp"
#include <stdint.h>
#include <vector>
#include <thread>

#if defined(__USE_SDL__)
#include <SDL.h>
#endif

typedef enum {
  STATE_NONE,
  STATE_SET_COLUMN,
  STATE_SET_PAGE,
  STATE_MEM_WRITE,
  STATE_SET_MADCTL
} lcd_state_e;

typedef struct {
  union {
    struct {
      unsigned int padding:2;
      unsigned int mh:1;
      unsigned int bgr:1;
      unsigned int ml:1;
      unsigned int mv:1;
      unsigned int mx:1;
      unsigned int my:1;
    };
    uint8_t raw;
  };
} madctl_t;

class ili9341;

class ili9341_qspi_itf : public Qspi_itf
{
public:
  ili9341_qspi_itf(ili9341 *top) : top(top) {}
  void sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
  void edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask);
  void cs_edge(int64_t timestamp, int cs);

private:
  ili9341 *top;
};


class ili9341_gpio_itf : public Gpio_itf
{
public:
  ili9341_gpio_itf(ili9341 *top) : top(top) {}
  void edge(int64_t timestamp, int data);

private:
  ili9341 *top;
};



class ili9341 : public Dpi_model
{
  friend class ili9341_qspi_itf;
  friend class ili9341_gpio_itf;

public:
  ili9341(js::config *config, void *handle);

protected:

  void sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void cs_edge(int64_t timestamp, int cs);

  void gpio_edge(int64_t timestamp, int data);
  void check_open();


private:

  void init();
  void fb_routine();
  void update(uint16_t pixel);

  ili9341_qspi_itf *qspi0;
  ili9341_gpio_itf *gpio;
  bool verbose;

  int prev_cs;
  int pending_bits;
  unsigned int pending_word;
  int waiting_bits;
  int current_column;
  int current_page;
  lcd_state_e state;

  bool active;
  bool is_command;

  int posx;
  int posy;
  int current_posx;
  int current_posy;
  int windows_width;
  int windows_height;
  int width;
  int height;
  std::thread *thread;
  uint32_t *pixels;
#if defined(__USE_SDL__)
  SDL_Surface *screen;
  SDL_Texture * texture;
  SDL_Renderer *renderer;
  SDL_Window *window;
#endif

  madctl_t madctl;
  void *trace;
  bool is_opened;
};


void ili9341::fb_routine()
{
#if defined(__USE_SDL__)
  bool quit = false;
  SDL_Event event;

  while (!quit)
  {
    SDL_UpdateTexture(texture, NULL, this->pixels, this->width*sizeof(Uint32));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_WaitEventTimeout(&event, 40);
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

void ili9341::check_open()
{
#if defined(__USE_SDL__)

  if (!this->is_opened)
  {
    this->is_opened = true;

    this->pixels = new uint32_t[this->width*this->height];
    memset(this->pixels, 255, this->width * this->height * sizeof(Uint32));

    SDL_Init(SDL_INIT_VIDEO);

    this->window = SDL_CreateWindow("lcd_ili9341",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->width, this->height, 0);

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

    this->texture = SDL_CreateTexture(this->renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, this->width, this->height);

    SDL_UpdateTexture(this->texture, NULL, this->pixels, this->width * sizeof(Uint32));

    SDL_RenderClear(this->renderer);
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
    SDL_RenderPresent(this->renderer);

    this->thread = new std::thread(&ili9341::fb_routine, this);
  }
#endif
}

ili9341::ili9341(js::config *config, void *handle) : Dpi_model(config, handle)
{
  verbose = true; //config->get("verbose")->get_bool();
  print("Creating LCD ILI9341 model");
  qspi0 = new ili9341_qspi_itf(this);
  gpio = new ili9341_gpio_itf(this);
  create_itf("input", static_cast<Dpi_itf *>(qspi0));
  create_itf("gpio", static_cast<Dpi_itf *>(gpio));

  this->init();

  this->is_opened = false;
  this->pending_bits = 0;
  this->waiting_bits = 8;
  this->is_command = true;
  this->state = STATE_NONE;

  this->madctl.raw = 0;

  this->width = 240;
  this->height = 320;

  this->trace = this->trace_new(config->get_child_str("name").c_str());
}

void ili9341_gpio_itf::edge(int64_t timestamp, int data)
{
  top->gpio_edge(timestamp, data);
}

void ili9341_qspi_itf::cs_edge(int64_t timestamp, int cs)
{
  top->cs_edge(timestamp, cs);
}

void ili9341_qspi_itf::sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->sck_edge(timestamp, sck, data_0, data_1, data_2, data_3, mask);
}

void ili9341_qspi_itf::edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->edge(timestamp, data_0, data_1, data_2, data_3, mask);
}

void ili9341::init()
{
}

void ili9341::gpio_edge(int64_t timestamp, int data)
{
  this->check_open();

  this->is_command = !data;

  this->trace_msg(this->trace, 3, "CMD GPIO edge (is_command: %d)", this->is_command);
}

void ili9341::cs_edge(int64_t timestamp, int cs)
{
  this->check_open();

  this->active = !cs;

  this->trace_msg(this->trace, 3, "CS edge (active: %d)", this->active);

  if (this->prev_cs == 1 && cs == 0)
  {
    this->init();
  }

  this->prev_cs = cs;
}

void ili9341::update(uint16_t pixel)
{
  this->check_open();

#if defined(__USE_SDL__)

  int r = ((pixel >> 11) & 0x1f) << 3;
  int g = ((pixel >>  5) & 0x3f) << 2;
  int b = ((pixel >>  0) & 0x1f) << 3;

#if 1
  int posx, posy, width, height;

  if (!this->madctl.mv)
  {
    width = this->width;
    height = this->height;
    posx = this->current_posx;
    posy = this->current_posy;

    if (this->madctl.my)
      posy = this->height - posy - 1;

    if (!this->madctl.mx)
      posx = this->width - posx - 1;
  }
  else
  {
    height = this->width;
    width = this->height;
    posy = this->current_posx;
    posx = this->current_posy;

    if (!this->madctl.my)
      posx = this->width - posx - 1;

    if (this->madctl.mx)
      posy = this->height - posy - 1;
  }



  unsigned int pos = posy*this->width + posx;

  //printf("WRITE PIXEL AT %d posx %d posy %d mv %d mx %d my %d\n", pos, this->current_posx, this->current_posy, this->madctl.mv, this->madctl.mx, this->madctl.my);

  this->pixels[pos % (this->width*this->height)] = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);

      this->current_posx++;
      if (this->current_posx == this->windows_width + 1)
      {
        this->current_posx = this->posx;
        this->current_posy++;
      }
#else
  int pos = this->current_posy*this->width + this->current_posx;

  printf("WRITE PIXEL AT %d posx %d posy %d\n", pos, this->current_posx, this->current_posy);

  this->pixels[pos % (this->width*this->height)] = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);

  if (this->madctl.mv)
  {
    if (!this->madctl.mx)
    {
      this->current_posy++;
      if (this->current_posy == this->windows_width + 1)
      {
        this->current_posy = this->posy;
        if (this->madctl.my)
        {
          this->current_posx++;
        }
        else
        {
          this->current_posx--;
        }
      }
    }
    else
    {
      this->current_posy--;
      if (this->current_posy == this->posy - 1)
      {
        this->current_posy = this->windows_width;
        if (this->madctl.my)
        {
          this->current_posx++;
        }
        else
        {
          this->current_posx--;
        }
      }
    }
  }
  else
  {
    if (!this->madctl.mx)
    {
      this->current_posx++;
      if (this->current_posx == this->windows_width + 1)
      {
        this->current_posx = this->posx;
        if (!this->madctl.my)
        {
          this->current_posy++;
        }
        else
        {
          this->current_posy--;
        }
      }
    }
    else
    {
      this->current_posx--;
      if (this->current_posx == this->posx - 1)
      {
        this->current_posx = this->windows_width;
        if (!this->madctl.my)
        {
          this->current_posy++;
        }
        else
        {
          this->current_posy--;
        }
      }
    }
  }
  #endif

#endif
}

void ili9341::edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  this->trace_msg(this->trace, 4, "Edge (timestamp: %ld, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)", timestamp, sdio0, sdio1, sdio2, sdio3, mask);

  this->check_open();

  if (this->active)
  {
    this->pending_word = (this->pending_word << 1) | sdio0;
    this->pending_bits++;

    if (this->is_command && this->pending_bits == 8)
    {
      this->pending_bits = 0;
  
      this->trace_msg(this->trace, 3, "Received command (command: 0x%2.2x)", this->pending_word & 0xff);

      switch (this->pending_word & 0xff)
      {
        case 0x2A:
          this->state = STATE_SET_COLUMN;
          this->waiting_bits = 32;
          break;

        case 0x2B:
          this->state = STATE_SET_PAGE;
          this->waiting_bits = 32;
          break;

        case 0x2C:
          this->state = STATE_MEM_WRITE;
          this->waiting_bits = 16;

#if 0
          if (this->madctl.mv)
          {
            this->current_posy = !this->madctl.mx ? this->posx : this->windows_width;
            this->current_posx = this->madctl.my ? this->posy : this->windows_height;

            printf("SET CURSOR mv %d mx %d my %d posx %d posy %d w %d h %d\n", this->madctl.mv, this->madctl.mx, this->madctl.my, this->posx, this->posy, this->windows_width, this->windows_height);

          }
          else
          {
            this->current_posy = !this->madctl.my ? this->posy : this->windows_height;
            this->current_posx = !this->madctl.mx ? this->posx : this->windows_width;
          }
#else
          this->current_posy = this->posy;
          this->current_posx = this->posx;
#endif


          break;

        case 0x36:
          this->state = STATE_SET_MADCTL;
          this->waiting_bits = 8;
          break;

        case 0xEF:
          this->waiting_bits = 24;
          break;

        case 0xCF:
          this->waiting_bits = 24;
          break;

        case 0xED:
          this->waiting_bits = 32;
          break;

        case 0xE8:
          this->waiting_bits = 24;
          break;

        case 0xCB:
          this->waiting_bits = 40;
          break;

        case 0xF7:
          this->waiting_bits = 8;
          break;

        case 0xEA:
          this->waiting_bits = 16;
          break;

        case 0xC0:
          this->waiting_bits = 8;
          break;

        case 0xC1:
          this->waiting_bits = 8;
          break;

        case 0xC5:
          this->waiting_bits = 16;
          break;

        case 0xC7:
          this->waiting_bits = 8;
          break;

        case 0x37:
          this->waiting_bits = 8;
          break;

        case 0x3A:
          this->waiting_bits = 8;
          break;

        case 0xB1:
          this->waiting_bits = 16;
          break;

        case 0xB6:
          this->waiting_bits = 24;
          break;

        case 0xF2:
          this->waiting_bits = 8;
          break;

        case 0x26:
          this->waiting_bits = 1*8;
          break;

        case 0xE0:
          this->waiting_bits = 15*8;
          break;

        case 0xE1:
          this->waiting_bits = 15*8;
          break;

        case 0x11:
          this->waiting_bits = 1*8;
          break;

        case 0x29:
          this->waiting_bits = 1*8;
          break;

        default:
          fatal("Received unknown command (command: 0x%2.2x)", this->pending_word & 0xff);
          break;
      }
    }
    else if (this->pending_bits == this->waiting_bits)
    {
      this->pending_bits = 0;

      switch (this->state)
      {
        case STATE_SET_MADCTL: 
          this->madctl.raw = this->pending_word & 0xff;
          this->trace_msg(this->trace, 2, "Setting MADCTL (value: 0x%2.2x)", this->madctl.raw);
          this->state = STATE_NONE;
          break;

        case STATE_SET_COLUMN: 
          this->current_column = this->pending_word;
          this->posx = this->pending_word >> 16;
          this->current_posx = posx;
          this->windows_width = this->pending_word & 0xffff;
          this->state = STATE_NONE;
          this->trace_msg(this->trace, 2, "Setting column (posx: %d, width: %d)", this->posx, this->windows_width);
          break;

        case STATE_SET_PAGE: 
          this->current_page = this->pending_word;
          this->posy = this->pending_word >> 16;
          this->current_posy = posy;
          this->windows_height = this->pending_word & 0xffff;
          this->state = STATE_NONE;
          this->trace_msg(this->trace, 2, "Setting page (posy: %d, height: %d)", this->posy, this->windows_height);
          break;

        case STATE_MEM_WRITE: 
          this->update(this->pending_word & 0xffff);
          this->trace_msg(this->trace, 2, "Writing pixel (value: 0x%4.4x)", this->pending_word & 0xffff);
          break;

        case STATE_NONE: 
        default:
          break;
      }

    }
  }
}

void ili9341::sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  this->check_open();

  if (verbose) print("SCK edge (timestamp: %ld, sck: %d, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)", timestamp, sck, sdio0, sdio1, sdio2, sdio3, mask);

}

extern "C" Dpi_model *dpi_model_new(js::config *config, void *handle)
{
  return new ili9341(config, handle);
}
