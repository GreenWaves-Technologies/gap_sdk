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
 * Authors: Francesco Paci, GreenWaves Technologies (francesco.paci@greenwaves-technologies.com)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */


#include "pmsis.h"
#include "pmsis/drivers/spi.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis/drivers/gpio.h"
#include "bsp/display/ili9341.h"
#include "bsp/bsp.h"
#include "ili9341.h"

#define TEMP_BUFFER_SIZE (256)

typedef struct
{
  struct pi_device spim;
  struct pi_device gpio_port;
  int width;
  uint32_t current_data;
  uint32_t current_size;
  uint32_t current_line_len;
  pi_buffer_t *buffer;
  uint8_t temp_buffer[TEMP_BUFFER_SIZE*2];
  pi_task_t temp_copy_task;
  pi_task_t *current_task;
  int gpio;

  unsigned int _width;
  unsigned int _height;
  int16_t cursor_x;
  int16_t cursor_y;
  int16_t wrap;
  uint8_t textsize;
  uint16_t textcolor;
  uint16_t textbgcolor;
} ili_t;

#if 1

static void __ili_init(ili_t *ili);
static void __ili_set_rotation(ili_t *ili,uint8_t m);
static void __ili_set_addr_window(ili_t *ili,uint16_t x, uint16_t y, uint16_t w, uint16_t h) ;
static void __ili_write_8(ili_t *ili, uint8_t  d);
static void __ili_write_command(ili_t *ili,uint8_t cmd);
static void __ili_gray8_to_rgb565(uint8_t *input,uint16_t *output,int width, int height);
static void __ili_rgb565_to_rgb565(uint16_t *input,uint16_t *output,int width, int height);

static void __ili9341_write_buffer_iter(void *arg)
{
  ili_t *ili = (ili_t *)arg;
  uint32_t size = TEMP_BUFFER_SIZE;

  if (ili->buffer->stride != 0)
  {
    if (size > ili->current_line_len)
      size = ili->current_line_len;
  }
  else
  {
    if (size > ili->current_size)
      size = ili->current_size;
  }

  pi_task_t *task = &ili->temp_copy_task;
  pi_spi_flags_e flags = PI_SPI_CS_KEEP;

  ili->current_size -= size;
  if (ili->current_size == 0)
  {
    task = ili->current_task;
    flags = PI_SPI_CS_AUTO;
  }
  if(ili->buffer->format==PI_BUFFER_FORMAT_RGB565){
    __ili_rgb565_to_rgb565((uint16_t *)ili->current_data, (uint16_t *)ili->temp_buffer, size, 1);
    ili->current_data += size*2;
  }
  else{
    __ili_gray8_to_rgb565((uint8_t *)ili->current_data, (uint16_t *)ili->temp_buffer, size, 1);
    ili->current_data += size;
  }

  
  if (ili->buffer->stride != 0)
  {
      ili->current_line_len -= size;
      if (ili->current_line_len == 0)
      {
          ili->current_line_len = ili->width;
          ili->current_data += ili->buffer->stride;
      }
  }
  pi_spi_send_async(&ili->spim, ili->temp_buffer, size*2*8, flags, task);
}



static void __ili_write_async(struct pi_device *device, pi_buffer_t *buffer, uint16_t x, uint16_t y,uint16_t w, uint16_t h, pi_task_t *task)
{
  ili_t *ili = (ili_t *)device->data;

  __ili_set_addr_window(ili, x, y, w, h); // Clipped area

  pi_task_callback(&ili->temp_copy_task, __ili9341_write_buffer_iter, (void *)ili);

  ili->buffer = buffer;
  ili->width = w;
  ili->current_task = task;
  ili->current_data = (uint32_t)buffer->data;
  ili->current_size = w*h;
  ili->current_line_len = w;

  __ili9341_write_buffer_iter(ili);
}



