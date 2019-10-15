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

// This code is an example of basic MT9V034 camera usage to demonstrate CPI.
// MT9V034 is configured in AUTOTEST mode (generates test pattern) with cropping to QVGA

// Pictures (test pattern) are stored on PC using transfers over the JTAG bridge.
// *** WARNING ***
// Currently the bridge to PC for .ppm file transfer is very slow -- be patient to store 1 pic
//  This should improve in the future

**/



// #############################################################################
// ##### DECLARATIONS  ########################################################

// ==  Handy "defines" for application, used locally   =====================

    
#define PIC_WIDTH   (((640/2)/4)*4)    
#define PIC_HEIGHT  (((480/2)/4)*4) 
    // Must be multiple of 4
     
#define PIC_SIZE    (PIC_WIDTH * PIC_HEIGHT)
#if PIC_SIZE>(128*1024)
  #error  "Picture > 128K limit (uDMA limitation in current SDK)"
#endif
    // !!! BEWARE !!!
    // The uDMA of GAP8 can transfer from the CPI interface chunks of data limited to 128KBytes.
    // Neither the current SDK nor the example code implement any work-around to cope with larger transfers at present.
    // This means target image size is limited to 12_K (OK for QVGA but not enough for VGA which represents ~363KB)


// ====  Includes    ==========================================================


#include "GAPOC_BSP_General.h"

#include "GAPOC_BSP_MT9V034.h"

#include "mbed_wait_api.h"



// ====  Application's Custom Types    ========================================
// <none>

    
// ==== Application's public global variables  ==============================


GAP_L2_DATA unsigned char image_buffer[PIC_SIZE];


// ==== Application's own global variables  ====================================

CPI_Type *const cpi_address[] = CPI_BASE_PTRS;

static volatile uint32_t Picture_Index = 0;  // to manage non-blocking picture transfer (snapshot)
 
// Misc. handles -- don't need to be seen elsewhere ?
static cpi_config_t cpi_config;
static cpi_transfer_t cpiTransfer;  
static cpi_handle_t hCPI;  

// for DBG
spi_t   spim1;      

// ==== Application's Function Prototypes    ===================================
    
// Local helper functions
static void Callback_Single_Shot();   // MAy need a __WEAK function declaration somewhere
static void save_pict_ppm( unsigned char* pic_buffer);


// #############################################################################
// ##### MAIN APPLICATION  ###########################################

