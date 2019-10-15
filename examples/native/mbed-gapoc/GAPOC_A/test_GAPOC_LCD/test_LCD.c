/*
 * Copyright (c) 2019, GreenWaves Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 
#include "GAPOC_BSP_General.h"

#include "mbed_wait_api.h"

#include "GAPOC_BSP_ADAFRUIT_LCD.h"

#define lcdW    320
#define lcdH    240


int main()
{

    DBG_PRINT("Starting LCD test...\n");
    DBG_PRINT("LCD Width = %d, LCD Height = %d\n", lcdW, lcdH);


    // -- Init SPI to interface with LCD  ----------------
 
    spi_t  spim;
    
    // SPI pins init, SPI udma channel init
    spi_init(&spim, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CSN0_A5);
       // on GAP_B3_CONN, GAP_A4_CONN, GAP_B4_CONN, GAP_A5_CONN


    // SPI Nbits, Mode (cpha, cpol),Slave/nMaster 
    spi_format(&spim, 8, 0, 0);

    // Set SPI fequency 
    spi_frequency(&spim, GAPOC_LCD_SPI_FQCY_MHz*1000000);

    printf("SPI Config done...\n"); 


    // -- Configure LCD  --------------------------------
    
    ILI9341_begin(&spim);  
    setRotation(&spim,1);

    printf("LCD Config done...\n"); 
    
    
    // -- Initialize data for test patterns   -----------
        
    uint16_t Palette[19] = {  ILI9341_BLACK, 
                                ILI9341_NAVY,
                                ILI9341_DARKGREEN,
                                ILI9341_DARKCYAN ,
                                ILI9341_MAROON,
                                ILI9341_PURPLE ,
                                ILI9341_OLIVE,
                                ILI9341_LIGHTGREY,
                                ILI9341_DARKGREY,
                                ILI9341_BLUE ,
                                ILI9341_GREEN,
                                ILI9341_CYAN,
                                ILI9341_RED ,
                                ILI9341_MAGENTA ,
                                ILI9341_YELLOW,
                                ILI9341_WHITE ,
                                ILI9341_ORANGE,
                                ILI9341_GREENYELLOW,
                                ILI9341_PINK };
                           
                            
    static uint8_t  Test_Pattern_gray8[lcdW*lcdH];  
    static uint16_t  Test_Pattern_rgb565[lcdW*lcdH];                                            
    for (uint32_t i=0; i<lcdH; i++)
    {
        for (uint32_t j=0; j<lcdW; j++)
        {
            Test_Pattern_gray8[i*lcdW +j] = i; 
        }
    }
    gray8_to_RGB565(Test_Pattern_gray8, Test_Pattern_rgb565, lcdW , lcdH);


    // -- Display stuff   ----------------------------------
    uint8_t i =0;    
    while(1)
    {
            {
            writeFillRect(&spim, 0,0,lcdW,lcdH,ILI9341_WHITE);
        
            setTextColor(Palette[i++]);
            setCursor(10,20);  // (X,Y) of bottom left of text frame
            writeText(&spim,"GreenWaves \nTechnologies\n",sizeof("GreenWaves \nTechnologies"),1);  // smallest text --> comes out always black ?
            setCursor(10,60);
            writeText(&spim,"GreenWaves \nTechnologies\n",sizeof("GreenWaves \nTechnologies"),2);
            setCursor(10,160);
            writeText(&spim,"GreenWaves \nTechnologies\n",sizeof("GreenWaves \nTechnologies"),3);

            wait(1);
            
            GAPOC_LCD_pushPixels(&spim, 0, 0, lcdW,lcdH, Test_Pattern_rgb565);
            wait(1);
        }
    }
    
    printf("end of the test\n");  // Never reached !

}
