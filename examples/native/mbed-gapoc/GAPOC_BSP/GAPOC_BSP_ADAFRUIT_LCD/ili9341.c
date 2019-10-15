// Code derived from ili9341 driver provided by Adafruit
// Some portions rewritten for better efficiency in the context of our SPI interface with uDMA

/*

ARDUINO SHIELD PINOUT

D0  Not Used
D1  Not Used
D2  Not Used
D3  Not Used
D4  TF_CS
D5  Not Used
D6  Not Used
D7  Backlight Control
D8  Not Used
D9  TFT_D/C
D10 TFT_CS
D11 SPI_MOSI
D12 SPI_MISO
D13 SPI_SCK

*/
#include "GAPOC_BSP_General.h"

#include "ili9341.h"
#include "font.c"

#include "spi_multi_api.h"
#include "mbed_wait_api.h"



#define MADCTL_MY  0x80     ///< Bottom to top
#define MADCTL_MX  0x40     ///< Right to left
#define MADCTL_MV  0x20     ///< Reverse Mode
#define MADCTL_ML  0x10     ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00     ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08     ///< Blue-Green-Red pixel order
#define MADCTL_MH  0x04     ///< LCD refresh right to left


#define SSPI_WRITE(a,v)          spiWrite(a,v)                ///< Software SPI write 8 bits
#define SPI_WRITE16(a,s)         SSPI_WRITE(a,(s) >> 8); SSPI_WRITE(a,s)  ///< Software SPI write 16 bits
#define SSPI_WRITE32(a,l)        SSPI_WRITE(a,(l >> 24)); SSPI_WRITE(a,(l >> 16)); SSPI_WRITE(a,(l >> 8)); SSPI_WRITE(a,l)   ///< Software SPI write 32 bits


//Use int type to avoid warning when comparing those to signed values (XC):
GAP_L2_DATA int _width=0;
GAP_L2_DATA int _height=0;

GAP_L2_DATA int16_t cursor_x=0;
GAP_L2_DATA int16_t cursor_y=0;
GAP_L2_DATA int16_t wrap=0;
GAP_L2_DATA uint8_t textsize=1;
GAP_L2_DATA uint16_t textcolor=ILI9341_GREEN;
GAP_L2_DATA uint16_t textbgcolor=ILI9341_WHITE;

// This function was added wrt original ili9341.c, specific to GAPOC
void  GAPOC_LCD_Init(spi_t* spim)
{
    /* SPI pins init, SPI udma channel init */
    #if (GAPOC_PLATFORM == GAPOC_B)
        #define  LCD_SPI_CSN   SPI1_CSN1_B2
    #else
        #define  LCD_SPI_CSN   SPI1_CSN0_A5
    #endif
    spi_init(spim, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, LCD_SPI_CSN);   // signals available on CONN3


    /* SPI Nbits, Mode (cpha, cpol),Slave/nMaster */
    spi_format(spim, 8, 0, 0);

    /* Set SPI fequency */
    spi_frequency(spim, GAPOC_LCD_SPI_FQCY_MHz*1000000);

    DBG_PRINT("SPI Config done...\n");

    ILI9341_begin(spim);

//    writeFillRect(spim, 0, 0, lcdW, lcdH, ILI9341_WHITE);
      writeFillRect(spim, 0, 0, lcdH, lcdW, ILI9341_WHITE);

    setTextColor(ILI9341_BLUE);

    setRotation(spim,0);
    setCursor(10, 10);
    writeText(spim,"LCD READY",sizeof("LCD READY"),3);
    setRotation(spim,1);
    setCursor(10, 10);
    writeText(spim,"LCD READY",sizeof("LCD READY"),3);
    setRotation(spim,2);
    setCursor(10, 10);
    writeText(spim,"LCD READY",sizeof("LCD READY"),3);
    setRotation(spim,3);
    setCursor(10, 10);
    writeText(spim,"LCD READY",sizeof("LCD READY"),3);

    DBG_PRINT("LCD Config done...\n");

}


