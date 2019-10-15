// File: GAPOC_BSP_MT9V034.c

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
// ##### DECLARATIONS  #########################################################

// ====  Handy "defines" for application, used locally   =======================

// <none>


// ====  Includes    ===========================================================

#include "GAPOC_BSP_MT9V034.h"

#include "mbed_wait_api.h"

 
// ====  Imported Global Variables  ============================================


// ====  Exported Global Variables  ============================================

cpi_config_t cpi_config;
cpi_transfer_t cpiTransfer;  
cpi_handle_t hCPI;  


// ====  File-wide Variables  ===================================================

// data accessed by I2C trhu uDMA needs to be in L2 (?)
static GAP_L2_DATA uint8_t MTV9V034_ReadVal16[2];  // 2-byte array
static GAP_L2_DATA uint8_t I2C1_WrData16[3];  // to store target register 8-bit address + 16-bit data
static GAP_L2_DATA uint8_t I2C1_RegAddr;

// Misc. handles -- Moved to "exported" section
/*
static cpi_config_t cpi_config;
static cpi_transfer_t cpiTransfer;  
static cpi_handle_t hCPI;  
*/

i2c_t i2c1; 

static volatile bool Got_New_BlackLvl_Frame;


// #############################################################################
// ##### FUNCTION DEFINITIONS        ###########################################


/** Function : GAPOC_MT9V034_ReadReg16 
    Action : Using I2C1, Reads and returns the contents of a 16-bit register of the MT9V034 specified by its 8-bit address RegAddr
*/

uint16_t GAPOC_MT9V034_ReadReg16(uint8_t RegAddr)
{
// !!! Arguments of I2C functions have to be in L2 for access by uDMA (true?)
    
    I2C1_RegAddr = RegAddr;  // in L2 for uDMA access
    
    // Send Register Address :
    i2c_write(&i2c1, MT9V034_DEV_ADDR_WR, (const char*)&I2C1_RegAddr, 1, 0);  // data write length = 1 byte (register address), no stop bit
    
    // Get 16-bit rgister contents:
    i2c_read(&i2c1, MT9V034_DEV_ADDR_RD, (char*)(MTV9V034_ReadVal16), 2, 1); // MTV9V034_ReadVal16 is declared in L2  (=> global)
        
    return ( (uint16_t)( (MTV9V034_ReadVal16[0]<<8) + MTV9V034_ReadVal16[1]) ); // MTV9V034 returns MSByte of 16-bit register first 
}

// ----------------------------------------------------

/** Function : GAPOC_MT9V034_WriteReg16 
    Action : Using I2C1, Writes value WriteVal into 16-bit register of the MT9V034 specified by its 8-bit address RegAddr
*/
     
void GAPOC_MT9V034_WriteReg16(uint8_t RegAddr, uint16_t WriteVal)
{
// !!! Arguments of I2C functions have to be in L2 for access by uDMA

    I2C1_WrData16[0] = (uint8_t)RegAddr;        // First, send address ot target register for this 16-bit write access
    I2C1_WrData16[1] = (uint8_t)((WriteVal & 0xFF00) >> 8);   // MSByte, to send first to MT9V024 for 16-bit transfer -- CORRECT ?
    I2C1_WrData16[2] = (uint8_t)(WriteVal & 0x00FF);   
    // Perform actual transfer :     
    i2c_write(&i2c1, MT9V034_DEV_ADDR_WR, (const char*)I2C1_WrData16, 3, 1); // over data write length = 1+2 bytes 
    
//    DBG_PRINT("Reg.Ox%X now = 0x%X\n", RegAddr, GAPOC_MT9V034_ReadReg16(RegAddr));
}
     
// -------------------------------------------------------


/** Function : GAPOC_MT9V034_Start 
    Action : Powers up MT9V034 and initializes it according to user's parameters passed through structure *pGAPOC_MT9V034_Cfg.
             Exposure, Gain and Black level settings are not handled here.
*/

