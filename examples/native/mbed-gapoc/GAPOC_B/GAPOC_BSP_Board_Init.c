// File: GAPOC_BSP_Board_Init.c

// #############################################################################
// ##### DECLARATIONS  #########################################################

// ====  Handy "defines" for application, used locally   =======================
// <none>


// ====  Includes    ===========================================================

#include "GAPOC_BSP_General.h"


// ====  Imported Global Variables  ============================================
// <none>


// ====  Exported Global Variables  ============================================
// <none>


// ====  File-wide Variables  ===================================================
// <none>      
            

// ----------------------------------------------------

/** Function : GAPOC_BSP_Board_Init 
    Action : Performs a full set of initial pin configuration suitable as default settings
             for GAPOC boards;
             also sets initial core voltage and frequency according to user needs
             specified through VOLTAGE_mV and CORE_FQCY_MHZ in GAPOC_BSP_Platform_Defs.h
*/

int GAPOC_BSP_Board_Init()
{  
GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

    int voltage_mV = VOLTAGE_mV;
    int fqcy_Hz = CORE_FQCY_MHZ * 1000000;


    // ---------------------------------------------

    // -- Set voltage   ----------
    if (PMU_SetVoltage(voltage_mV, 1)) {
            DBG_PRINT("Error when changing voltage\n");
            return -1;
    }

    // -- Set frequency  ----------
    if (FLL_SetFrequency(uFLL_SOC, fqcy_Hz, 1) == -1) {
            DBG_PRINT("Error of changing freqency, check Voltage value!\n");
            return -1;
    }
    
    DBG_PRINT("Frequency = %d Hz, Voltage = %d mv\n", (int)FLL_GetFrequency(uFLL_SOC), (int)voltage_mV);
    
    
    // -- GAPMod Pins  -------------------------
     
    // NB:  GPIO_xx_yy refers to GPIO #xx available on physical pin #yy of GAP8 
    // !!! #yy is omitted if only 1 physical pin matches  !!!
    

    // *** GAPMod Pin #1 = GAPA4_CONN = SPI1_MISO (for use on Connector CONN3, not available on Sensor Connector CONN9
    GAPOC_AnyPin_Config( A4, NOPULL, uPORT_MuxAlt0 );  // pin GAP_A4 keeps default function = SPIM1_MISO (input)
        // !! BEWARE !! GAPA4 can also be bear GPIO0 but in this implementation GPIO0 is already used on pin GAP_A3 
        // !!  ==> risk of conflict. DO NOT USE GAP_A4 as GPIO0 here.    
    
                
    // *** GAPMod Pin #2 = GAPB3_CONN = SENSOR_SPI_MOSI
//   GAPOC_AnyPin_Config( B3, NOPULL, uPORT_MuxAlt0 );  // pin GAP_B3 keeps default function = SPIM1_MOSI (output)
        // !! BEWARE !! GAPB3 can also be bear GPIO1 but in this implementation GPIO1 is already used on pin GAP_B2 
        // !!  ==> risk of conflict. DO NOT USE GAP_A4 as GPIO0 here.  
// DBG:
//    GAPOC_GPIO_Init_JustPull(GPIO_A1_B3); 
                
    // *** GAPMod Pin #3 = LED_G/GPIOGAP_SW1 = GAP_A3 = GPIO0
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A0_A3);      
    
    // *** GAPMod Pin #4 = GAPB2_CONN  (=HEARTBEAT LED if DIP Switch pos 6 is closed)
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A1_B2);  // = GPIO_LED_G/NINA_SW1, normally driven by NINA but can drive from GAP at Nina start-up (Nina boot mode)
        
    // *** GAPMod Pin #5 = GPIOGAP_NINARST# = GAP_A2 =  GPIO2
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A2_A2);      
        
    // *** GAPMod Pin #6 = GPIO_CIS_TRIGGER = GAP_B1  = GPIO3
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A3_B1);       
        
    // *** GAPMod Pin #7 = GPIO_1V8_EN = GAP_A44
#if (GAPOC_START_HYPERBUS_1V8==0)
    GAPOC_GPIO_Init_Pure_Output_Low (GPIO_A4_A44);     // = GPIO_1V8_EN  -->  1V8 not supplied to HyperMem after init