void ILI9341_begin(spi_t* spim)
{
    // Define D/C (data/control) pin for LCD shield
#if ( GAPOC_PLATFORM == GAPOC_B )
    #define TFT_SPI_DC  GPIO_A19      // GPIO_A19 on pin B12 of GAP
#else
    #define TFT_SPI_DC  GPIO_A0_A3      // GPIO_A0 on pin A3 of GAP
#endif
    GAPOC_GPIO_Init_Pure_Output_High(TFT_SPI_DC);

    //startWrite();
    writeCommand(spim,0xEF);
    spiWrite(spim,0x03);
    spiWrite(spim,0x80);
    spiWrite(spim,0x02);

    writeCommand(spim,0xCF);
    spiWrite(spim,0x00);
    spiWrite(spim,0XC1);
    spiWrite(spim,0X30);

    writeCommand(spim,0xED);
    spiWrite(spim,0x64);
    spiWrite(spim,0x03);
    spiWrite(spim,0X12);
    spiWrite(spim,0X81);

    writeCommand(spim, 0xE8);
    spiWrite(spim,0x85);
    spiWrite(spim,0x00);
    spiWrite(spim,0x78);

    writeCommand(spim,0xCB);
    spiWrite(spim,0x39);
    spiWrite(spim,0x2C);
    spiWrite(spim,0x00);
    spiWrite(spim,0x34);
    spiWrite(spim,0x02);

    writeCommand(spim,0xF7);
    spiWrite(spim,0x20);

    writeCommand(spim,0xEA);
    spiWrite(spim,0x00);
    spiWrite(spim,0x00);

    writeCommand(spim,ILI9341_PWCTR1);    //Power control
    spiWrite(spim,0x23);   //VRH[5:0]

    writeCommand(spim,ILI9341_PWCTR2);    //Power control
    spiWrite(spim,0x10);   //SAP[2:0];BT[3:0]

    writeCommand(spim,ILI9341_VMCTR1);    //VCM control
    spiWrite(spim,0x3e);
    spiWrite(spim,0x28);

    writeCommand(spim,ILI9341_VMCTR2);    //VCM control2
    spiWrite(spim,0x86);  //--

    writeCommand(spim,ILI9341_MADCTL);    // Memory Access Control
    spiWrite(spim,0x48);

    writeCommand(spim,ILI9341_VSCRSADD); // Vertical scroll
    SPI_WRITE16(spim,0);                 // Zero

    writeCommand(spim,ILI9341_PIXFMT);
    spiWrite(spim,0x55);

    writeCommand(spim,ILI9341_FRMCTR1);
    spiWrite(spim,0x00);
    spiWrite(spim,0x18);

    writeCommand(spim,ILI9341_DFUNCTR);    // Display Function Control
    spiWrite(spim,0x08);
    spiWrite(spim,0x82);
    spiWrite(spim,0x27);

    writeCommand(spim,0xF2);    // 3Gamma Function Disable
    spiWrite(spim,0x00);

    writeCommand(spim,ILI9341_GAMMASET);    //Gamma curve selected
    spiWrite(spim,0x01);

    writeCommand(spim,ILI9341_GMCTRP1);    //Set Gamma
    spiWrite(spim,0x0F);
    spiWrite(spim,0x31);
    spiWrite(spim,0x2B);
    spiWrite(spim,0x0C);
    spiWrite(spim,0x0E);
    spiWrite(spim,0x08);
    spiWrite(spim,0x4E);
    spiWrite(spim,0xF1);
    spiWrite(spim,0x37);
    spiWrite(spim,0x07);
    spiWrite(spim,0x10);
    spiWrite(spim,0x03);
    spiWrite(spim,0x0E);
    spiWrite(spim,0x09);
    spiWrite(spim,0x00);

    writeCommand(spim,ILI9341_GMCTRN1);    //Set Gamma
    spiWrite(spim,0x00);
    spiWrite(spim,0x0E);
    spiWrite(spim,0x14);
    spiWrite(spim,0x03);
    spiWrite(spim,0x11);
    spiWrite(spim,0x07);
    spiWrite(spim,0x31);
    spiWrite(spim,0xC1);
    spiWrite(spim,0x48);
    spiWrite(spim,0x08);
    spiWrite(spim,0x0F);
    spiWrite(spim,0x0C);
    spiWrite(spim,0x31);
    spiWrite(spim,0x36);
    spiWrite(spim,0x0F);

    writeCommand(spim,ILI9341_SLPOUT);    //Exit Sleep

//    rt_time_wait_us(120000);
    wait(0.12); // Mbed style

    writeCommand(spim,ILI9341_DISPON);    //Display on
//    rt_time_wait_us(120000);
    wait(0.12); // Mbed style

    //endWrite();

    _width  = ILI9341_TFTWIDTH;
    _height = ILI9341_TFTHEIGHT;

}