int8_t GAPOC_MT9V034_Start(GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg)
{
uint16_t  ChipId_Reg;
uint32_t  Original_Width, Original_Height;
uint32_t  Xstart, Ystart;

GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

GAPOC_MT9V034_Cfg_t    GAPOC_MT9V034_Cfg = *pGAPOC_MT9V034_Cfg;

    // Initial sanity checks:
    if ((GAPOC_MT9V034_Cfg.TargetWidth>MT9V034_WINDOW_WIDTH_MAX) || (GAPOC_MT9V034_Cfg.TargetHeight>MT9V034_WINDOW_HEIGHT_MAX))
    {
        DBG_PRINT("Error - Invalid CIS window size\n");
        return -1;   
    }

    GAPOC_GPIO_Set_Low(GPIO_CIS_EXP);      // Make sure trigger input is low -- normally redundant
    
    // Enable 3V3A/3V3D 
    GAPOC_GPIO_Set_High( GPIO_CIS_PWRON );

    // Wait for chip ready
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
          
    // Replace default values of some reserved registers as per recommendations ginven in datasheet Rev.D, Table 8 p14
    // Need different settings in case of binning ?
    if( (GAPOC_MT9V034_ReadReg16(MT9V034_COARSE_SHUTTER_WIDTH_A)==0) && (GAPOC_MT9V034_ReadReg16(MT9V034_FINE_SHUTTER_WIDTH_A)<456) )
    {
        GAPOC_MT9V034_WriteReg16(MT9V034_RESERVED_REG20, 0x01c7); // exception to below general case
    }
    else
    {
        GAPOC_MT9V034_WriteReg16(MT9V034_RESERVED_REG20, 0x03C7); // rather than default = 0x01C1
        GAPOC_MT9V034_WriteReg16(MT9V034_RESERVED_REG13, 0x2D2E); // rather than default = 0x01C1
    }
    GAPOC_MT9V034_WriteReg16(MT9V034_RESERVED_REG24, 0x001b); // rather than default = 0x0010
    GAPOC_MT9V034_WriteReg16(MT9V034_RESERVED_REG2B, 0x0003); // rather than default = 0x0004
    GAPOC_MT9V034_WriteReg16(MT9V034_RESERVED_REG2F, 0x0003); // rather than default = 0x0004

    // Set HDR mode if required
    if (GAPOC_MT9V034_Cfg.En_HDR)
    {
        GAPOC_MT9V034_WriteReg16( MT9V034_PIXEL_OPERATION_MODE, MT9V034_PIXEL_OPERATION_MODE_HDR_A);  
                
        // TRY THIS --  Select auto integration time for HDR
        // Keep default V1, V2, V3 for now
            #define T2_RATIO                4   // t2 = TotalExpTime / (2**T2_RATIO) - default 4
            #define T3_RATIO                7   // t3 = TotalExpTime / (2**T3_RATIO)  - default 6
            GAPOC_MT9V034_WriteReg16(MT9V034_SHUTTER_WIDTH_CONTROL_A, 
            1 << SHUTTER_WIDTH_HDR_AUTO_ADJUST_SHIFT | 
            0 << SHUTTER_WIDTH_SINGLE_KNEE_SHIFT  |
            T2_RATIO << SHUTTER_WIDTH_T2_RATIO_SHIFT  |
            T3_RATIO << SHUTTER_WIDTH_T3_RATIO_SHIFT   );  

        DBG_PRINT("HDR enable done\n");  
    }
    
    
    // Prepare Mask related to Colum/Row flipping for next operation (Configure Image Format)
    uint16_t ColRow_Flip_Bits = ( (GAPOC_MT9V034_Cfg.Row_Flip <<MT9V034_READ_MODE_ROW_FLIP_SHIFT) | (GAPOC_MT9V034_Cfg.Column_Flip <<MT9V034_READ_MODE_COLUMN_FLIP_SHIFT) );

    
    // Configure Image Format Binning, Cropping, Flipping...)
    if ( (!GAPOC_MT9V034_Cfg.Rescale_CroppingOnly) && (GAPOC_MT9V034_Cfg.TargetWidth <=MT9V034_WINDOW_WIDTH_MAX/2) && (GAPOC_MT9V034_Cfg.TargetHeight <=MT9V034_WINDOW_HEIGHT_MAX/2) )
    {

        if ( (GAPOC_MT9V034_Cfg.TargetWidth <=MT9V034_WINDOW_WIDTH_MAX/4) && (GAPOC_MT9V034_Cfg.TargetHeight <=MT9V034_WINDOW_HEIGHT_MAX/4) )  //so, can use Binx4
        {
            DBG_PRINT("MT9V034 in Binning x4 Mode\n");
            
            Original_Width = GAPOC_MT9V034_Cfg.TargetWidth *4;
            Original_Height = GAPOC_MT9V034_Cfg.TargetHeight *4;
            
            GAPOC_MT9V034_WriteReg16(MT9V034_READ_MODE_A, 
              (MT9V034_READ_MODE_ROW_BIN_MASK & (0x2<<MT9V034_READ_MODE_ROW_BIN_SHIFT))  // row downsampling x4
            | (MT9V034_READ_MODE_COLUMN_BIN_MASK & (0x2<< MT9V034_READ_MODE_COLUMN_BIN_SHIFT))  // column downsampling x4
            | ColRow_Flip_Bits  |   MT9V034_READ_MODE_RESERVED  ); // keep reseved bits to required value      
            
            // !!! In binning mode, MT9V034 appears to provide pixels with difft timing -- requiring clock inversion !!
            GAPOC_MT9V034_WriteReg16( MT9V034_PIXEL_CLOCK, MT9V034_PIXEL_CLOCK_INV_PXL_CLK);
            
            // Correct Pixel Count in Histogram 
//            GAPOC_MT9V034_WriteReg16( MT9V034_HISTOGRAM_PIXCOUNT, MT9V034_HISTOGRAM_PIXCOUNT_DEF/(4*4));                          
        }
        else  // TargetWidth<MAX/2 && TargetHeight <MAX/2 (...so, can use at least Binx2), but [TargetWidth >MAX/4 || TargetHeight] > MAX/4) (so, can't use Binx4)
        {
            DBG_PRINT("MT9V034 in Binning x2 Mode\n");
            
            Original_Width = GAPOC_MT9V034_Cfg.TargetWidth *2;
            Original_Height = GAPOC_MT9V034_Cfg.TargetHeight *2;
        

            GAPOC_MT9V034_WriteReg16(MT9V034_READ_MODE_A, 
              (MT9V034_READ_MODE_ROW_BIN_MASK & (0x1<<MT9V034_READ_MODE_ROW_BIN_SHIFT)) // row downsampling x2
            | (MT9V034_READ_MODE_COLUMN_BIN_MASK & (0x1<<MT9V034_READ_MODE_COLUMN_BIN_SHIFT))  // column downsampling x2
            | ColRow_Flip_Bits  |  MT9V034_READ_MODE_RESERVED  ); // keep reseved bits to required value    
            
            // !!! In binning mode, MT9V034 appears to provide pixels with difft timing -- requiring clock inversion !!
             GAPOC_MT9V034_WriteReg16( MT9V034_PIXEL_CLOCK, MT9V034_PIXEL_CLOCK_INV_PXL_CLK);
 
            // Correct Pixel Count in Histogram 
            GAPOC_MT9V034_WriteReg16( MT9V034_HISTOGRAM_PIXCOUNT, MT9V034_HISTOGRAM_PIXCOUNT_DEF/(2*2));                  
            
        }
    }
    else
    {
        DBG_PRINT("MT9V034 not in Binning Mode\n");
        Original_Width = GAPOC_MT9V034_Cfg.TargetWidth;
        Original_Height = GAPOC_MT9V034_Cfg.TargetHeight;               

        // (= default reg. settings) 
        GAPOC_MT9V034_WriteReg16(MT9V034_READ_MODE_A, 
           (MT9V034_READ_MODE_ROW_BIN_MASK & (0x0<< MT9V034_READ_MODE_ROW_BIN_SHIFT)) // no row downsampling 
         | (MT9V034_READ_MODE_COLUMN_BIN_MASK & (0x0<<MT9V034_READ_MODE_COLUMN_BIN_SHIFT)) // no column downsampling 
         | ColRow_Flip_Bits  |  MT9V034_READ_MODE_RESERVED  ); // keep reseved bits to required value    

    }
  
    Xstart = (MT9V034_WINDOW_WIDTH_MAX -Original_Width) /2;
    Ystart = (MT9V034_WINDOW_HEIGHT_MAX -Original_Height) /2;     
    GAPOC_MT9V034_WriteReg16(MT9V034_COLUMN_START_A, Xstart+1);     // first column index is 1    
    GAPOC_MT9V034_WriteReg16(MT9V034_WINDOW_WIDTH_A, Original_Width);                               
    GAPOC_MT9V034_WriteReg16(MT9V034_ROW_START_A, Ystart+4);         // CAUTION: First 4 rows are dark rows, not to be read out // !!! A VOIR ////
    GAPOC_MT9V034_WriteReg16(MT9V034_WINDOW_HEIGHT_A, Original_Height); 


    // Adjust target brightness
    // TODO -- This could be offered as user-definable setting
    #define BRIGHTNESS_1_64     MT9V034_AEC_AGC_DESIRED_BIN_DEF     // keep at MT9V034_AEC_AGC_DESIRED_BIN_DEF=58 for default brightness (Min=1, Max=64)
    GAPOC_MT9V034_WriteReg16( MT9V034_AEC_AGC_DESIRED_BIN, BRIGHTNESS_1_64 );    


    // Program Horizontal and Vertical Blanking accordingly:
    // Keep Frame Start Blanking and Frame End Blanking to their default values
    // Short row timing is not possible (see Table 5 Note 1 of DS rev D),
    // minimum total row time is 704pclk (hal width +blanking)
    // --> Decide to always keep total row time = 752+94 (max line width + default HBI) = 846pclk @26.66MHz -- keep same for 27MHz
    #define TOTAL_ROW_TIME 704 // was 846  // = max line width + default HBI
    GAPOC_MT9V034_WriteReg16( MT9V034_HORIZONTAL_BLANKING_A, TOTAL_ROW_TIME -GAPOC_MT9V034_Cfg.TargetWidth );

   
    // TODO ? Adjust vertical (/horizontal) blankink / total line time
    //   so that frame time is multiple of  1/100 or 1/120s
    //    under control of flag 50Hz/60Hz/no risk of (fluorescent) light flicker
    // but TBD - as only makes sense for video not single shots
        
    
    // Set MT9V034 in autotest mode if required:
    if (GAPOC_MT9V034_Cfg.En_Autotest)   // 
    {
         GAPOC_MT9V034_WriteReg16(MT9V034_TEST_PATTERN, MT9V034_TEST_PATTERN_ENABLE|MT9V034_TEST_PATTERN_GREY_DIAGONAL); // use grey shade test pattern
        /*
          // NB - would do this to also support custom defined grey fill:
          GAPOC_MT9V034_WriteReg16(MT9V034_TEST_PATTERN, 
            ((MT9V034_TEST_PATTERN_DATA_MASK & 0x000)<<MT9V034_TEST_PATTERN_DATA_SHIFT)
            | MT9V034_TEST_PATTERN_USE_DATA
            | MT9V034_TEST_PATTERN_ENABLE );  // Replace 10-bit out from ADC by (e.g. 0x2AA, leading to 0xAA when trucnated to 8 bits) 
        */
        GAPOC_MT9V034_WriteReg16(MT9V034_ROW_NOISE_CORR_CONTROL, 0x0000); // Make sure row noise correction is OFF in test mode (normally redundant as off by default...)
    }
    else  // non test mode
    {
        GAPOC_MT9V034_WriteReg16(MT9V034_TEST_PATTERN, 0x00); 
        if (GAPOC_MT9V034_Cfg.En_Row_Noise_Cancel)
        {
            GAPOC_MT9V034_WriteReg16(MT9V034_ROW_NOISE_CORR_CONTROL, MT9V034_ROW_NOISE_CORR_ENABLE_A);   // Enable row noise correction with default algo settings    
        }     
    }
         
   
    // Optional non-linear compression of brighter zones at ADC level   
    if (GAPOC_MT9V034_Cfg.En_ADC_NonLinear_Companding)
    {
        GAPOC_MT9V034_WriteReg16(MT9V034_ADC_COMPANDING, MT9V034_12TO10_COMPANDING_AB);    
    }
    else  // default  -- Remove at some point to save power
    {
        GAPOC_MT9V034_WriteReg16(MT9V034_ADC_COMPANDING, MT9V034_LINEAR_AB);    
    }
   
    DBG_PRINT("MT9V034 Ready and Configured\n");  

    return 0;
}