#else
    GAPOC_GPIO_Init_Pure_Output_High(GPIO_A4_A44);     // = GPIO_1V8_EN  -->  1V8 still supplied to HyperMem after init (in line w/ weak Pull-up on this signal on-board)
#endif

    // *** GAPMod Pin #8 = GPIO_CIS_DPWRON / GAP_D1 = GPIO16
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_CIS_DPWRON);           // Digital Power OFF
        
    // *** GAPMod Pin #9 = GPIO_CIS_APWRON / GAP_B40 = GPIO5
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_CIS_APWRON);       // Analog Power OFF         

       
    // *** GAPMod Pin #10 =  GAP_A43 = CPI_PCLK
    GAPOC_AnyPin_Config( A43, NOPULL, uPORT_MuxAlt0 );    //keep default function = CPI 
           
    // *** GAPMod Pin #11 =  GAP_B39 = CPI_D0
    GAPOC_AnyPin_Config( B39, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #12 =  GAP_A42 = CPI_D1
    GAPOC_AnyPin_Config( A42, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #13 =  GAP_B38 = CPI_D2
    GAPOC_AnyPin_Config( B38, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #14 =  GAP_A41 = CPI_D3
    GAPOC_AnyPin_Config( A41, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #15 =  GAP_A40 = CPI_D5
    GAPOC_AnyPin_Config( A40, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #16 =  GAP_B36 = CPI_D6
    GAPOC_AnyPin_Config( B36, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #17 =  GAP_A38 = CPI_D7
    GAPOC_AnyPin_Config( A38, NOPULL, uPORT_MuxAlt0 );   
        
    // *** GAPMod Pin #18 =  GAP_A37 = CPI_HSYNC
    GAPOC_AnyPin_Config( A37, NOPULL, uPORT_MuxAlt0 );   
       
    // *** GAPMod Pin #19 =  GAP_A36 = CPI_VSYNC
    GAPOC_AnyPin_Config( A36, NOPULL, uPORT_MuxAlt0 );     //keep default function = CPI

        
    // *** GAPMod Pin #20 = GND   
    // *** GAPMod Pin #20bis = GAP_VREGOUT (analog)   
    // *** GAPMod Pin #21 = VROOT    
    // *** GAPMod Pin #21bis = XTALVDD_RN      
    // *** GAPMod Pin #22 = GND    
    // *** GAPMod Pin #23 = 2V5_FUSE      
    // *** GAPMod Pin #24 = STM8_PB0_ADC ==> N/A       
    // *** GAPMod Pin #25 = STM8_PD0_GNDPULL ==> N/A    
    // *** GAPMod Pin #26 = STM8_PC5_PWREN ==> N/A   
    // *** GAPMod Pin #27 = STM8_PB2 ==> N/A        
    // *** GAPMod Pin #28 = STM8_PA0_SWIM ==> N/A         
    // *** GAPMod Pin #29 = GND   
       
    // *** GAPMod Pin #30 = GAP_A29 = JTAG_TMS      
    // *** GAPMod Pin #31 = GAP_B26 = JTAG_TDO        
    // *** GAPMod Pin #32 = GAP_B25 = JTAG_TCK     
        
    // *** GAPMod Pin #33 = RFU/NC        
    // *** GAPMod Pin #34 = GAP_A26 = I2S0_WS (no other option)
    // *** GAPMod Pin #35 = GAP_B23 = I2S0_SDI (no other option)       
    // *** GAPMod Pin #36 = GAP_A24 = I2S0_SCK (no other option)    
    
    // *** GAPMod Pin #37 = GAP_A25 = I2C0_SCL (no other option)            
    // *** GAPMod Pin #38 = GAP_B22 = I2C0_SDA (no other option)    
    
    // *** GAPMod Pin #39 = GND        
    // *** GAPMod Pin #40 = NRESET        
    // *** GAPMod Pin #41 = HYPER_RSTO#    
/*     
    // *** GAPMod Pin #42 = GAP_A7_VT = UART_GAP_TX (or GPIO25)    
    GAPOC_AnyPin_Config( A7, NOPULL, uPORT_MuxAlt0 );   // keep default function = UART_GAP_TX
    
    // *** GAPMod Pin #43 = GAP_B6_VT = UART_GAP_RX (or GPIO24)    
    GAPOC_AnyPin_Config( B6, NOPULL, uPORT_MuxAlt0 );   // keep default function = UART_GAP_RX
*/             
    // *** GAPMod Pin #44 = GAP_B12 = GPIO19 -- available on Connector    
    GAPOC_GPIO_Init_JustPull(GPIO_A19); 
         
    // *** GAPMod Pin #45 = GAP_A13 = GPIO18 -- available on Connector     
    GAPOC_GPIO_Init_JustPull(GPIO_A18); 
         
    // *** GAPMod Pin #46 = GND    
/*     
    // *** GAPMod Pin #47 = GAP_B11 = GPIO_A17 = GPIO_CIS_LED_EN#  ** low = enable, high = enable, invert signal, high-Z = OFF  
    GAPOC_GPIO_Init_HighZ(GPIO_A17);       
*/         
    // *** GAPMod Pin #48 = GND    
     
    // *** GAPMod Pin #49 = GAP_A5 = SENSOR_SPI_CS0
    GAPOC_AnyPin_Config( A5, NOPULL, uPORT_MuxAlt0 );  // pin GAP_A5 keeps default function = SENSOR_SPI_CS0 (output)
        // !! BEWARE !! GAP_A5 can also be bear GPIO2 but in this implementation GPIO2 is already used on pin GAP_A2 as NINA_RST# 
        // !!  ==> risk of conflict. DO NOT USE GAP_A5 as GPIO0 here.        
             
    // *** GAPMod Pin #50 = = GAP_B4 = SENSOR_SPI_SCK    
    GAPOC_AnyPin_Config( B4, NOPULL, uPORT_MuxAlt0 );  // pin GAP_B4 keeps default function = SENSOR_SPI_SCK (output)
        // !! BEWARE !! GAP_A5 can also be bear GPIO3 but in this implementation GPIO2 is already used on pin GAP_B1 as CSI_TRIGGER 
        // !!  ==> risk of conflict. DO NOT USE GAP_B4 as GPIO0 here.    
                 
    // *** GAPMod Pin #51 = GND    
     
    // *** GAPMod Pin #52 = GAP_B34 = GPIO_A15 = GPIO_CISCLK -- enables/disables ClkGen to IR Sensor Proxy Connector (**active LOW**) or directly drives it as PWM
    GAPOC_GPIO_Init_Pure_Output_High(GPIO_A15);       
         
    // *** GAPMod Pin #53 = VCAM_IO (2V5)    
    // *** GAPMod Pin #54 = GND    
/*     
    // *** GAPMod Pin #55 = GAP_B37 = CPI_D4
    GAPOC_AnyPin_Config( B36, NOPULL, uPORT_MuxAlt0 );   //keep default function = CPI
*/         
    // *** GAPMod Pin #56 = VCAM_IO (2V5)    
     
    // *** GAPMod Pin #57 = GAP_B27 = JTAG_NTRST    
     
    // *** GAPMod Pin #58 = VDD_SAFEIO (3V)    
 
    // *** GAPMod Pin #59 = GAP_A28 = JTAG_TDI    
     
    // *** GAPMod Pin #60 = GND    
    // *** GAPMod Pin #61 = 1V8    
    // *** GAPMod Pin #62 = 1V8    
 
    // *** GAPMod Pin #63 = HYPER_INTO# (from HyperMem)    
     
    // *** GAPMod Pin #64 = GAP_B13 = GPIOGAP_NINA17    
    GAPOC_GPIO_Init_Pure_Output_High(GPIO_A21);  // can be used as UART_DSR input on Nina (high= de-asserted)     
    
   
    
    // -- GAP8 I/Os not visible at GAPMod pin level :  -------------------------
    
    // -- HyperBus i/f
    // !! BEWARE:  Bug requires to disable HyperMem to use UART Rx
 /*
    hyperbus_t hyperbus0;
    hyperbus_init(&hyperbus0, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);    
 */  
        
 // TODO - TBD - Also initalize other non-GPIO pins here ?
 // e.g.may make sense to output NINA module in sleep mode
 

  return 0;
}

// ###   END OF FILE  ##################################################################################################################"""
