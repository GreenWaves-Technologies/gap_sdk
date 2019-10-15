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

/**

// This code is an example of MT9V034 camera usage in video mode
// (which may be interesting for demos but implies high power consumption,
// the camera being always on
//
// Informative messages can be turned on or off by appropriate setting of symbol GAPOC_DEBUG in GAPOC_BSP_Platform_Defs.h
//
// Images can optionally be displayed on an Adafruit LCD display

**/



// #############################################################################
// ##### DECLARATIONS  ########################################################

// ==  Handy "defines" for application, used locally   =====================

#define NO_DISPLAY  0
#define ON_LCD  1

/** ****************************
//
// Select below if you want the pictures to be displayed on an Adafruit LCD attached to GAPOC
// (select ON_LCD) or not displayed, just saved to memory 
//
// Note: speed of GAPOC<>LCD interface may be adjusted through GAPOC_LCD_SPI_FQCY_MHz in ili9341.h if wished
//
** ****************************/
// ============================

#define DISPLAY     ON_LCD

// ============================

    
#define PIC_TARGET_WIDTH    (((640/2)/4)*4)    
    // Width of picture you want to obtain, max is 752 (WVGA)
    
#define PIC_TARGET_HEIGHT   (((480/2)/4)*4) 
    // Height of picture you want to obtain, max is 480 (WVGA)
     
#define RESCALE_CROPPING_ONLY  false 
    // If true, the picture will be taken by cropping a centered window of target size from the full WVGA frame
    //    This means effective Field of View (FOV) is reduced if target picture size if less than WVGA
    // If false, binning is allowed. The code will try to keep maximum FOV by taking largest possible picture    
    //    from full WVGA frame and apply greatest possible binning in X and Y directions (either none, x2 or x4)
    
#define EN_AUTOTEST     false
    // If true, the CIS image sensor will produce a test pattern rather than a snapshot of the scene

#define EN_HDR          false
    // If true, High Dynamic Range operation is enabled
    // !!! HDR Mode Settings may need some tuning (also according to expected scene characteristics)
    
#define ROW_FLIP        false   
    // option to apply a symmetry wrt central horizontal axis (= Top-Bottom Mirroring)
#define COLUMN_FLIP     true   
    // option to apply a symmetry wrt central vertical axis (= Left-Right Mirroring)   
    
    
#define PIC_TARGET_SIZE    (PIC_TARGET_WIDTH * PIC_TARGET_HEIGHT)
#if PIC_TARGET_SIZE>(128*1024)
  #error  "Picture > 128K limit"
#endif
    // !!! BEWARE !!!
    // The uDMA of GAP8 can transfer from the CPI interface chunks of data limited to 128KBytes.
    // Neither the current SDK nor the example code implement any work-around to cope with larger transfers at present.
    // This means target image size is limited to 12_K (OK for QVGA but not enough for VGA which represents ~363KB)


// ====  Includes    ==========================================================


#include "GAPOC_BSP_General.h"

#include "GAPOC_BSP_MT9V034.h"
#include "GAPOC_BSP_ADAFRUIT_LCD.h"   

#include "mbed_wait_api.h"




// ====  Application's Custom Types    ========================================
// <none>

    
// ==== Application's public global variables  ==============================


  GAP_L2_DATA  uint8_t image_buffer[PIC_TARGET_SIZE];
#if (DISPLAY == ON_LCD)
  GAP_L2_DATA  uint16_t image_buffer_rgb565[PIC_TARGET_SIZE];
#endif


// ==== Application's own global variables  ====================================

static volatile uint32_t Picture_Index = 0;  // to manage non-blocking picture transfer (snapshot)

GAP_L2_DATA static  GAPOC_MT9V034_Cfg_t    GAPOC_MT9V034_Cfg;
 


// ==== Externally declared variables       ====================================
 
extern cpi_config_t cpi_config;         // declared and used in GAPOC_BSP_MT9V034.c
extern cpi_transfer_t cpiTransfer;      // declared and used in GAPOC_BSP_MT9V034.c  
extern cpi_handle_t hCPI;               // declared and used in GAPOC_BSP_MT9V034.c 



// ==== Application's Function Prototypes    ===================================

// Local helper functions
static void Callback_Video_Frame();   



// #############################################################################
// ##### MAIN APPLICATION  ###########################################