// -------------------------------------------------------

/** Function : GAPOC_MT9V034_Off 
    Action : Powers off MT9V034 
*/

void  GAPOC_MT9V034_Off()
{
//GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
//PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

    // Stop 3V3A/3V3D 
    GAPOC_GPIO_Set_Low(GPIO_CIS_PWRON);
}


// 

// -------------------------------------------------------

/** Function : GAPOC_MT9V034_I2C_Init 
    Action : Configures GAP8 pins assigned to I2C1 interfacing with MT9V034 and sets its working frequency
        as per user definition (through symbol CIS_I2C1_FREQ_KHz)
*/

void GAPOC_MT9V034_I2C1_Init(uint32_t i2c_freq_hz)
{
    i2c_init(&i2c1, I2C1_SDA_B34, I2C1_SCL_D1);  // I2C pins init, I2C udma channel init 
//    uint32_t i2c_freq_hz = CIS_I2C1_FREQ_KHz * 1000;
    i2c_frequency(&i2c1, i2c_freq_hz);  
    DBG_PRINT("I2C1 to MT9V034 configured -- Frequency set to %d KHz\n", CIS_I2C1_FREQ_KHz);
}


// -------------------------------------------------------

/** Function : GAPOC_MT9V034_CPI_Setup 
    Action : Initializes interfaces to MT9V034, i.e. I2C1 and CPI taking in account user-defined image dimensions passed through
             structure *pGAPOC_MT9V034_Cfg.  CPI channel of uDMA is also enabled. 
*/

