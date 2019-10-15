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


// #############################################################################
// ##### DECLARATIONS  ########################################################

// ==  Handy "defines" for application, used locally   =====================

#define ON_LCD  0
#define ON_PC   1    
#define NO_DISP   2    


/** ****************************
// Select below if you want the pictures to be displayed on an Adafruit LCD attached to GAPOC
// (select ON_LCD) or to be saved as .ppm on PC (select ON_PC) -- in this case you must have JTAG connected 
// or not displayed at all (select NO_DISP)
** ****************************/
// ===========================

#define DISPLAY     ON_PC    

// ===========================


#define PIC_WIDTH    80   // IR Proxy Frame = 80x80 pixels at 2bytes per pix (14bits provided as 2 successive bytes) -- temp. info is *not* included in data
#define PIC_HEIGHT   80    
#define PIC_SIZE    (PIC_WIDTH*PIC_HEIGHT)  



// ====  Includes    ==========================================================

#include "GAPOC_BSP_General.h"

#include "GAPOC_BSP_IRPROXY.h"


#if (DISPLAY == ON_LCD) 
  #include "GAPOC_BSP_ADAFRUIT_LCD.h"   
#endif

#include "mbed_wait_api.h"


// ====  Application's Custom Types    ========================================
// <none>

    
// ==== Application's variables  ==============================

// Note: pragma GAP_L2_DATA makes sure data is stored in L2 (just to be explicit -- is normally the case anyway for all global variables)
//    as uDMA (used eg. for SPI or CPI transfers) can only access data in L2


// Two buffers to be used in ping-pong mode to store data from CPI in one while other is being processed :
GAP_L2_DATA   uint16_t buffer_processing_14bpp[ PIC_WIDTH*PIC_HEIGHT ];   // 14 bits per pixel, on 16 bits with 2MSBs="00"  
GAP_L2_DATA   uint16_t buffer_receiving_14bpp[ PIC_WIDTH*PIC_HEIGHT ];   // 14 bits per pixel, on 16 bits with 2MSBs="00"  

GAP_L2_DATA   int16_t buffer_ref_s16[ PIC_WIDTH*PIC_HEIGHT ];   // to store reference picture (shutter)
GAP_L2_DATA   int16_t buffer_diff_s16[ PIC_WIDTH*PIC_HEIGHT ];  // diff between last captured picture and reference picture
GAP_L2_DATA   uint8_t  scaled_buffer_diff_u8[ PIC_WIDTH*PIC_HEIGHT ];  // diff scaled to exploit full 8-bit range for display @8bpp

#if (DISPLAY == ON_LCD)
  GAP_L2_DATA  uint16_t image_buffer_rgb565[ lcdW*lcdH ];   // RGB565 buffer for LCD display
#elif (DISPLAY == ON_PC)
  uint32_t  dummy_frames_before_PC_store = 0;
#endif            


CPI_Type *const cpi_address[] = CPI_BASE_PTRS;
static GAP_L2_DATA    cpi_config_t cpi_config;
static GAP_L2_DATA    cpi_transfer_t cpiTransfer;  
static GAP_L2_DATA    cpi_handle_t hCPI;  
static volatile uint32_t Picture_Index = 0;  // to manage non-blocking picture transfer (snapshot)

static GAP_L2_DATA   spi_t   spim1;      
static GAP_L2_DATA  uint16_t  spi_word16;




// ==== Application's Function Prototypes    ===================================


// TODO: Move to BSP library
/*
void GAPOC_IRProxy_WriteReg12(uint8_t RegAddr, uint16_t WriteVal);
void GAPOC_IRProxy_SPI_Init(uint32_t spi_fqcy_khz);
*/

// Local helper functions
static void Callback_Got_Frame();   
static void save_pict_ppm( unsigned char* pic_buffer);
static void convert_14bpp_to_8bpp( unsigned char* pic_buffer_14bpp, unsigned char* pic_buffer_8bpp);


// #############################################################################
// ##### MAIN APPLICATION  ###########################################