int main()
{


    DBG_PRINT("\nGAPOC basic CPI test\n");

    // This is to be able to use debug bridge to sace .ppm picture on host PC
    BRIDGE_Init();
    printf("Connecting to bridge\n");
    BRIDGE_Connect(0, NULL);
    printf("Connection done\n\n");

    
    
    //  === INITIALIZATIONS     ==========================================================================================
    
    
    //  ---   Initalize Board (GPIO direction and default level, supplies, etc.) -----------------------------------------
        
    GAPOC_BSP_Board_Init();
    
    
    // !! If enabling HyperBus interface, UART Rx no more functional
    // (this init redundant? already done in GAPOC_BSP_Board_Init() ?)
    hyperbus_t hyperbus0;
    hyperbus_init(&hyperbus0, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);


    // -- Init I2C1 for CIS  --------------------------------------  
    
    GAPOC_MT9V034_I2C1_Init(CIS_I2C1_FREQ_KHz * 1000);



    // -- Init CPI i/f        ------------------------------------  
    
    CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7); 
        // Initiliazes CPI I/Os *and* inits CPI channel of uDMA through UDMA_Init()


    // -- Configure CPI i/f       -----------------------------------  
    
    CPI_GetDefaultConfig(&cpi_config);
    cpi_config.row_len = PIC_WIDTH ; 
            // !! Keep equal to target width
            // Can normally be a portion of window width in slice mode 
            // but this is buggy on GAP8 Cut1
    cpi_config.resolution      = PIC_SIZE;
    cpi_config.format          = BYPASS_BIGEND;  // Monochrome
    cpi_config.shift           = 0;
    cpi_config.slice_en        = 0;
    cpi_config.frameDrop_en    = 0;
    cpi_config.frameDrop_value = 0;
    cpi_config.wordWidth       = 16;  // 8, 16 or 32bits // IN FACT IGNORED in CPI DRIVER [gap_cpi.c] !!!!
        // Those settings will be used by CPI_Enable
 
   
    // Set-up uDMA for getting data from CPI:

    cpiTransfer.data        = image_buffer;
    cpiTransfer.dataSize    = PIC_SIZE;  // !! MUST BE <128K
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1); //0 -> 8bit //1 -> 16bit //2 -> 32bit -- ALWAYS USE 16-bits for CPI uDMA (?)
    DBG_PRINT("CPI uDMA configured\n");  
        // Will be used by CPI Reception function (blocking or non-blocking)
              
    // -- Start MT9V034 chip :              ----------------------------------------------------------------------

    GAPOC_GPIO_Set_Low(GPIO_CIS_EXP);      // Make sure trigger input is low (for snapshot mode)
    
    // Enable 3V3A/3V3D 
    GAPOC_GPIO_Set_High( GPIO_CIS_PWRON );

    #define SUPPLY_SETTLING_TIME_ms 10
    wait ((float)SUPPLY_SETTLING_TIME_ms/1000.0);


    // Wait for chip ready
    uint16_t ChipId_Reg;
    do 
    {
        ChipId_Reg = GAPOC_MT9V034_ReadReg16(MT9V034_CHIP_ID_REG);
    } while ( ChipId_Reg == 0xFFFF );  // Getting 0xFFFF when I2C not yet ready

    
    // Check ChipID is correct    
    if (ChipId_Reg != MT9V034_CHIP_ID)
    {
        DBG_PRINT("Error - Unexpected I2C device address from CIS\n");
        return -1;     
    }
    else 
    {
        DBG_PRINT("MT9V034_ID_VALUE : %X\n", ChipId_Reg ); 
    }


    // Make sure camera won't start outputting valid pixels yet: go to snapshot mode (trigger input asssumed low)
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_OFF| MT9V034_CHIP_CONTROL_SIMULTANEOUS);     

    // Reset chip (except I2C config) to abort any capture started in default master mode before config change (helps save power)
    GAPOC_MT9V034_WriteReg16(MT9V034_RESET, MT9V034_SOFT_RESET);     

         
    // Set image viewport = cropped area off full frame, centered
    uint16_t Xstart = (MT9V034_WINDOW_WIDTH_MAX -PIC_WIDTH) /2;
    uint16_t Ystart = (MT9V034_WINDOW_HEIGHT_MAX -PIC_HEIGHT) /2;     
    GAPOC_MT9V034_WriteReg16(MT9V034_COLUMN_START_A, Xstart+1);     // first column index is 1    
    GAPOC_MT9V034_WriteReg16(MT9V034_WINDOW_WIDTH_A, PIC_WIDTH);                               
    GAPOC_MT9V034_WriteReg16(MT9V034_ROW_START_A, Ystart+4);         // CAUTION: First 4 rows are dark rows, not to be read out // !!! A VOIR ////
    GAPOC_MT9V034_WriteReg16(MT9V034_WINDOW_HEIGHT_A, PIC_HEIGHT); 


    // Program Horizontal and Vertical Blanking accordingly:
    // Keep Frame Start Blanking and Frame End Blanking to their default values
    // Short row timing is not possible (see Table 5 Note 1 of DS rev D),
    // Minimum total row time is 704pclk (horizontal width +blanking)
    #define TOTAL_ROW_TIME 704 // was 846  // = max line width + default HBI
    GAPOC_MT9V034_WriteReg16( MT9V034_HORIZONTAL_BLANKING_A, TOTAL_ROW_TIME -PIC_WIDTH );

    
    // Put MT9V034 in autotest mode 
    GAPOC_MT9V034_WriteReg16(MT9V034_TEST_PATTERN, MT9V034_TEST_PATTERN_ENABLE|MT9V034_TEST_PATTERN_GREY_VERTICAL); // use grey shade test pattern
    /*
          // NB - would do this to also support custom defined grey fill:
          GAPOC_MT9V034_WriteReg16(MT9V034_TEST_PATTERN, 
            ((MT9V034_TEST_PATTERN_DATA_MASK & 0x000)<<MT9V034_TEST_PATTERN_DATA_SHIFT)
            | MT9V034_TEST_PATTERN_USE_DATA
            | MT9V034_TEST_PATTERN_ENABLE );  // Replace 10-bit out from ADC by (e.g. 0x2AA, leading to 0xAA when trucnated to 8 bits) 
    */
  
    DBG_PRINT("MT9V034 Ready and Configured\n");  


    // Enable CPI
    // (CPI be continuously active -- alternatively, we could
    // disable it after picture taken, enable back prior to shooting next picture etc... To have more clock gating and save power
    CPI_Enable(cpi_address[0], &cpi_config); // Activate configuration of CPI channel -- Starts gated clock of CPI; needed if disabled in callback (to save power)


    // 1) Continuous master mode with blocking reception
      
    // Start Camera in Master mode:
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_MASTER_MODE | MT9V034_CHIP_CONTROL_DVP_ON| MT9V034_CHIP_CONTROL_SIMULTANEOUS);     

    
    // Note: MT9V034 has re-started new frame and we haven't enabled CPI Reception yet (done just below);
    // !! BEWARE !! CPI doesn't look for rising edge on FRAME_VALID to decide start of frame, instead it just relies on its first observation of
    // FRAME_VALID high and LINE_VALID high. As a result in this configuration we must enable CPI Reception immediately otherwise we'll miss pixels and
    // end up with a shifted picture.


    // Get Picture
    CPI_ReceptionBlocking(cpi_address[0], &cpiTransfer);  
    
    // Stop master mode, disable DVP and abort any new capture
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_OFF| MT9V034_CHIP_CONTROL_SIMULTANEOUS);         
    GAPOC_MT9V034_WriteReg16(MT9V034_RESET, MT9V034_SOFT_RESET);    
        
    // Save Picture
    save_pict_ppm( image_buffer );  // ppm pict #0
    
    
    DBG_PRINT("\nPicture Taken in Blocking Reception Mode\nNow use non-blocking reception\n\n");  
    
    
        
    // 2) Snapshot mode with non-blocking reception


    // Prepare handles for non-blocking camera capture
    CPI_ReceptionCreateHandle(cpi_address[0], &hCPI, Callback_Single_Shot, NULL);  

    // Enable back DVP
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_ON| MT9V034_CHIP_CONTROL_SIMULTANEOUS);         
    
    
            
    //  ===   MAIN LOOP    =================================================================================================       

    #define NUM_PICTURES    2
    for (uint8_t i=0; i<NUM_PICTURES; i++)
    {
        
        // -- Enable capture by camera (non-blocking)
        CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);  


        // -- GPIO_CIS_EXP snapshot trigger signal to actually shoot picture !
        GAPOC_GPIO_Set_High(GPIO_CIS_EXP);   // generate rising edge to trigger snapshot
            //To be set back low by Callback function
    
        while (Picture_Index ==0); // wait for flag from callback signalling image capture finished

        Picture_Index =0;


        // -- Save image_buffer as .ppm Picture (usable for display on PC)        -----------------------------------------------------    
        save_pict_ppm( image_buffer );
    
    }       

    DBG_PRINT("\nDone, disconnecting bridge\n");
    BRIDGE_Disconnect(NULL);


    return 0;    
}


// #############################################################################
// ##### LOCAL FUNCTION DEFINITIONS  ###########################################


static void Callback_Single_Shot()   // MAy need a __WEAK attribute somewhere
{
CPI_Type *const cpi_address[] = CPI_BASE_PTRS;


//    CPI_Disable(cpi_address[0]);  // Stop gated clocks
    
    //also need this ???
    // CPI_Deinit(cpi_address[0]);   //disables CPI (no more!) AND deinit uDMA for CPI
        
    GAPOC_GPIO_Set_Low(GPIO_CIS_EXP);
    Picture_Index++;    
}

// ----------------------------------------------------------------------
static void save_pict_ppm( unsigned char* pic_buffer)
{
    static uint32_t imgNum = 0;
    static char imgName[50];    
    
        sprintf(imgName, "../../../img_OUT%d.ppm", (int)imgNum++);
        printf("\nimgName: %s\n", imgName);
        WriteImageToFile(imgName, PIC_WIDTH, PIC_HEIGHT, (pic_buffer));
}
        
        
        



// ## END OF FILE ##############################################################################