void GAPOC_MT9V034_CPI_Setup( GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg ) // no parameters passed as using globals -- makes sense or better change ??
{
CPI_Type *const cpi_address[] = CPI_BASE_PTRS;
GAPOC_MT9V034_Cfg_t    GAPOC_MT9V034_Cfg = *pGAPOC_MT9V034_Cfg;

        // -- Init I2C1 for CIS  --------------------------------------  
        
        /*
        i2c_init(&i2c1, I2C1_SDA_B34, I2C1_SCL_D1);  // I2C pins init, I2C udma channel init 
        uint32_t i2c_freq_hz = CIS_I2C1_FREQ_KHz * 1000;
        i2c_frequency(&i2c1, i2c_freq_hz);  
        DBG_PRINT("I2C frequency set to %d KHz\n", CIS_I2C1_FREQ_KHz);
        */
        // or just:
        GAPOC_MT9V034_I2C1_Init(CIS_I2C1_FREQ_KHz * 1000);          
        
        CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7); 

        UDMA_Init((UDMA_Type *)cpi_address[0]); // REMOVE -- as redundant, already included in CPI_Init()  [tbc]            

        CPI_GetDefaultConfig(&cpi_config);
        cpi_config.row_len = GAPOC_MT9V034_Cfg.TargetWidth ; 
            // !! Keep equal to target width
            // Can normally be a portion of window width in slice mode 
            // but this is buggy on GAP8 Cut1
        cpi_config.resolution      = ( GAPOC_MT9V034_Cfg.TargetWidth * GAPOC_MT9V034_Cfg.TargetHeight );
        cpi_config.format          = BYPASS_BIGEND;  // Monochrome
        cpi_config.shift           = 0;
        cpi_config.slice_en        = 0;
        cpi_config.frameDrop_en    = 0;
        cpi_config.frameDrop_value = 0;
        cpi_config.wordWidth       = 16;  // 8, 16 or 32bits // IN FACT IGNORED in CPI DRIVER [gap_cpi.c] !!!!
        DBG_PRINT("CPI i/f incl. I2C1 initalized\n");
 
}


