// File GAPOC_BSP_Platform_Defs.h

/**  ****************************************************
//
// This is a set of useful definitions pertaining to:
//                   GAPOC A
//
********************************************************/ 




#ifndef  __GAPOC_BSP_PLATFORM_DEFS_H
#define  __GAPOC_BSP_PLATFORM_DEFS_H  // prevent multiple inclusions



// ==  Includes relied upon in this file   ========================================
// <none>

// ==  Custom Types  ==============================================================



// ==  Defines    =================================================================


#define GAPOC_A 0xA0
#define GAPOC_B 0xB0
#define GAPOC_PLATFORM    GAPOC_A



// --  GENERAL ---------------------------------------------

// Set to 1 to enable debug features (DBG_PRINT() etc.), else to 0
#define  GAPOC_DEBUG  1

// Select FC frequency  -- 1 to 250MHz
#define CORE_FQCY_MHZ    50

// Select Voltage  -- 1000 to 1200mV
#define VOLTAGE_mV  1200

// Select if HyperMem (and therefore 1V8 supply) should stay enabled (if set to 1) or not (if 0) after init 
#define GAPOC_START_HYPERBUS_1V8    1
// Select if 3V_1V_MEMCORE_MEMIO suplying HyperMem should be enabled or not during sleep 
#define GAPOC_APPLY_HYPERBUS_1V8_IN_SLEEP    1

// --  MT9V034 CIS RELATED      -----------------------------

// Frequency of CIS pixel clock (keep @27MHz if using on-board ClkGen)
#define CIS_PIXCLK_FREQ_KHz         27000    

// Frequency of I2C to MT9V034 CIS
#define CIS_I2C1_FREQ_KHz           400



// --  NINA BLE MODULE RELATED      ----------------------

#define NINA_UART_AT_BAUDRATE_bps   115200   
#define GAPOC_NINA_MODULE_NAME      "GreenWaves-GAPOC"

// -- CONVENIENT GPIO ALIASES   -----------------------------


#define GPIO_CIS_PWRON              GPIO_A5_B40     // GPIO5 on physical pin B40 of GAP8
#define GPIO_CIS_EXP                GPIO_A3_B1
#define GAPOC_HEARTBEAT_LED         GPIO_A0_A3      // = GAPA3_CONN also used as Heartbeat LED -- only if Pos.6 of DIP switch is closed
#define GAPOC_NINA_NRST             GPIO_A2_A2      // 
#define GAPOC_1V8_EN                GPIO_A4_A44
#define GAPOC_NINA17_DSR            GPIO_A21        // = GAP_B13, drives DSR pin of NINA Module (input pin 17)
   

// ==  Public Functions Prototypes        ============================================
// <none>


#endif   //####  __GAPOC_BSP_PLATFORM_DEFS_H  #####################################################