static int __ili_open(struct pi_device *device)
{
  struct pi_ili9341_conf *conf = (struct pi_ili9341_conf *)device->config;

  ili_t *ili = (ili_t *)pmsis_l2_malloc(sizeof(ili_t));
  if (ili == NULL) return -1;

  if (bsp_ili9341_open(conf))
    goto error;

  struct pi_gpio_conf gpio_conf;
  pi_gpio_conf_init(&gpio_conf);

  pi_open_from_conf(&ili->gpio_port, &gpio_conf);

  if (pi_gpio_open(&ili->gpio_port))
    goto error;

  device->data = (void *)ili;

  ili->gpio = conf->gpio;

  struct pi_spi_conf spi_conf;
  pi_spi_conf_init(&spi_conf);
  spi_conf.itf = conf->spi_itf;
  spi_conf.cs = conf->spi_cs;

  spi_conf.wordsize = PI_SPI_WORDSIZE_8;
  spi_conf.big_endian = 1;
  spi_conf.max_baudrate = 50000000;
  spi_conf.polarity = 0;
  spi_conf.phase = 0;

  pi_open_from_conf(&ili->spim, &spi_conf);

  if (pi_spi_open(&ili->spim))
    goto error;

  __ili_init(ili);
  ili->_width = ILI9341_TFTWIDTH;
  ili->_height = ILI9341_TFTHEIGHT;
  __ili_set_rotation(ili,0);

  ili->cursor_x = 0;
  ili->cursor_y = 0;
  ili->wrap = 0;
  ili->textsize = 1;
  ili->textcolor = ILI9341_GREEN;
  ili->textbgcolor = ILI9341_WHITE;

  return 0;

error:
  pmsis_l2_malloc_free(ili, sizeof(ili_t));
  return -1;
}



static int32_t __ili_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
  ili_t *ili = (ili_t *)device->data;

  switch (cmd)
  {
    case PI_ILI_IOCTL_ORIENTATION:
    __ili_set_rotation(ili, (uint8_t)(long)arg);
    return 0;
  }
  return -1;
}



static pi_display_api_t ili_api =
{
  .open           = &__ili_open,
  .write_async    = &__ili_write_async,
  .ioctl          = &__ili_ioctl
};



void pi_ili9341_conf_init(struct pi_ili9341_conf *conf)
{
  conf->display.api = &ili_api;
  conf->spi_itf = 0;
  conf->skip_pads_config = 0;
  __display_conf_init(&conf->display);
  bsp_ili9341_conf_init(conf);
}



static void __ili_write_8(ili_t *ili, uint8_t value)
{
  ili->temp_buffer[0] = value;
  pi_spi_send(&ili->spim, ili->temp_buffer, 8, PI_SPI_CS_AUTO);
}



static void __ili_write_16(ili_t *ili, uint16_t value)
{
  __ili_write_8(ili, value >> 8);
  __ili_write_8(ili, value);
}



static void __ili_write_32(ili_t *ili, uint32_t value)
{
  __ili_write_16(ili, value >> 16);
  __ili_write_16(ili, value);
}



static void __ili_write_command(ili_t *ili, uint8_t cmd)
{
  pi_gpio_pin_write(&ili->gpio_port, ili->gpio, 0);
  __ili_write_8(ili,cmd);
  pi_gpio_pin_write(&ili->gpio_port, ili->gpio, 1);
}