// -------------------------------------------------------

/** Function : GAPOC_MT9V034_uDMA_Config 
    Action : Configure uDMA to transfer 'buffer_length' words of 16-bit data (imposed by CPI) into buffer pointed by dest_buffer. 
*/

void GAPOC_MT9V034_uDMA_Config( unsigned char* dest_buffer, uint32_t buffer_length) 
{
// TODO - Cope with transfers >128KB
       

        cpiTransfer.data        = dest_buffer;
        cpiTransfer.dataSize    =  buffer_length;  // !! MUST BE <128K
        cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1); //0 -> 8bit //1 -> 16bit //2 -> 32bit -- ALWAYS USE 16-bits for CPI uDMA (?)

        DBG_PRINT("CPI uDMA configured\n");  
}


// -------------------------------------------------------

static volatile uint32_t Calib_Frame_Index =0;

/* Function : Callback_Calib (callback from non-blocking CPI transfer used in GAPOC_MT9V034_uDMA_Config() below)
    Action : Increments file-wide variable Calib_Frame_Index 
             and flags that new frame used for Black Level Calibration has been received by setting file-wide boolean Got_New_BlackLvl_Frame
*/
static void Callback_Calib()   
{
    Calib_Frame_Index++;
    Got_New_BlackLvl_Frame = true;
    // DBG_PRINT("Callback_Calib\n");
}

/** Function : GAPOC_MT9V034_BlackLevelCalib_Blocking 
    Action : Performs auto-calibration of Black Level and Gain. Exposure is fixed to a default duration (equivalent to one frame duration).
             This requires taking a number of successive pictures (e.g. 10)
             Parameters and results are adjusted according to the configuration specified by the user through structure *pGAPOC_MT9V034_Cfg (in
             particular HDR mode and Binning).
             Computed AGC, AEC and Black Level are stored in memory locations pointed by pAGC_Latest_Value, pAEC_Latest_Value and pBlack_Latest_Value_u16.
             The function handles enabling the CPI for the required number of frames and then disabling it.
             The function is blocking in the sense that it will exit only once the required number of frames to perform the full calibration have been processed.
*/

