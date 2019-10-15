// File GAPOC_BSP_MT9V034.h

#ifndef  __GAPOC_BSP_MT9V034_H
#define  __GAPOC_BSP_MT9V034_H  // prevent multiple inclusions


// ==  Includes relied upon in this file   ========================================

#include "CIS_MT9V034.h"
#include "GAPOC_BSP_General.h"


// ==  Custom Types  ==============================================================


typedef struct {
    uint32_t            TargetWidth;       // Before any possible binning is applied
    uint32_t            TargetHeight;      // ditto
    bool                Rescale_CroppingOnly;   // Rescale uisng cropping only (reduces FOV), no binning
    bool                En_HDR;            // Enable High Dynamic Range mode
    bool                Row_Flip;       // Top-Bottom Mirroring
    bool                Column_Flip;    // Left-Right Mirroring
    bool                En_Autotest;       // NB - restricting to diagonal grey shade test pattern
    bool                En_Row_Noise_Cancel;
    bool                En_Exposure_LED;  
    bool                En_ADC_NonLinear_Companding;   
}  GAPOC_MT9V034_Cfg_t;
// TODO: SPECIFY DEFAULT INIT VALUES



// ==  Public Functions Prototypes        ============================================

uint16_t GAPOC_MT9V034_ReadReg16(uint8_t RegAddr);
void     GAPOC_MT9V034_WriteReg16(uint8_t RegAddr, uint16_t WriteVal);

int8_t    GAPOC_MT9V034_Start(GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg);  // returns non-0 if error

void GAPOC_MT9V034_Off();

void GAPOC_MT9V034_I2C1_Init(uint32_t i2c_freq_hz);

void GAPOC_MT9V034_CPI_Setup(GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg); // no parameters passed as using globals -- makes sense or better change ??

void GAPOC_MT9V034_uDMA_Config( unsigned char* dest_buffer, uint32_t buffer_length);

void  GAPOC_MT9V034_BlackLevelCalib_Blocking(uint16_t* pAGC_Latest_Value, uint16_t* pAEC_Latest_Value, uint16_t* pBlack_Latest_Value_u16, GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg  );
   
//void GAPOC_MT9V034_Gain_Exposure_Update_Blocking(uint16_t* pAGC_Latest_Value, uint16_t* pAEC_Latest_Value, uint16_t* pBlack_Latest_Value_u16);     

void GAPOC_MT9V034_MonitorMode( bool En_nDis, uint16_t* pAGC_Latest_Value, uint16_t* pAEC_Latest_Value, uint16_t* pBlack_Latest_Value_u16);

void GAPOC_MT9V034_Apply_Calibrations( uint16_t AGC_Latest, uint16_t AEC_Latest, uint16_t Black_Latest_Value_u16, GAPOC_MT9V034_Cfg_t* pGAPOC_MT9V034_Cfg );

void GAPOC_MT9V034_Enable_Full_Auto();

void GAPOC_MT9V034_Single_Shot_NonBlock( void(*Callback)() );      //argument is a pointer to a function 'Callback' that takes no parameter and returns nothing

//void GAPOC_MT9V034_Continuous_Frames_NonBlock( void(*Callback)() );


#endif  // GAPOC_BSP_MT9V034_H