int main()
{

uint32_t warmup_frame_count = 0;      
bool Calib_Done;


    DBG_PRINT("\nBasic Test of Proxy with ULIS IR Sensor on DF12 Connector CONN8\n\n");
    
    DBG_PRINT("Through your #define DISPLAY, you have selected to ");
    
#if  DISPLAY == ON_LCD
    DBG_PRINT("display an image of captured IR data on LCD\n\n");
    
#elif  DISPLAY == ON_PC
    DBG_PRINT("save an image as .ppm e.g. for diplay on PC\n\n");
    // This is to be able to use debug bridge to sace .ppm picture on host PC :
    BRIDGE_Init();
    printf("Connecting to bridge\n");
    BRIDGE_Connect(0, NULL);
    printf("Connection done\n\n");
    
#elif  DISPLAY == NO_DISP
    DBG_PRINT("not to display an image\n\n");   
    
#else
    #error "you didn't properly #define DISPLAY"   
#endif
    
    

    // ----  Initializations   -------------------------------------------------------------
    
    // Get pointers to arrays as we'll want to swap roles (ping-pong buffers) and it's easy to do with pointers, not arrays
    uint16_t* ptr_Processing = buffer_processing_14bpp;   // address of Processing Array 
    uint16_t* ptr_Receiving = buffer_receiving_14bpp;     // address of Receiving Array  
     
    
    //  --  Initalize Board (GPIO direction and default level, supplies, etc.)       
    GAPOC_BSP_Board_Init();
    
    // Might want to set new frequency if willing to run faster than default 50MHz 
    // either here using FLL_SetFrequency() or by changing defined symbol in GAPOC_BSP_Platform_Defs.h
    // Caution (FIXME): wait() funnction used below assumes default clock (50MHz) and does not scale with clock
    //  so required latencies inserted in code may need tuning if core clock is changed 
 
       
    // To avoid "Vsync-vs-B34 as GPIO"pad frame bug on GAP8 Cut1.0/1.1
    // To be used with modified board or sensor emulator,
    // won't work with IRProxy as is since this GPIO enables MCLK   
    GAPOC_AnyPin_Config( B34, PULL, uPORT_MuxAlt0 );  
        // pin GAP_B34 keeps default function (I2C_SDA) rather than use as GPIO_CIS_CLK -- Pull-Down ensures GPIO_CIS_CLK will be low so clk active
    
  
    // -- Initialize SPI1 for Proxy Use
    GAPOC_IRProxy_SPI_Init( SPI_FQCY_IRSENSOR_KHZ );  // SPI_FQCY_IRSENSOR_KHZ as defined in GAPOC_BSP_platform_Defs.h

    
    // Make sure pin A4 (can be SPI MISO but not used here) is configured as GPIO, used as Master Clock Enable   
    GAPOC_GPIO_Init_Pure_Output_High( GPIO_CIS_CLK_NOTEN ); // ACTIVE LOW (=standby pin) so init'd as not active


    // --  Initialize Heartbeat LED (LED enabled only if DIP Switch S6 is on) 
    //   !! BEWARE !! not available if LCD used as same GPIO is used as LCD control line
#if  DISPLAY != ON_LCD
    GAPOC_GPIO_Init_Pure_Output_High( GAPOC_HEARTBEAT_LED );
#endif


    // -- If wished, enable Vsync-controlled LED  (keep GPIO_CIS_LED_ENB = hi-Z to disable)
    GAPOC_GPIO_Init_Pure_Output_Low( GPIO_CIS_LED_ENB );  // Set Low to have LED on when Vsync is high, High to have LED on when Vsync is low


    // -- Init CPI i/f        ------------------------------------  

    CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7); 
        // UDMA_Init() is included in the above                


    // -- Configure CPI i/f , Monochrome raw data   -----------------------------------  


    CPI_GetDefaultConfig(&cpi_config);
    cpi_config.row_len = 2*PIC_WIDTH ; 
    cpi_config.resolution      = 2*PIC_WIDTH*PIC_HEIGHT;   // factro 2 as at 14bpp we're getting 2 bytes for one pixel 
    cpi_config.format          = BYPASS_LITEND;  
        // here we want 1st byte received over CPI (= MSByte of 14bit pixe) to be stored at address  0x01 and 2nd byte (=LSByte) at addres 0x00
        // as uDMA assumes 16 bit data is stored in little-endian format
    cpi_config.shift           = 0;
    cpi_config.slice_en        = 0;
    cpi_config.frameDrop_en    = 0;
    cpi_config.frameDrop_value = 0;
    cpi_config.wordWidth       = 16;  // 8, 16 or 32bits // IN FACT IGNORED in CPI DRIVER [gap_cpi.c] !!!!
        // Those settings will be used by CPI_Enable
 
   
    // Set-up uDMA for getting data from CPI:

    //  cpiTransfer.data      which is target buffer address will be set later (different buffers for ref picture and captured picture);
    cpiTransfer.dataSize    = PIC_SIZE*2;  // 2 bytes per pix !
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1); //0 -> 8bit //1 -> 16bit //2 -> 32bit -- ALWAYS USE 16-bits for CPI uDMA (?)
        // Will be used by CPI Reception function (blocking or non-blocking)
        

    DBG_PRINT("Width=%d, Height=%d, Size=%d\n", cpi_config.row_len, PIC_HEIGHT, cpiTransfer.dataSize);



              
    // -- Start IR Proxy:              ----------------------------------------------------------------------

    #define  MIN_INTERCOMMAND_DELAY_ms  50.0  // Proxy requires 10ms min between 2 SPI commands and even up to 50ms (>16ms looks ok though) after clock setting 
        // TODO reduce this delay as much as possible

    
    // --  Enable 3V3A/3V3D   -------------------
    GAPOC_GPIO_Set_High( GPIO_CIS_APWRON );              
    GAPOC_GPIO_Set_High( GPIO_CIS_DPWRON );  

    
    // --  Initialize trigger signal   ---------    
     GAPOC_GPIO_Set_High(GPIO_CIS_TRIGGER);      // Make sure trigger input is inactive (active high for snapshot mode A, active low for snapshot mode B)
        // GPIO_A3 on pin B1

       
        
    // --  Master Clock On   --------------------
    // When voltages are established, wait 150ms and apply Master Clock
    #define  MCLOCK_TURNON_LATENCY_ms       200.0   // was 150.0
    // Reworked board: Turn on master clock through GAP_A4 = GPIO_CIS_CLK_NOTEN
    GAPOC_GPIO_Set_Low( GPIO_CIS_CLK_NOTEN ); // ACTIVE LOW (=standby pin)
          
    // NOTE: GAPOC_B (0.1) when using dedicated ClkGen for MCLK provides 2.048MHz
    //  while latest Proxy DS specifies 3.8MHz (earlier versions said 1.9MHZ with 2.048 OK)
    //  Impact: frame rate limited to 30Hz instead of 60Hz

    #define  IRPROXY_SETTLING_TIME_ms       150.0
    wait( (float) IRPROXY_SETTLING_TIME_ms/1000.0  );  // else upcoming SPI prog may not be properly taken into account ? (min duration TBC)


     
    // == Proxy is now ready to be configured through SPI Bus : ====
                   
    // --- ClkDiv Register  -----------
    GAPOC_IRProxy_WriteReg12( IRPROXY_CKDIV, 0x01);  // from DS, would expect PCLK = 0.5*MCLK/(2**(0x01+1)) = 2.048MHz/8 = 256KHz -- actually observing 512KHz..    
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);    
    
    // ---  Trigger mode Register  - ---    
    GAPOC_IRProxy_WriteReg12( IRPROXY_TRIGGER_ADDR, IRPROXY_TRIGGER_ADDR_MAGIC_NUM);
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);        
    GAPOC_IRProxy_WriteReg12( IRPROXY_TRIGGER_VAL, IRPROXY_TRIGGER_VAL_TRIGMODE_B);    // "FullScale trigger" mode
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);      
  
    // --- Integration Time Register  ---
    GAPOC_IRProxy_WriteReg12( IRPROXY_TINT, 0x10);      // DS says (hardwired) default is 0x10
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);    
        
    // --- GFID Register   --------------
    GAPOC_IRProxy_WriteReg12( IRPROXY_GFID, 0xBB0);     // DS says h/w default is 0xBB0 
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);    
            
    // --- GSK  Register   --------------
    GAPOC_IRProxy_WriteReg12( IRPROXY_GSK,  0x4C0);      // DS says h/w default is 0x540 -- 0x4C0 seems to work well
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);    
           
    // --- GMS  Register   --------------
    GAPOC_IRProxy_WriteReg12( IRPROXY_GMS, 0x053);      // DS says h/w default is 0x053  (--> Gain 2, Gain 0, UpCol, UpRow bits)  
    wait(MIN_INTERCOMMAND_DELAY_ms/1000.0);    
        
        
    DBG_PRINT("IR Proxy Ready and Configured\n");  
    
    

    // --- Initialize LCD for display of results if needed ------------------------------------------------------
    
    // BEWARE: LCD shield uses same SPIM1 as Proxy;
    //         Initializing LCD will assign SPI to LCD shield and make it no more usable by Proxy
