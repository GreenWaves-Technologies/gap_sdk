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
    
 
    // -- Default init for GPIOs -------------------------
    // NB:  GPIO_xx_yy refers to GPIO #xx available on physical pin #yy of GAP8 
    // !!! #yy is omitted if only 1 physical pin matches  !!!
    
    // -- SPECIAL CASES:  5x2 pins that share same GPIO source

    // NB - The same GPIOA0 can be routed to GAP_A4 (=GAPA4_CONN on GAPOC) or GAP_A3 (= GAPA3_CONN on GAPOC) -- beware of conflicts !    
    //  PLUS: if DIP switch pos. 6 is closed, GAP_A3 also drives Green "heatbeat" LED
// *** GAP_A4
    GAPOC_AnyPin_Config( A4, NOPULL, uPORT_MuxAlt0 );  // pin GAP_A4 keeps default function = SPIM1_MISO (input)
        // enable pull, as by default nothing on connector, so pin not driven
// *** GAP_A3  : Green LED OFF  (GPIO_A0_A3) (if DIP switch S6 set to allow Hearbeat LED) / GAPA3_CONN driven LOW from GAP
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A0_A3);  

    
    // NB - The same GPIOA1 can be routed to GAP_B2 (= GPIO_LED_G/NINA_SW1 on GAPOC) or GAP_B3 (= GAPB3_CONN on GAPOC) -- beware of conflicts !
// *** GAP_B3
    GAPOC_GPIO_Init_JustPull(GPIO_A1_B3); 
// *** GAP_B2
    GAPOC_GPIO_Init_JustPull(GPIO_A1_B2);  // = GPIO_LED_G/NINA_SW1, normally driven by NINA but can drive from GAP at Nina start-up (Nina boot mode)
    
    // NB - The same GPIOA2 can be routed to GAP_A2(=GPIO_NINA_RST#) or GAP_A5 (=GAP_A5_CONN) 
    //     ==> !! *** DO NOT USE GAP_A5 as GPIO (any other alt function ok) ***
    //     (unless NINA is not use -- even in that case, beware of NINA power consumption if toggling its RST#)
// *** GAP_A2
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A2_A2);      // = GPIO_NINA_NRST  // TBD - possible conflcit with GAP_A5_CONN above ?? (also pertains to GPIO A2)
// *** GAP_A5
    GAPOC_AnyPin_Config( A5, NOPULL, uPORT_MuxAlt0 );  // pin GAP_A5 routed to GAPA5_CONN keeps default function = SPIM1_CS0 (output)

    
    // NB - The same GPIOA3 can be routed to GAP_B1(=GPIO_CIS_EXP) or GAP_B4(=GAP_B4_CONN) -- beware of conflicts !
    //  ==> !! *** DO NOT USE GAP_B4 as GPIO if GPIO_CIS_EXP is required by CIS (single shot mode)
// *** GAP_B1
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A3_B1);      // = GPIO_CIS_EXP   
// *** GAP_B4
    GAPOC_AnyPin_Config( B4, NOPULL, uPORT_MuxAlt0 );  // pin GAP_B4 routed to GAPB4_CONN keeps default function = SPIM1_SCK (output)        
    
    
    // NB - The same GPIOA4 can be routed to GAP_A44 (GPIO_1V8_EN) or GAP_A43 (=CIS_PCLK) 
    //      No conflict here since GAP_A43 dedicated to CPI interface, not available for use as GPIO
// *** GAP_A44
#if (GAPOC_START_HYPERBUS_1V8==0)
    GAPOC_GPIO_Init_Pure_Output_Low (GPIO_A4_A44);     // = GPIO_1V8_EN  -->  1V8 not supplied to HyperMem after init
#else
    GAPOC_GPIO_Init_Pure_Output_High(GPIO_A4_A44);     // = GPIO_1V8_EN  -->  1V8 stil supplied to HyperMem after init (in line w/ weak Pull-up on this signal on-board)
#endif
// *** GAP_A43
    GAPOC_AnyPin_Config( A43, NOPULL, uPORT_MuxAlt0 );  // pin GAP_A43 keeps default function = CAM_PCLK (input)
        // If CIS is not powered then PCLK=0V (sure??), if powered then PCLK is driven to 0 or 1 ==> won't be floating, no need to enable Pull Resistor  
    
        
    // NB - The same GPIOA5 can be routed to GAP_B40 (=GPIO_CIS_PWRON) or GAP_A37 (=CIS_HSYNC) 
    //      No conflict here since GAP_A37 dedicated to CPI interface, not available for use as GPIO       
// *** GAP_B40
    GAPOC_GPIO_Init_Pure_Output_Low(GPIO_A5_B40);     // = GPIO_CIS_PWRON      
// *** GAP_A37
    GAPOC_AnyPin_Config( A37, NOPULL, uPORT_MuxAlt0 );  // pin GAP_A37 keeps default function = CAM_HSYNC (input)   
        // If CIS is not powered then HSYNC=0V (sure??), if powered then HSYNC is driven to 0 or 1 ==> won't be floating, no need to enable Pull Resistor          
    
    
    // -- REMAINING GPIOs (no possible GPIO conflict issues) :
 
// *** GAP_B12
    GAPOC_GPIO_Init_JustPull(GPIO_A19);   // = GAPB12_CONN 
// *** GAP_A13
    GAPOC_GPIO_Init_JustPull(GPIO_A18);   // = GAPA13_CONN  
    

// *** GAP_B13
    GAPOC_GPIO_Init_Pure_Output_High(GPIO_A21);  // = GPIO_A21 =GPIO_NINA17, can be used as UART_DSR input on Nina (high= de-asserted)   


// ** HYPERBUS INTERFACE
// TODO -- May need to use a #define to restrict this to platforms using HyperBus -- some other platforms may work w/o ext memory
//          or use QSPI Flash/RAM rather than Hyperbus..
    hyperbus_t hyperbus0;
    hyperbus_init(&hyperbus0, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);


 // TODO - TBD - Also initalize other non-GPIO pins here ?
 // e.g.may make sense to output NINA module in sleep mode
 

  return 0;
}

// ###   END OF FILE  ##################################################################################################################"""