#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif


void setTextWrap(signed short w) {
    wrap = w;
}

void setCursor(signed short x, signed short y) {
    cursor_x = x;
    cursor_y = y;
}

void setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = c;
}

void writePixelAtPos(spi_t *spim, int16_t x, int16_t y, uint16_t color) {
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
    setAddrWindow(spim,x,y,1,1);
    writePixel(spim,color);
}

void drawChar(spi_t *spim,int16_t x, int16_t y, unsigned char c,
  uint16_t color, uint16_t bg, uint8_t size) {

        if((x >= _width)            || // Clip right
           (y >= _height)           || // Clip bottom
           ((x + 6 * size - 1) < 0) || // Clip left
           ((y + 8 * size - 1) < 0))   // Clip top
            return;

        //if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

        for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
            uint8_t line = pgm_read_byte(&font[c * 5 + i]);
            for(int8_t j=0; j<8; j++, line >>= 1) {
                if(line & 1) {
                    if(size == 1)
                        writePixelAtPos(spim,x+i, y+j, color);
                    else
                        writeFillRect(spim,x+i*size, y+j*size, size, size, color);
                } else if(bg != color) {
                    if(size == 1)
                        writePixelAtPos(spim,x+i, y+j, bg);
                    else
                        writeFillRect(spim,x+i*size, y+j*size, size, size, bg);
                }
            }
        }


    }


void writeChar(spi_t *spim,uint8_t c) {

     if(c == '\n') {                        // Newline?
            cursor_x  = 0;                     // Reset x to zero,
            cursor_y += textsize * 8;          // advance y one line
        } else if(c != '\r') {                 // Ignore carriage returns
            if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
                cursor_x  = 0;                 // Reset x to zero,
                cursor_y += textsize * 8;      // advance y one line
            }
            drawChar(spim,cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            cursor_x += textsize * 6;          // Advance x one char
        }
}

void writeText(spi_t *spim,char* str,int size,int fontsize){
    textsize= fontsize;
    for(int i=0;i<size;i++)
        writeChar(spim,str[i]);
}

void setRotation(spi_t *spim,unsigned char m) {
    int rotation = m % 4; // can't be higher than 3
    switch (rotation) {
        case 0:
            m = (MADCTL_MX | MADCTL_BGR);
            _width  = ILI9341_TFTWIDTH;
            _height = ILI9341_TFTHEIGHT;
            break;
        case 1:
            m = (MADCTL_MV | MADCTL_BGR);
            _width  = ILI9341_TFTHEIGHT;
            _height = ILI9341_TFTWIDTH;
            break;
        case 2:
            m = (MADCTL_MY | MADCTL_BGR);
            _width  = ILI9341_TFTWIDTH;
            _height = ILI9341_TFTHEIGHT;
            break;
        case 3:
            m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            _width  = ILI9341_TFTHEIGHT;
            _height = ILI9341_TFTWIDTH;
            break;
    }

    //startWrite();
    writeCommand(spim, ILI9341_MADCTL);
    spiWrite(spim, m);
   // endWrite();
}

void writePixel(spi_t *spim,unsigned short color){
    SPI_WRITE16(spim, color);
}

void writeColor(spi_t *spim, unsigned short color, unsigned int len)
{
    spi_command_sequence_t sequence;
    memset(&sequence, 0, sizeof(spi_command_sequence_t));  // clear structure contents

    for (uint32_t i=0; i<len; i++)
    {
        sequence.tx_bits = 16;
        sequence.tx_data = color;
        spi_master_transfer_command_sequence(spim, &sequence);  // this automatically handles the CS (I think)
    }
}