void  GAPOC_MT9V034_BlackLevelCalib_Blocking(uint16_t* pAGC_Latest_Value, uint16_t* pAEC_Latest_Value, uint16_t* pBlack_Latest_Value_u16, GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg  )
{

GAPOC_MT9V034_Cfg_t    GAPOC_MT9V034_Cfg = *pGAPOC_MT9V034_Cfg; 
CPI_Type *const cpi_address[] = CPI_BASE_PTRS;
uint16_t default_exposure_time;

#define NUM_FRAMES_FOR_BLACK_CAL    10

    // Decide if we're in binning mode or not
    bool row_binning_x2 = ( (GAPOC_MT9V034_ReadReg16(MT9V034_READ_MODE_A)&0x03)== 0x01 );
    bool row_binning_x4 = ( (GAPOC_MT9V034_ReadReg16(MT9V034_READ_MODE_A)&0x03)== 0x02 );
        
//    if (!GAPOC_MT9V034_Cfg.En_HDR)  // outside HDR only:  // (Temporary experiment)
    {
        // Enable AGC, force Exposure to # of lines in VGA (=max possible)
        // + compensate for binning as shutter width is programmed in terms of equiv. row duration *after* binning
        //    but we're interested in actual exposure time
        
        GAPOC_MT9V034_WriteReg16( MT9V034_AEC_AGC_ENABLE, MT9V034_AGC_ENABLE_A);    // Enable Gain auto_update only, no AEC
        

        if (row_binning_x4)
        {
            default_exposure_time = MT9V034_COARSE_SHUTTER_WIDTH_DEF/4;          
        }
        else if (row_binning_x2)
        {
            default_exposure_time = MT9V034_COARSE_SHUTTER_WIDTH_DEF/2;
        }
        else
        {
            default_exposure_time = MT9V034_COARSE_SHUTTER_WIDTH_DEF;
        }       
        
        // If we'll run in HDR mode, increase default exposure time (empirically found this is beneficial...)
        #define HDR_EXP_CALIB_MULT_FACTOR  2
        if (GAPOC_MT9V034_Cfg.En_HDR)   
        {
            default_exposure_time = default_exposure_time * HDR_EXP_CALIB_MULT_FACTOR;
        } 
    
        GAPOC_MT9V034_WriteReg16( MT9V034_COARSE_SHUTTER_WIDTH_A, default_exposure_time);  // Force Exposure duration 
    }
    
    // Make sure Black level is auto-calibrated, Average over 2**3=8 frames
    GAPOC_MT9V034_WriteReg16(MT9V034_BLACK_LEVEL_CTRL, MT9V034_BLACK_LEVEL_AUTO | (MT9V034_BLACK_LEVEL_AVG_FRAMES_MASK|(0x03<<MT9V034_BLACK_LEVEL_AVG_FRAMES_SHIFT)) );           

    
    // cpiTransfer.configFlags |= UDMA_CFG_CONTINOUS(1);     -- ??!!? Continuous doesn't work anymore ??   
    CPI_Enable(cpi_address[0], &cpi_config); // Program and enable CPI interface   

    CPI_ReceptionCreateHandle(cpi_address[0], &hCPI, Callback_Calib, NULL);  
    Calib_Frame_Index =0;
    CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);              
    // Launch MT9V034 in master mode -- 
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_MASTER_MODE | MT9V034_CHIP_CONTROL_DVP_ON | MT9V034_CHIP_CONTROL_SIMULTANEOUS); 
    
    // Receive xx frames before continuing
    while(Calib_Frame_Index <NUM_FRAMES_FOR_BLACK_CAL)
    {     
        // Would not need to relaunch if using DMA in *continuous* mode ...but doesn't work anymore ?!
        if (Got_New_BlackLvl_Frame)
        {
           CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);  
           Got_New_BlackLvl_Frame = false;    
         }
    }

    // Stop camera: go back to Snapshot mode 
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_ON | MT9V034_CHIP_CONTROL_SIMULTANEOUS);
    // Would be even better from power perspective to not even send frames from MT9V034 so CPI pins dont toggle
    //  -- or could simply force etst mode and send frames with data =0x00?

    CPI_Disable(cpi_address[0]);   
    CPI_Deinit(cpi_address[0]);   // Deinits uDMA for CPI (used to also disable CPI but no more)
    
    
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1) | UDMA_CFG_CONTINOUS(0); // Clear CONTINUOUS mode flag   (...if was set!)
     
     
    // Now remember Black Level, AGC, AEC levels :
    *pAGC_Latest_Value = GAPOC_MT9V034_ReadReg16(MT9V034_AGC_OUTPUT);
    *pAEC_Latest_Value = GAPOC_MT9V034_ReadReg16(MT9V034_AEC_OUTPUT);    
    *pBlack_Latest_Value_u16 = GAPOC_MT9V034_ReadReg16(MT9V034_BLACK_LEVEL_VALUE_A);    


    DBG_PRINT("\nAfter Auto-black\n");
    DBG_PRINT("AEC level= %d\n",(int)GAPOC_MT9V034_ReadReg16(MT9V034_AEC_OUTPUT));
    DBG_PRINT("AGC level= %d\n",(int)GAPOC_MT9V034_ReadReg16(MT9V034_AGC_OUTPUT));
    DBG_PRINT("Black level= %d\n",  (int)((int8_t)(0x00FF&GAPOC_MT9V034_ReadReg16(MT9V034_BLACK_LEVEL_VALUE_A))) );   

}


  
// -------------------------------------------------------