static void __ili_init(ili_t *ili)
{
  pi_gpio_pin_configure(&ili->gpio_port, ili->gpio, PI_GPIO_OUTPUT);
  pi_gpio_pin_write(&ili->gpio_port, ili->gpio, 0);

  __ili_write_command(ili,0xEF);
  __ili_write_8(ili,0x03);
  __ili_write_8(ili,0x80);
  __ili_write_8(ili,0x02);

  __ili_write_command(ili,0xCF);
  __ili_write_8(ili,0x00);
  __ili_write_8(ili,0XC1);
  __ili_write_8(ili,0X30);

  __ili_write_command(ili,0xED);
  __ili_write_8(ili,0x64);
  __ili_write_8(ili,0x03);
  __ili_write_8(ili,0X12);
  __ili_write_8(ili,0X81);

  __ili_write_command(ili, 0xE8);
  __ili_write_8(ili,0x85);
  __ili_write_8(ili,0x00);
  __ili_write_8(ili,0x78);

  __ili_write_command(ili,0xCB);
  __ili_write_8(ili,0x39);
  __ili_write_8(ili,0x2C);
  __ili_write_8(ili,0x00);
  __ili_write_8(ili,0x34);
  __ili_write_8(ili,0x02);

  __ili_write_command(ili,0xF7);
  __ili_write_8(ili,0x20);

  __ili_write_command(ili,0xEA);
  __ili_write_8(ili,0x00);
  __ili_write_8(ili,0x00);

  __ili_write_command(ili,ILI9341_PWCTR1);    //Power control
  __ili_write_8(ili,0x23);   //VRH[5:0]

  __ili_write_command(ili,ILI9341_PWCTR2);    //Power control
  __ili_write_8(ili,0x10);   //SAP[2:0];BT[3:0]

  __ili_write_command(ili,ILI9341_VMCTR1);    //VCM control
  __ili_write_8(ili,0x3e);
  __ili_write_8(ili,0x28);

  __ili_write_command(ili,ILI9341_VMCTR2);    //VCM control2
  __ili_write_8(ili,0x86);  //--

  __ili_write_command(ili,ILI9341_MADCTL);    // Memory Access Control
  __ili_write_8(ili,0x48);

  __ili_write_command(ili,ILI9341_VSCRSADD); // Vertical scroll
  __ili_write_16(ili,0);                 // Zero

  __ili_write_command(ili,ILI9341_PIXFMT);
  __ili_write_8(ili,0x55);

  __ili_write_command(ili,ILI9341_FRMCTR1);
  __ili_write_8(ili,0x00);
  __ili_write_8(ili,0x18);

  __ili_write_command(ili,ILI9341_DFUNCTR);    // Display Function Control
  __ili_write_8(ili,0x08);
  __ili_write_8(ili,0x82);
  __ili_write_8(ili,0x27);

  __ili_write_command(ili,0xF2);    // 3Gamma Function Disable
  __ili_write_8(ili,0x00);

  __ili_write_command(ili,ILI9341_GAMMASET);    //Gamma curve selected
  __ili_write_8(ili,0x01);

  __ili_write_command(ili,ILI9341_GMCTRP1);    //Set Gamma
  __ili_write_8(ili,0x0F);
  __ili_write_8(ili,0x31);
  __ili_write_8(ili,0x2B);
  __ili_write_8(ili,0x0C);
  __ili_write_8(ili,0x0E);
  __ili_write_8(ili,0x08);
  __ili_write_8(ili,0x4E);
  __ili_write_8(ili,0xF1);
  __ili_write_8(ili,0x37);
  __ili_write_8(ili,0x07);
  __ili_write_8(ili,0x10);
  __ili_write_8(ili,0x03);
  __ili_write_8(ili,0x0E);
  __ili_write_8(ili,0x09);
  __ili_write_8(ili,0x00);

  __ili_write_command(ili,ILI9341_GMCTRN1);    //Set Gamma
  __ili_write_8(ili,0x00);
  __ili_write_8(ili,0x0E);
  __ili_write_8(ili,0x14);
  __ili_write_8(ili,0x03);
  __ili_write_8(ili,0x11);
  __ili_write_8(ili,0x07);
  __ili_write_8(ili,0x31);
  __ili_write_8(ili,0xC1);
  __ili_write_8(ili,0x48);
  __ili_write_8(ili,0x08);
  __ili_write_8(ili,0x0F);
  __ili_write_8(ili,0x0C);
  __ili_write_8(ili,0x31);
  __ili_write_8(ili,0x36);
  __ili_write_8(ili,0x0F);

  __ili_write_command(ili,ILI9341_SLPOUT);    //Exit Sleep
  pi_time_wait_us(120000);
  __ili_write_command(ili,ILI9341_DISPON);    //Display on
  pi_time_wait_us(120000);

}



static void __ili_set_rotation(ili_t *ili, uint8_t m)
{
  int rotation = m % 4; // can't be higher than 3
  switch (rotation)
  {
    case 0:
      m = (MADCTL_MX | MADCTL_BGR);
      ili->_width  = ILI9341_TFTWIDTH;
      ili->_height = ILI9341_TFTHEIGHT;
      break;

    case 1:
      m = (MADCTL_MV | MADCTL_BGR);
      ili->_width  = ILI9341_TFTHEIGHT;
      ili->_height = ILI9341_TFTWIDTH;
      break;

    case 2:
      m = (MADCTL_MY | MADCTL_BGR);
      ili->_width  = ILI9341_TFTWIDTH;
      ili->_height = ILI9341_TFTHEIGHT;
      break;

    case 3:
      m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      ili->_width  = ILI9341_TFTHEIGHT;
      ili->_height = ILI9341_TFTWIDTH;
      break;
  }

  __ili_write_command(ili, ILI9341_MADCTL);
  __ili_write_8(ili, m);
}



static void __ili_gray8_to_rgb565(uint8_t *input,uint16_t *output,int width, int height)
{
  for(int i=0;i<width*height;i++)
  {
    output[i] = ((input[i] >> 3 ) << 3) | ((input[i] >> 5) ) | (((input[i] >> 2 ) << 13) )|   ((input[i] >> 3) <<8);
  }
}

static void __ili_rgb565_to_rgb565(uint16_t *input,uint16_t *output,int width, int height)
{
  for(int i=0;i<width*height;i++)
  {
    //This is a workaround, might be possible to set in LCD display
    output[i] = ((input[i] & 0xFF00) >> 8) | ((input[i] & 0x00FF) << 8);
  }
}