#if (DISPLAY == ON_LCD)
    GAPOC_LCD_Init( &spim1 );   // configures SPI1 for LCD & displays welcome message
#endif



    // -- Now prepare to capture frames in continuous mode with non-blocking reception ---------------------------------------
    
    
    // Enable CPI
    CPI_Enable(cpi_address[0], &cpi_config); // Activate configuration of CPI channel -- Starts gated clock of CPI; needed if disabled in callback (to save power)

      
    // Prepare handles for non-blocking camera capture
    CPI_ReceptionCreateHandle(cpi_address[0], &hCPI, Callback_Got_Frame, NULL);  
    DBG_PRINT("CPI Handle created\n");


    // --  Information about shuttered picture to use as reference :   -------------------------------------------
    
#if (DISPLAY == ON_LCD)    
    writeFillRect(&spim1, (lcdW-PIC_WIDTH)/2, (lcdH-PIC_HEIGHT)/2, PIC_WIDTH, PIC_HEIGHT, ILI9341_BLACK);   
    DBG_PRINT("\n\n*****    APPLY SHUTTER till IR picture displayed    ****** \n\n");
#else
    GAPOC_GPIO_Set_High( GAPOC_HEARTBEAT_LED );
    DBG_PRINT("\n\n*****    APPLY SHUTTER till Green Led (LED3) turns off    ****** \n\n");   