/** Function : GAPOC_MT9V034_MonitorMode 
    Action : Enables (if En_nDis is true) or disables (if En_nDis is false) the monitor mode of the MT9V034.
    In this mode, the MT9V034 would sleep for some user defined duration and wake up periodically to capture
    a few frames and use them for adjusting gain, exposure and black level (?)
*/

/** NOT TESTED ** -- For possible future use -- Will probably need tuning  **/
void    GAPOC_MT9V034_MonitorMode( bool En_nDis, uint16_t* pAGC_Latest_Value, uint16_t* pAEC_Latest_Value, uint16_t* pBlack_Latest_Value_u16)     
{ 
    if (En_nDis) // Enter monitor mode: sensor to sleep for ~5mn and wake up to capture xx frames
    {
        // First make sure AEC, AGC, Black calibration are in auto mode 
        GAPOC_MT9V034_WriteReg16(MT9V034_AEC_AGC_ENABLE, MT9V034_AEC_ENABLE_A|MT9V034_AGC_ENABLE_A);                  
        GAPOC_MT9V034_WriteReg16(MT9V034_BLACK_LEVEL_CTRL, MT9V034_BLACK_LEVEL_AUTO);    
        GAPOC_MT9V034_WriteReg16(MT9V034_MONITOR_NUM_FRAMES, MT9V034_MONITOR_DEFAULT_NUM); 
               
        GAPOC_MT9V034_WriteReg16(MT9V034_MONITOR_MODE, MT9V034_MONITOR_EN);   
    }
    else // exit monitor mode
    {
        GAPOC_MT9V034_WriteReg16(MT9V034_MONITOR_MODE, MT9V034_MONITOR_DIS);  
         
        // Remember adjusted Black Level, AGC, AEC levels :
        *pAGC_Latest_Value = GAPOC_MT9V034_ReadReg16(MT9V034_AGC_OUTPUT);
        *pAEC_Latest_Value = GAPOC_MT9V034_ReadReg16(MT9V034_AEC_OUTPUT);    
        *pBlack_Latest_Value_u16 = GAPOC_MT9V034_ReadReg16(MT9V034_BLACK_LEVEL_VALUE_A);       
    }     
}

// --------------

/** Function : GAPOC_MT9V034_Apply_Calibrations 
    Action : Puts MT9V034 in manual mode and sets gain, exposure and black level values to those passed as AGC_Latest, AEC_Latest and Black_Latest_Value_u16.
             These are typically the values obtained during a prior calibration phase, at the end of which they were saved for future use.
             Note that the MT9V034 validates its manual gain and exposure settings only at the end of the current frame -- therefore this function
             involves taking a dummy picture to ensure this validation
    (NB: pGAPOC_MT9V034_Cfg is passed but not used -- could be removed)
*/

