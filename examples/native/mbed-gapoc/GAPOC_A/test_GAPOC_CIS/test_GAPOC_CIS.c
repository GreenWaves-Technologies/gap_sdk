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

// This code is an example of MT9V034 camera usage in single shot mode with lo average power objectives.
// It starts (at cold boot) by auto-calibrating the camera (Black level, Gain, Exposure)
// which requires to shoot a series of pictures of the scene
// Then power to the camera is switched off and back on again only when a picture needs to be taken.
// A single snapshot is taken periodically. This involves powering up the 
// MT9V034 Camera Image Sensor (CIS), reloading configuration parameters (including previously
// obtained gain, exposure and black level settings), taking a dummy picture to validate those
// settings (required by MT9V034), taking the actual picture and switching back off the CIS.
// Then the image captured can be processed by some algorithm.
// Between these bursts of activity GAP8 should be put in sleep mode.
// TODO: sleep mode is not implemented yet.

// NOTE: This example assumes that scene brightness reamins fairly constant so it's OK
// to compute required Exposure/Gain at power up and not change it. In some real-life
// scenarios it may be required to periodically re-adjust gain/exposure


// In this example, the captured image can either be dispalyed on an Adafruit 2.9" LCD Display
// or be transfered through the JTAG bridge to a host PC in .ppm format for exploitation on the PC.

// Informative messages can be turned on or off by appropriate setting of symbol GAPOC_DEBUG in GAPOC_BSP_Platform_Defs.h
// They should be off if trying to save power as, when present, they extend the time during which the camera is powered.


**/



// #############################################################################
// ##### DECLARATIONS  ########################################################

// ==  Handy "defines" for application, used locally   =====================

#define ON_LCD  0
#define ON_PC   1    

/** ****************************
// Select below if you want the pictures to be displayed on an Adafruit LCD attached to GAPOC
// (select ON_LCD) or to be saved as .ppm on PC (select ON_PC) -- in this case you must have JTAG connected 
//
// *** WARNING ***
// Currently the bridge to PC for .ppm file transfer is very slow -- be patient to store 1 pic
//  This should improve in the future
** ****************************/
// ============================

#define DISPLAY     ON_PC    

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
    
#define ROW_FLIP        true   
    // option to apply a symmetry wrt central horizontal axis (= Top-Bottom Mirroring)
#define COLUMN_FLIP     false   
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

#include "Identify_Image_ROI.h"


// ====  Application's Custom Types    ========================================
// <none>

    
// ==== Application's public global variables  ==============================


  GAP_L2_DATA  uint8_t image_buffer[PIC_TARGET_SIZE];
#if (DISPLAY == ON_LCD)
  GAP_L2_DATA  uint16_t image_buffer_rgb565[PIC_TARGET_SIZE];
#endif




// ==== Application's own global variables  ====================================

static volatile uint32_t Picture_Index = 0;  // to manage non-blocking picture transfer (snapshot)


// This needs to SURVIVE DEEP SLEEP  -- OK in L2 if retention mode used, else need to save in NVM 
GAP_L2_DATA static uint16_t  AGC_Latest_Value, AEC_Latest_Value;    // Latest auto-calibrated Gain and Exposure values -- obtained thru GAPOC_MT9V034_Gain_Exposure_Update
GAP_L2_DATA static uint16_t  Black_Latest_Value;                     // Latest auto-calibrated Black Level value -- obtained thru GAPOC_MT9V034_BlackLevelCalib_Blocking()
GAP_L2_DATA static  GAPOC_MT9V034_Cfg_t    GAPOC_MT9V034_Cfg;
 

#if (DISPLAY == ON_PC)
  static char imgName[50];
  static uint32_t imgNum = 0;
#endif


// ==== Application's Function Prototypes    ===================================

ROI_Struct_t    Identify_Image_ROI( uint8_t* image_buffer, uint32_t Picture_Width, uint32_t Picture_Height  );
    
// Local helper functions
static void Callback_Single_Shot();   // MAy need a __WEAK function declaration somewhere



// #############################################################################
// ##### MAIN APPLICATION  ###########################################