#endif
    Calib_Done = false;
    
    
    
    //  ===   MAIN LOOP    =================================================================================================       

          
    // -- Indicate initial receive buffer and launch First capture by camera :
    
    cpiTransfer.data        = (unsigned char*)ptr_Receiving;  // store picture in buffer for reference picture
        // data received 8 bytes at a time over CPI is stored in a 16bpp buffer as data from IR PRoxy is actually 14 bits split in 2 bytes 

    CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);  

    GAPOC_GPIO_Set_Low(GPIO_CIS_TRIGGER);      // Start stream      
    // Now free running...


    while(1)
    {    
    
        // Create working copies of pointer to be able to alter it while keeping memory of inital value 
        uint16_t* ptr_Processing_tmp = ptr_Processing;
        
        // Use xx frames as "warm-up" and to leave time for user to put shutter in fromnt of lens, 
        // the last frame of this serires will be used as refence.
        // TODO: also allow calibration to be explictly required by user at any time e.g. by (temporarily) pulling up a GPIO on connector pin or through UART
        #define NUM_WARMUP_FRAMES   100
        if (warmup_frame_count++ == NUM_WARMUP_FRAMES)
        {
            Calib_Done  = true;  // and this is the picture that will actually be that taken as reference
          #if (DISPLAY != ON_LCD)                                 
            // If LCD not used : flag shutter done by turning Green Led off
            GAPOC_GPIO_Set_Low( GAPOC_HEARTBEAT_LED );     
          #endif              
        }



        // Process Buffer A (ping buffer) while buffer B is being filled (pong buffer) :
        
        if (!Calib_Done)    // Treat frame as reference frame (to be captured with shutter in front of lens) -- even though only last one will be actually used as ref
        {         
                
            // Get_Frame_Average( uint16_t* buffer_processing_14bpp );
            uint32_t frame_avg = 0;
            for (uint32_t i=0; i<PIC_SIZE; i++)
            {
                frame_avg += *ptr_Processing_tmp++;  // progressing in 
            }
            frame_avg = frame_avg /PIC_SIZE;
            ptr_Processing_tmp = ptr_Processing;   // re-init pointer after use (incremantations)
            
            // Now substract frame average from each pixel
            for (uint32_t i=0; i<PIC_SIZE; i++)
            {
                buffer_ref_s16[i] = *ptr_Processing_tmp++ - frame_avg;
            }         
            ptr_Processing_tmp = ptr_Processing;   // re-init pointer after use (incrementations)
                  
        }
 
        
        else      // Calibration done
        {
            // get difference between captured frame and reference frame (shutter)
            // also remember min and max value
            int32_t MIN_buffer_diff = (1<<14);  // init at max possible value
            int32_t MAX_buffer_diff = -(1<<14);  // init at min possible value
            
            for (uint32_t i=0; i<PIC_SIZE; i++)
            {
                buffer_diff_s16[i] = *ptr_Processing_tmp++ - buffer_ref_s16[i];
                if (buffer_diff_s16[i] <0)
                {
                    DBG_PRINT("Negative value!\n");
                    buffer_diff_s16[i] = 0;
                }
                if (buffer_diff_s16[i] > (1<<14))
                {
                    DBG_PRINT("Saturation!\n");
                    buffer_diff_s16[i] = (1<<14);
                }
                
                // Update min and max :
                if ( buffer_diff_s16[i] > MAX_buffer_diff)
                {
                    MAX_buffer_diff = buffer_diff_s16[i];
                }
                else if ( buffer_diff_s16[i] < MIN_buffer_diff)
                {
                    MIN_buffer_diff = buffer_diff_s16[i];
                }
                
            }  
            ptr_Processing_tmp = ptr_Processing;   // re-init pointer after use (incremantations)
                        
            // scale difference to exploit full swing (tone mapping + contrast enhancement)
            uint32_t  MinMax_Swing = MAX_buffer_diff - MIN_buffer_diff;
            for (uint32_t i=0; i<PIC_SIZE; i++)
            {
                scaled_buffer_diff_u8[i] = ( (buffer_diff_s16[i] - MIN_buffer_diff) * 255 ) / MinMax_Swing;
            }  
                     

        #if (DISPLAY == ON_LCD)    
            // Note: if display is slow and it takes longer to display an image than capture next one, the frame rate will be broken    

            #define UPSCALE_FACTOR  1   
                            // !! BEWARE !! Max.3 for 80x80 IR sensor and 240-lines LCD ! (as 3x80=240)
                            // ** but also SPI must be fast enough to transfer full picture in short time
                            // ** Currently need to stick to UPSCALE_FACTOR = 1   (i.e. no upscale in fact...)
            gray8_to_RGB565_upscale(scaled_buffer_diff_u8, image_buffer_rgb565, PIC_WIDTH , PIC_HEIGHT, UPSCALE_FACTOR);
            GAPOC_LCD_pushPixels(&spim1, (lcdW-PIC_WIDTH)/2, (lcdH-PIC_HEIGHT)/2, UPSCALE_FACTOR*PIC_WIDTH, UPSCALE_FACTOR*PIC_HEIGHT, image_buffer_rgb565); 
 
       
        #elif (DISPLAY == ON_PC) 
            // Keep running after initial shutter for a few dummy pictures, then store on on IO file system and exit loop (terminate program)
            #define NUM_FRAMES_BEFORE_STORAGE   80
            if (dummy_frames_before_PC_store++ == NUM_FRAMES_BEFORE_STORAGE)
            {
                DBG_PRINT(".......  Please wait until you get Proxy off message ..........\n");
                save_pict_ppm( scaled_buffer_diff_u8 );
                break;
            }
        #endif
     
        }  // end if (!Calib_Done) ... else ...
         
                 
        // Wait for image capture finished
        // Check flag is not already set, else it'd mean processing took longer than image capture going on in parallel,
        // and therefore we're taking too much time to process vs. camera full frame rate
        if (Picture_Index ==0)
        {
            //DBG_PRINT("Processing too long; Camera full frame rate broken !!!\n");
        }
        while (Picture_Index ==0);  // wait for flag from callback signalling image capture finished
        Picture_Index =0;  
        
                       
        // Now swap ping and pong buffers
        uint16_t* tmp_Buff_Pointer = ptr_Processing;
        ptr_Processing = ptr_Receiving;
        ptr_Receiving = tmp_Buff_Pointer;
        

        // Relaunch picture capture
        cpiTransfer.data        = (unsigned char*)ptr_Receiving;  // store picture in buffer for reference picture
        CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);  
        
        
    }  // end while(1)
        

   
    //  ===   END PROCESSING   =================================================================================================           

    DBG_PRINT("\nDone, Switching off IR Proxy\n");
    

    // --  Switch off IR sensor   ------------------------------
    
    // Set integration time to zero
     GAPOC_IRProxy_WriteReg12( IRPROXY_TINT, 0x000);   
    
    // Power off analog supply and wait 100ms
    GAPOC_GPIO_Set_Low( GPIO_CIS_APWRON );              
    #define  ANA_TO_DIG_POWEROFF_LATENCY_ms 100
    wait((float)ANA_TO_DIG_POWEROFF_LATENCY_ms/1000.0);

    // Power off digital supply 
    GAPOC_GPIO_Set_Low( GPIO_CIS_DPWRON );      
        
    // Switch off master clock to IR sensor
    GAPOC_GPIO_Set_High( GPIO_CIS_CLK_NOTEN ); // ACTIVE LOW (=standby pin)

        
    // ----------------------------------------------------------
    
    GAPOC_GPIO_Set_Low(GPIO_CIS_TRIGGER);  // don't keep high GPIO driving powered off device

    BRIDGE_Disconnect(NULL);


    return (0);    
}