void GAPOC_MT9V034_Apply_Calibrations( uint16_t AGC_Latest, uint16_t AEC_Latest, uint16_t Black_Latest_Value_u16, GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg  )
{

GAPOC_MT9V034_Cfg_t    GAPOC_MT9V034_Cfg = *pGAPOC_MT9V034_Cfg;

   // -- Disable auto-black and restore previously saved black level:
    GAPOC_MT9V034_WriteReg16( MT9V034_BLACK_LEVEL_CTRL, MT9V034_BLACK_LEVEL_OVERRIDE); // assume other bits are don't care in override mode
    GAPOC_MT9V034_WriteReg16( MT9V034_BLACK_LEVEL_VALUE_A, (uint16_t)Black_Latest_Value_u16); // assume other bits are don't care in override mode
    

    GAPOC_MT9V034_WriteReg16( MT9V034_AEC_AGC_ENABLE, 0x00);    // no AEC, no AGC (exposure+gain forced)   
    
    // Force exposure:
    GAPOC_MT9V034_WriteReg16( MT9V034_COARSE_SHUTTER_WIDTH_A, AEC_Latest); // force exposure    
 
    // Force Gain:
    if ( (AGC_Latest>=MT9V034_ANALOG_GAIN_MIN) && (AGC_Latest<=MT9V034_ANALOG_GAIN_MAX))
    {
        GAPOC_MT9V034_WriteReg16( MT9V034_ANALOG_GAIN_A, AGC_Latest);
    }   
    else 
    {
        DBG_PRINT("Analog Gain out of range\n");
    }


    // -- Take a dummy picture to validate these settings  :
    
    // Good idea to disable DVP pins (no need to send data) to save power
    // ...avtually this is already the case as DVP is off at exit of GAPOC_MT9V034_Start()

    // NB - This dummy exposure is first exposure so has necessarily default duration (480lines);
    // duration of very first exposure can't be changed as any change requires an exposure to be validated !

    // Trigger exposure :
    #define  ONE_EXP_DURATION_sec  ( (float)(MT9V034_COARSE_SHUTTER_WIDTH_DEF * TOTAL_ROW_TIME) / (CIS_PIXCLK_FREQ_KHz*1000.0) )  
    GAPOC_GPIO_Set_High(GPIO_CIS_EXP);
    wait(ONE_EXP_DURATION_sec);            // leave time to CIS to capture 1 dummy frame (could be made shorter in row binning?)
    GAPOC_GPIO_Set_Low(GPIO_CIS_EXP);
    
    // Note that DVP pins are still of at exit from this function
     
}

// --------------

/** Function : GAPOC_MT9V034_Enable_Full_Auto 
    Action : Selects fully automatic exposure control (AEC), gain control (AGC) and black level on MT9V034
*/

void GAPOC_MT9V034_Enable_Full_Auto( )
{
   // -- Enable auto-black :
    GAPOC_MT9V034_WriteReg16( MT9V034_BLACK_LEVEL_CTRL, MT9V034_BLACK_LEVEL_AUTO); // keeping default value for frame ot avrega over (could be chnaged)

   // -- Enable AEC, AGC :    
    GAPOC_MT9V034_WriteReg16( MT9V034_AEC_AGC_ENABLE, MT9V034_AEC_ENABLE_A|MT9V034_AGC_ENABLE_A);       
}

// -------------------------------------------------------

/** Function : GAPOC_MT9V034_Single_Shot_NonBlock 
    Action : Takes a single picture in non-blocking mode. Once the picture is taken, function 'Callback' will be called.
             This function enables CPI and generates rising edge on trigger signal which then remains high;
             it expect callback function to clear the trigger signal and to disable the CPI.            
*/

void GAPOC_MT9V034_Single_Shot_NonBlock( void(*Callback)() )      //argument Callback is a pointer to a function that takes no parameter and returns nothing
{  
GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
PORT_Type *const port_addrs[] = PORT_BASE_PTRS;
CPI_Type *const cpi_address[] = CPI_BASE_PTRS;
   
    // -- Configure CPI uDMA channel          
    CPI_Enable(cpi_address[0], &cpi_config); // Activate configuration of CPI channel   -- To be disabled in callback
    DBG_PRINT("CPI Channel Enabled\n");      

    // -- Enable actual capture of data from camera (non-blocking)
    // Beware: cpi_handle_t hCPI can't be declared locally (can't just live on the stack) and needs to be global
    CPI_ReceptionCreateHandle(cpi_address[0], &hCPI, Callback, NULL);  
    CPI_ReceptionNonBlocking(cpi_address[0], &hCPI, &cpiTransfer);  
//TODO: Cope with images requiring transfers >128KB


    // -- Switch camera to suitable mode (GPIO_CIS_EXP assumed low, normally already done!]
    GAPOC_MT9V034_WriteReg16(MT9V034_CHIP_CONTROL, MT9V034_CHIP_CONTROL_SNAPSHOT_MODE | MT9V034_CHIP_CONTROL_DVP_ON | MT9V034_CHIP_CONTROL_SIMULTANEOUS);            

    // -- GPIO_CIS_EXP snapshot trigger signal 
    GAPOC_GPIO_Set_High(GPIO_CIS_EXP);   // generate rising edge to trigger snapshot
    //To be set back low by Callback function

}



// ###   END OF FILE  ##################################################################################################################"""