int main()
{

CPI_Type *const cpi_address[] = CPI_BASE_PTRS;


    DBG_PRINT("\nGAPOC CIS test in Video under mbed\n");

#if (DISPLAY == ON_LCD)  
    DBG_PRINT("(see #define DISPLAY =>) Displaying pictures on LCD\n\n");
#else     // LCD
    DBG_PRINT("(see #define DISPLAY =>) Acquired pictures won't be displayed\n\n");
#endif

    
    
    //  === INITIALIZATIONS     ==========================================================================================
    
    

    if (EN_HDR)
    {
        DBG_PRINT("...Using HDR mode\n");
    }

    
    //  ---   Initalize Board (GPIO direction and default level, supplies, etc.) -----------------------------------------
        
    GAPOC_BSP_Board_Init();
    

#if (DISPLAY == ON_LCD)
    spi_t   spim ;   
    GAPOC_LCD_Init(&spim);   // configures SPI1 for LCD & displays welcome message
#endif


    //  ---   Configure CIS (MT9V034) and related GAP8 interfaces            -----------------------------------------------  
         
    //  Initalize structure containing user's requiremnts for CIS  (see #defines above)    
    GAPOC_MT9V034_Cfg.TargetWidth = PIC_TARGET_WIDTH;   // (PIC_TARGET_WIDTH/4)*4;  // to force a multiple of 4
    GAPOC_MT9V034_Cfg.TargetHeight = PIC_TARGET_HEIGHT; // (PIC_TARGET_HEIGHT/4)*4;    
    GAPOC_MT9V034_Cfg.Rescale_CroppingOnly = RESCALE_CROPPING_ONLY;   // For small pic sizes, no binning gives more cropping => smaller FOV
    GAPOC_MT9V034_Cfg.En_HDR = EN_HDR;   
    GAPOC_MT9V034_Cfg.Row_Flip = ROW_FLIP;     
    GAPOC_MT9V034_Cfg.Column_Flip = COLUMN_FLIP;     
    GAPOC_MT9V034_Cfg.En_Autotest = EN_AUTOTEST;    
    GAPOC_MT9V034_Cfg.En_Row_Noise_Cancel =false;
    GAPOC_MT9V034_Cfg.En_Exposure_LED =true;
    GAPOC_MT9V034_Cfg.En_ADC_NonLinear_Companding =false;
     // NB: if warm boot after sleep with retention, the above could be skipped as Cfg variables survides in L2
     

    // Initialize Camera Interface (CPI+I2C)
    GAPOC_MT9V034_CPI_Setup(&GAPOC_MT9V034_Cfg);
    
    // Set-up uDMA for getting data from CPI
    GAPOC_MT9V034_uDMA_Config( image_buffer, GAPOC_MT9V034_Cfg.TargetWidth*GAPOC_MT9V034_Cfg.TargetHeight );


               
    // Start MT9V034 chip (in external trigger mode so doesn't provide output pixels for now)
    if ( GAPOC_MT9V034_Start(&GAPOC_MT9V034_Cfg) != 0 ) 
    {
        DBG_PRINT("Error - didn't start\n");
        return (-1);
    }

            
    // Enable full automatic exposure control mode (AEC, AGC, Black level) :
    GAPOC_MT9V034_Enable_Full_Auto(&GAPOC_MT9V034_Cfg );
        // Note: Target brightness may be changed by a configuration register Reg.0xa5 (MT9V034_AEC_AGC_DESIRED_BIN)
        // initialized in GAPOC_MT9V034_Start()
        

    DBG_PRINT("Initializations done\n");              



    //  ===   MAIN LOOP    =================================================================================================       


   // Enable CPI
    CPI_Enable(cpi_address[0], &cpi_config);  // Activate configuration of CPI channel -- Starts gated clock of CPI
    DBG_PRINT("CPI Channel Enabled\n"); 
    
      
    // Prepare handle for non-blocking camera capture
    CPI_ReceptionCreateHandle(cpi_address[0], &hCPI, Callback_Video_Frame, NULL);  
    DBG_PRINT("CPI Handle created\n");
    
    // Enable capture of camera data by CPI (non-blocking)
    CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);  
                   
    // -- Switch camera to suitable mode (GPIO_CIS_EXP assumed low, normally already done!]
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_ON | MT9V034_CHIP_CONTROL_SIMULTANEOUS);            

    // -- First GPIO_CIS_EXP snapshot trigger signal 
    GAPOC_GPIO_Set_High(GPIO_CIS_EXP);   // generate rising edge to trigger snapshot, To be set back low by Callback function


   while(1)
    {
        while (Picture_Index ==0);  // wait for picture received

        // ...Now picture has been received...
        
        Picture_Index--;
        
        // Re-enable a non-blocking capture 
        CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);   // Re-enable a non-blocking capture       

        // Generate new rising edge to trigger snapshot (set back low by Callback function)        
        GAPOC_GPIO_Set_High(GPIO_CIS_EXP);   

#if (DISPLAY == ON_LCD)   
        
        // Meanwhile, Display Picture if needed  
        // Note: if display is slow and it takes longer to display an image than capture next one, the frame rate will be impacted    

        gray8_to_RGB565(image_buffer, image_buffer_rgb565, GAPOC_MT9V034_Cfg.TargetWidth , GAPOC_MT9V034_Cfg.TargetHeight);

        GAPOC_LCD_pushPixels(&spim, 0, 0, GAPOC_MT9V034_Cfg.TargetWidth, GAPOC_MT9V034_Cfg.TargetHeight, image_buffer_rgb565);
            // this is a blocking function for now...
#else  
        // No Display, nothing to do...
#endif

/*
        #define TIME_BETWEEN_PICS_ms  500
        wait((float)TIME_BETWEEN_PICS_ms/1000);
*/


    
    }  // end while(1)     


    return 0;    
}


// #############################################################################
// ##### LOCAL FUNCTION DEFINITIONS  ###########################################


static void Callback_Video_Frame()   
{
CPI_Type *const cpi_address[] = CPI_BASE_PTRS;

    GAPOC_GPIO_Set_Low(GPIO_CIS_EXP);  // de-assert trigger signal to CIS
    Picture_Index++;    
}



// ## END OF FILE ##############################################################################