void writePixels(spi_t *spim, uint16_t* Pixels_rgb565, uint32_t Num_pixels)
{
    spi_command_sequence_t sequence;
    memset(&sequence, 0, sizeof(spi_command_sequence_t));  // clear structure contents

    sequence.tx_bits   = Num_pixels*16;
    sequence.tx_buffer = (uint8_t*) &Pixels_rgb565[0];
    spi_master_transfer_command_sequence(spim, &sequence);  // this automatically handles the CS (I think)
}

void drawRGBBitmap(spi_t *spim,signed short x, signed short y,
  unsigned short *pcolors, signed short w, signed short h) {

    int16_t x2, y2; // Lower-right coord
    if(( x             >= _width ) ||      // Off-edge right
       ( y             >= _height) ||      // " top
       ((x2 = (x+w-1)) <  0      ) ||      // " left
       ((y2 = (y+h-1)) <  0)     ) return; // " bottom

    int16_t bx1=0, by1=0, // Clipped top-left within bitmap
            saveW=w;      // Save original bitmap width value
    if(x < 0) { // Clip left
        w  +=  x;
        bx1 = -x;
        x   =  0;
    }
    if(y < 0) { // Clip top
        h  +=  y;
        by1 = -y;
        y   =  0;
    }
    if(x2 >= _width ) w = _width  - x; // Clip right
    if(y2 >= _height) h = _height - y; // Clip bottom

    pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
    //startWrite();
    setAddrWindow(spim,x, y, w, h); // Clipped area
    while(h--) { // For each (clipped) scanline...
      writePixels(spim,pcolors, w); // Push one (clipped) row
      pcolors += saveW; // Advance pointer by one full (unclipped) line
    }
    //endWrite();
}

#define LINES_PER_CHUNK 12

// TODO -- (WIP) Rewrite this....
//void lcd_pushPixels(spi_t *spim,  uint16_t viewport_start_x, uint16_t viewport_start_y,uint16_t viewport_w,uint16_t viewport_h, uint16_t* pBuffer)
void GAPOC_LCD_pushPixels(spi_t *spim,  uint16_t viewport_start_x, uint16_t viewport_start_y,uint16_t viewport_w,uint16_t viewport_h, uint16_t* pBuffer)
{

    setAddrWindow(spim, viewport_start_x, viewport_start_y, viewport_w, viewport_h); // Clipped area

    for(int i=0; i<viewport_h; i+=LINES_PER_CHUNK)
    {
        writePixels(spim, pBuffer+(viewport_w*i), viewport_w*LINES_PER_CHUNK);
    }

}


void setAddrWindow(spi_t *spim,unsigned short x, unsigned short y, unsigned short w, unsigned short h) {
    uint32_t xa = ((uint32_t)x << 16) | (x+w-1);
    uint32_t ya = ((uint32_t)y << 16) | (y+h-1);
    writeCommand(spim,ILI9341_CASET); // Column addr set
    SSPI_WRITE32(spim,xa);
    writeCommand(spim,ILI9341_PASET); // Row addr set
    SSPI_WRITE32(spim,ya);
    writeCommand(spim,ILI9341_RAMWR); // write to RAM
}

void writeFillRect(spi_t *spim, unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color){
    if((x >= _width) || (y >= _height)) return;
    unsigned short x2 = x + w - 1, y2 = y + h - 1;

    // Clip right/bottom
    if(x2 >= _width)  w = _width  - x;
    if(y2 >= _height) h = _height - y;
    unsigned int len = (int32_t)w * h;

    setAddrWindow(spim,x, y, w, h);
    writeColor(spim,color, len);
}


void spiWrite(spi_t *spim, unsigned char  d)
{
    spi_command_sequence_t sequence;
    memset(&sequence, 0, sizeof(spi_command_sequence_t));  // clear structure contents

    sequence.tx_bits = 8;
    sequence.tx_data = d;
    spi_master_transfer_command_sequence(spim, &sequence);  // this automatically handles the CS (I think)
}


void writeCommand(spi_t *spim, unsigned char cmd){
    GAPOC_GPIO_Set_Low(TFT_SPI_DC);
    spiWrite(spim,cmd);
    GAPOC_GPIO_Set_High(TFT_SPI_DC);
}
