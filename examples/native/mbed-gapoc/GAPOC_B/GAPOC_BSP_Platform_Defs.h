// File GAPOC_BSP_Platform_Defs.h

/**  ****************************************************
//
// This is a set of useful definitions pertaining to:
//                   GAPOC B
//
********************************************************/ 



#ifndef  __GAPOC_BSP_PLATFORM_DEFS_H
#define  __GAPOC_BSP_PLATFORM_DEFS_H  // prevent multiple inclusions


// ==  Includes relied upon in this file   ========================================
// <none>

// ==  Custom Types  ==============================================================

//typedef enum  _PullEn {NOPULL=0, PULL=1}  GAPOC_PullEn_t;


// ==  Defines    =================================================================

#define GAPOC_A 0xA0
#define GAPOC_B 0xB0
#define GAPOC_PLATFORM    GAPOC_B


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
#define GAPOC_APPLY_HYPERBUS_1V8_IN_SLEEP    0


// --  Thermal Imaging Proxy Related      -----------------------------

// Frequency of CIS pixel clock
// Needed ?

// Frequency of SPI to Proxy
#define     SPI_FQCY_IRSENSOR_KHZ   400    // must be in 10KHz to 1MHz range


// --  NINA BLE MODULE RELATED      ----------------------

#define NINA_UART_AT_BAUDRATE_bps   115200   
#define GAPOC_NINA_MODULE_NAME      "GreenWaves-GAPOC"


// -- CONVENIENT GPIO ALIASES   -----------------------------

  #define GPIO_CIS_DPWRON             GPIO_A16        // GPIO16 on physical pin D1 of GAP8
  #define GPIO_CIS_APWRON             GPIO_A5_B40     // GPIO5 on physical pin B40 of GAP8
  #define GPIO_CIS_TRIGGER            GPIO_A3_B1
  #define GPIO_CIS_LED_ENB            GPIO_A17        // GPIO A17 on physical pin B11 of GAP8

/*  
  #define GPIO_CIS_CLK                GPIO_A15   // GPIO_A15 = GAP_B34, used either as enable of clkgen for CIS clock or as PWM source driving CIS clock (assy option)
*/
// !!! The above was the original intention -  However GAP8 cut1.0/1.1 has a bug that causes Vsync to be stuck if using B34 as GPIO.
//     ...so on fixed B0.2 (PCB with manual rework), pin StdBy of ClkGen now driven from GAP_A4 = GPIO_A0 
  #define GPIO_CIS_CLK_NOTEN            GPIO_A0_A4      //...so on B0.2 reworked version, pin StdBy of ClkGen now driven from GAP_A4 = GPIO_A0               
                                      // *** CAUTION  GPIO0 also available on GAP_A3 and used as LED_G/GPIOGAP_SW1                        

                                          
  #define GAPOC_HEARTBEAT_LED         GPIO_A1_B2      // GAPB2_CONN becomes Heartbeat LED if Pos.6 of DIP switch is closed
  #define GAPOC_NINA_NRST             GPIO_A2_A2      // 
  #define GAPOC_1V8_EN                GPIO_A4_A44
  #define GAPOC_NINA17_DSR            GPIO_A21        // = GAP_B13, drives DSR pin of NINA Module (input pin 17) 



// ==  Public Functions Prototypes        ============================================
// <none>


#endif   //####  __GAPOC_BSP_PLATFORM_DEFS_H  #####################################################