static void __ili_set_addr_window(ili_t *ili,uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint32_t xa = ((uint32_t)x << 16) | (x+w-1);
  uint32_t ya = ((uint32_t)y << 16) | (y+h-1);
  __ili_write_command(ili,ILI9341_CASET); // Column addr set
  __ili_write_32(ili,xa);
  __ili_write_command(ili,ILI9341_PASET); // Row addr set
  __ili_write_32(ili,ya);
  __ili_write_command(ili,ILI9341_RAMWR); // write to RAM
}



void writeColor(struct pi_device *device, uint16_t color, unsigned int len)
{
  ili_t *ili = (ili_t *)device->data;
  for (uint32_t t=0; t<len; t++)
  {
    __ili_write_16(ili, color);
  }
  return;
}

void writeFillRect(struct pi_device *device, unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color)
{
  ili_t *ili = (ili_t *)device->data;

  if((x >= ili->_width) || (y >= ili->_height)) return;
  unsigned short x2 = x + w - 1, y2 = y + h - 1;

  // Clip right/bottom
  if(x2 >= ili->_width)  w = ili->_width  - x;
  if(y2 >= ili->_height) h = ili->_height - y;
  unsigned int len = (int32_t)w * h;

  //setAddrWindow(spim,x, y, w, h);
  __ili_set_addr_window(ili, x, y, w, h); // Clipped area

  writeColor(device, color, len);
}


void setTextWrap(struct pi_device *device,signed short w)
{
  ili_t *ili = (ili_t *)device->data;
  ili->wrap = w;
}

void setCursor(struct pi_device *device,signed short x, signed short y)
{
  ili_t *ili = (ili_t *)device->data;
  ili->cursor_x = x;
  ili->cursor_y = y;
}

void setTextColor(struct pi_device *device,uint16_t c)
{
  ili_t *ili = (ili_t *)device->data;
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  ili->textcolor = c;
}

static void __ili_writePixelAtPos(struct pi_device *device,int16_t x, int16_t y, uint16_t color)
{
  ili_t *ili = (ili_t *)device->data;

  if((x < 0) ||(x >= (int)ili->_width) || (y < 0) || (y >= (int)ili->_height)) return;

  __ili_set_addr_window(ili, x, y, 1, 1); // Clipped area
  __ili_write_16(ili,color);
}

static void drawChar(struct pi_device *device,int16_t x, int16_t y, unsigned char c,
  uint16_t color, uint16_t bg, uint8_t size)
{
  ili_t *ili = (ili_t *)device->data;

  if((x >= (int)ili->_width)  || // Clip right
     (y >= (int)ili->_height) || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  //if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

  for(int8_t i=0; i<5; i++ )
  { // Char bitmap = 5 columns
    uint8_t line = font[c * 5 + i];
    for(int8_t j=0; j<8; j++, line >>= 1)
    {
      if(line & 1)
      {
        if(size == 1)
          __ili_writePixelAtPos(device,x+i, y+j, color);
        else
          writeFillRect(device,x+i*size, y+j*size, size, size, color);
      }
      else if(bg != color)
      {
        if(size == 1)
          __ili_writePixelAtPos(device,x+i, y+j, bg);
        else
          writeFillRect(device,x+i*size, y+j*size, size, size, bg);
      }
    }
  }
}


static void writeChar(struct pi_device *device,uint8_t c)
{
  ili_t *ili = (ili_t *)device->data;

  if(c == '\n')
  {                        // Newline?
    ili->cursor_x  = 0;                     // Reset x to zero,
    ili->cursor_y += ili->textsize * 8;          // advance y one line
  }
  else if (c != '\r')
  {                 // Ignore carriage returns
    if (ili->wrap && ((ili->cursor_x + ili->textsize * 6) > (int)ili->_width))
    { // Off right?
      ili->cursor_x  = 0;                 // Reset x to zero,
      ili->cursor_y += ili->textsize * 8;      // advance y one line
    }
    drawChar(device, ili->cursor_x, ili->cursor_y, c, ili->textcolor, ili->textbgcolor, ili->textsize);
    ili->cursor_x += ili->textsize * 6;          // Advance x one char
  }
}

void writeText(struct pi_device *device,char* str,int fontsize)
{
  ili_t *ili = (ili_t *)device->data;
  ili->textsize = fontsize;
  int i=0;
  while(str[i] != '\0')
    writeChar(device,str[i++]);
}
#endif