int main()
{

    DBG_PRINT("\nGAPOC CIS test under mbed\n");

#if (DISPLAY == ON_PC)  
    DBG_PRINT("(see #define DISPLAY =>) Outputting pictures as .ppm files\n");
    
    BRIDGE_Init();
    printf("Connecting to bridge\n");
    
    BRIDGE_Connect(0, NULL);
    printf("Connection done\n\n");
#else     // LCD
    DBG_PRINT("(see #define DISPLAY =>) Displaying pictures on LCD\n\n");
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


    //  ---   Configure Wake-Up from Sleep if needed  and pin behaviours in sleep modes  ---------------------------------       
    //TODO -- see PMU Tests
    // Beware: with Cut1.0 can't use deep sleep AND pad sleep enable -- also retentive sleep works only if booting from Flash
    // !!! FIXME: with current SDK, it appears that configuring the HyperBus (required to boot from Flash)
    //  breaks the non-blocking transfers functionality ?!!?  ...so sleep modes not usable for now in this test
    
    
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
     

    // Apply configuration  (cold boot or not -- registers did not survive low power) >>
    // Set-up GAP8 CPI interface:
    GAPOC_MT9V034_CPI_Setup(&GAPOC_MT9V034_Cfg);
    
    // Set-up uDMA for getting data from CPI:
    GAPOC_MT9V034_uDMA_Config( image_buffer, GAPOC_MT9V034_Cfg.TargetWidth*GAPOC_MT9V034_Cfg.TargetHeight );


     
    //  === COLD BOOT ONLY INITIALIZATIONS ==================================================================================
    
    //  Run cold boot calibrations of  MT9V034 Camera Image Sensor     
           
    // TODO - MAy also want to relaunch a black level calibration every xx mn in case of lighting coditions change ?  (or couple to an ambient ligth sensor?)
        
    if(PMU_WakeupState() == uPMU_COLD_BOOT) // TODO - or if time (fromRTC) since last wake-up > Threshold
    {

         //  ---   Cold boot MT9V034 Initializations    ------------------------------------------------------------------- 
              
        // Start MT9V034 chip :
        if ( GAPOC_MT9V034_Start(&GAPOC_MT9V034_Cfg) != 0 )
        {
            DBG_PRINT("Error - didn't start\n");
            return (-1);
        }

        // Even if HDR was selected, perform iniital calibration with HDR mode off at CIS level
        GAPOC_MT9V034_WriteReg16( MT9V034_PIXEL_OPERATION_MODE, 0x0);  
            
        // Run initial calibration :
        GAPOC_MT9V034_BlackLevelCalib_Blocking(&AGC_Latest_Value, &AEC_Latest_Value, &Black_Latest_Value, &GAPOC_MT9V034_Cfg );
                // Is beneficial, TBD min # of frames to use (today = 10 = default)


        // Now switch off CIS till snapshots required
        GAPOC_MT9V034_Off();     
        DBG_PRINT("CIS now OFF\n");              
                        
    }


    //  ===   MAIN LOOP    =================================================================================================       


// This while(1) should disappear when we're going to sleep after picture snapshot with wake-up from RTC enabled
while(1)
{
        
    // Set-up GAP8 CPI interface [CPI registers did not survive low-power mode]
    GAPOC_MT9V034_CPI_Setup(&GAPOC_MT9V034_Cfg);
        //redundant if going to deep sleep with mandatory reboot at end of while "loop" (executed only once in that case!) -- already done in prologue before while()

    // Set-up uDMA for getting data from CPI  [uDMA registers did not survive low-power mode]
        //redundant if going to deep sleep with mandatory reboot at end of while "loop" (loop executed only once in that case!)   
    GAPOC_MT9V034_uDMA_Config( image_buffer, GAPOC_MT9V034_Cfg.TargetWidth*GAPOC_MT9V034_Cfg.TargetHeight );

    // (Re-)Start MT9V034 chip   
    DBG_PRINT("\nCIS restarting\n");              
    if ( GAPOC_MT9V034_Start(&GAPOC_MT9V034_Cfg) != 0 )
    {
        DBG_PRINT("Error - didn't start\n");
        return (-1);
    }



    // ++++ TRY THIS  ++++++++++++++++++++++
    // In HDR mode, option to increase exposure time ot get good low light details, given than brigth areas will be compressed
    #define AEC_MULT_FACTOR_IN_HDR  1   // no effect if set to 1
    #define AGC_MULT_FACTOR_IN_HDR  1   // no effect if set to 1  
    #define MAX_AEC_HDR             480
    #define MAX_AGC_HDR             64
    
    
    if (GAPOC_MT9V034_Cfg.En_HDR)
    {       // TODO - move this inside Apply_Calibrations()

        uint16_t  AEC_Latest_Value_HDR = AEC_Latest_Value * AEC_MULT_FACTOR_IN_HDR; 
        uint16_t  AGC_Latest_Value_HDR = AGC_Latest_Value * AGC_MULT_FACTOR_IN_HDR; 
        if (AEC_Latest_Value_HDR >MAX_AEC_HDR)
        {
            AEC_Latest_Value_HDR = MAX_AEC_HDR;
        }           
        if (AGC_Latest_Value_HDR >MAX_AGC_HDR)
        {
            AGC_Latest_Value_HDR = MAX_AGC_HDR;
        }     
        GAPOC_MT9V034_Apply_Calibrations( AGC_Latest_Value_HDR, AEC_Latest_Value_HDR, Black_Latest_Value, &GAPOC_MT9V034_Cfg );    
                              
        // Compensate fact that in HDR digital range is typically larger than ADC input range
        // Several methods, see DS and AN  -- Try this one:
        // GAPOC_MT9V034_WriteReg16( MT9V034_VREF_ADC, MT9V034_VREF_2V1);                    
        GAPOC_MT9V034_WriteReg16( MT9V034_ANALOG_GAIN_A, AGC_Latest_Value_HDR|MT9V034_ANALOG_GAIN_GLOBAL );                    

    }
    else
    // ++++ +++++++++++++++++++++
    
    // Recall previous results of Black Level, AGC, AEC 
    GAPOC_MT9V034_Apply_Calibrations( AGC_Latest_Value, AEC_Latest_Value, Black_Latest_Value, &GAPOC_MT9V034_Cfg );
        // would it make sense to keep AGC and AEC and only force black level ? always or from time to time...
        // Anyway otherwise: how to keep track of evolution of lighting conditions/scene illumination
        // IF AEC and AGC are computed during frame N and applied udring N+1, better do this -- still 2 exposures needed anyway !


    // Shoot picture !    
    GAPOC_MT9V034_Single_Shot_NonBlock( Callback_Single_Shot );

     // NB: Above fn sets GPIO_CIS_EXP high and enables CPI interface
     //  Bringing GPIO_CIS_EXP back low and de-initing CPI is taken care of in callback function
    
    do
    {
    //TODO - Go to sleep until callback IT arrives ?
    } while (Picture_Index ==0);
    // or (simpler) just to a blocking Single Shot (consumption delta due to core << CIS consumption, anyway) ??
    Picture_Index--;

    /*
    DBG_PRINT("\nAfter useful picture taken\n");   
    DBG_PRINT("AEC Reg. = %d\n",(int)GAPOC_MT9V034_ReadReg16(MT9V034_AEC_OUTPUT));
    DBG_PRINT("Exposure Stored = %d\n",(int)AEC_Latest_Value);
    DBG_PRINT("AGC Reg= %d\n",(int)GAPOC_MT9V034_ReadReg16(MT9V034_AGC_OUTPUT));
    DBG_PRINT("Gain Stored = %d\n",(int)AGC_Latest_Value);
    DBG_PRINT("Black Reg= %d\n",  (int)((int8_t)(0x00FF&GAPOC_MT9V034_ReadReg16(MT9V034_BLACK_LEVEL_VALUE_A))) );
    DBG_PRINT("Black Stored = %d\n",(int)(int8_t)(0x00FF&Black_Latest_Value));   
    */


    // Now picture has been shot and stored in memory by uDMA :  power off CIS ASAP (CPI i/f already stopped in callback ISR)
    GAPOC_MT9V034_Off();
    DBG_PRINT("CIS now OFF\n");              

   
   
    // --- Launch Processing  (dummmy in this example)  ---------------------------------------------------------    
    // - Pass pointer to frame buffer + picture size
    // - Get back # of ROI and ROI coordinates (X-Y top left + X-Y bottom right)
    
    ROI_Struct_t ROI_Struct = Identify_Image_ROI( image_buffer, GAPOC_MT9V034_Cfg.TargetWidth, GAPOC_MT9V034_Cfg.TargetHeight );
    // This is just an example / placeholder
    // Here we get back a dummy ROI -- and do nothing with it...
    
    
    // -- Display Picture      -----------------------------------------------------------------------------------    


#if (DISPLAY == ON_LCD)   

    gray8_to_RGB565(image_buffer, image_buffer_rgb565, GAPOC_MT9V034_Cfg.TargetWidth , GAPOC_MT9V034_Cfg.TargetHeight);

    GAPOC_LCD_pushPixels(&spim, 0, 0, GAPOC_MT9V034_Cfg.TargetWidth, GAPOC_MT9V034_Cfg.TargetHeight, image_buffer_rgb565);

    #define TIME_BETWEEN_PICS_ms  500
    wait((float)TIME_BETWEEN_PICS_ms/1000);
    
#else  // Display  on PC 

        sprintf(imgName, "../../../img_OUT%d.ppm", (int)imgNum++);
        printf("imgName: %s\n", imgName);
        WriteImageToFile(imgName, GAPOC_MT9V034_Cfg.TargetWidth, GAPOC_MT9V034_Cfg.TargetHeight, (image_buffer));
#endif




/*    
    // TODO: go to [retentive] deep sleep 
    // -- will start again at beginning of program if RTC elapses or selected GPIOIRQ fires
    // (but may need to enforce latency if PIR-triggered )   
    PMU_StateSwitch(uPMU_SWITCH_RETENTIVE_SLEEP, uPMU_SWITCH_FAST);

*/        

    
}  // end of temporary while(1)     

#if (DISPLAY == ON_PC)   
    BRIDGE_Disconnect(NULL);
#endif

    return 0;    
}


// #############################################################################
// ##### LOCAL FUNCTION DEFINITIONS  ###########################################


static void Callback_Single_Shot()   // MAy need a __WEAK attribute somewhere
// As a minimum, in Callback after a single shot non-blocking call :
// - disable CPI (to stop gated clocks)
// - reset CIS_EXP signal
{
CPI_Type *const cpi_address[] = CPI_BASE_PTRS;


    CPI_Disable(cpi_address[0]);
    
    //also need this ???
    CPI_Deinit(cpi_address[0]);   //disables CPI (no more!) AND deinit uDMA for CPI
        
    GAPOC_GPIO_Set_Low(GPIO_CIS_EXP);
    Picture_Index++;    
}



// ## END OF FILE ##############################################################################