// #############################################################################
// ##### LOCAL FUNCTION DEFINITIONS  ###########################################


static void Callback_Got_Frame()   
{   
    Picture_Index++;    
}

// ----------------------------------------------------------------------
static void convert_14bpp_to_8bpp( unsigned char* pic_buffer_14bpp, unsigned char* pic_buffer_8bpp)
{
    for (uint32_t i=0; i<PIC_SIZE/2; i++)
    { 
        // Could do rounding ...not implemented here

        pic_buffer_8bpp[i] = ( (pic_buffer_14bpp[2*i] <<2 ) | (pic_buffer_14bpp[2*i+1] >>6)  );   // simple truncation from 14 to 8 bits
    }
}


// ----------------------------------------------------------------------
static void save_pict_ppm( unsigned char* pic_buffer_8bpp)
{
    static uint32_t imgNum = 0;
    static char imgName[50];    

    sprintf(imgName, "../../../img_OUT%d.ppm", (int)imgNum++);
    printf("\nimgName: %s\n", imgName);
    WriteImageToFile(imgName, PIC_WIDTH, PIC_HEIGHT, (pic_buffer_8bpp));
}
        
        

// ------------------------------------------------------------

/*

void GAPOC_IRProxy_SPI_Init(uint32_t spi_fqcy_khz)
{
        // SPI pins init, SPI udma channel init 
        spi_init(&spim1, SPI1_MOSI, NC, SPI1_SCLK, SPI1_CSN0_A5);
//        spi_init(&spim1, B3, SPI1_MISO, B4, SPI1_CSN0_A5);


        // SPI bits, cpha, cpol configuration 
        spi_format(&spim1, 16, 0, 0);  // 16-bit words, idle level =low
            // BEWARE - For some reason when doing 16-bit SPI transfers, SPI send MSB first in ech byte and LSByte first 
            
        // Set SPI fequency *
        spi_frequency(&spim1, spi_fqcy_khz*1000);

        DBG_PRINT("\nSPI1 initalized\n");
           
}


// -----------------------------------------------------------------


     
void GAPOC_IRProxy_WriteReg12(uint8_t RegAddr, uint16_t WriteVal)  // RegAddr to fit on 4 bits and WriteVal on 12 bits
{

#if GAPOC_DEBUG == 1
    if ( (RegAddr >>4) != 0x0)
    {
        DBG_PRINT("Reg. Address must fit on 4 bits !  -- Stopping Here\n");
        while(1);
    }
    if ( (WriteVal >>12) != 0x0)
    {
        DBG_PRINT("Reg. Value must fit on 12 bits !  -- Stopping Here\n");
        while(1);
    }
#endif

    spi_word16 =  (RegAddr <<12) | (WriteVal );  // Addr = 4 MSB, Value = 12 LSB

    // Ulis Proxy wants 16-bit with MSB first : A3.A2.A1.A0.D11.D10.......D1.D0
    // while (when doing 16-bit SPI transfers) SPI send MSB first in ech byte and LSByte first (strange!?)
    // So swap MSBbyte and LSBbyte
    spi_word16 =  (spi_word16 & 0xFF) <<8 | spi_word16 >>8 ; 
                
    spi_master_cs(&spim1, 0);            
    spi_master_write(&spim1, spi_word16);
        // NB - When doing 16-bit SPI transfers, it appears SPI send MSB first in ech byte and LSByte first ??!!??       
    spi_master_cs(&spim1, 1);    
}
 
*/     

// ## END OF FILE ##############################################################################


