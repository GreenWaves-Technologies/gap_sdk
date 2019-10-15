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
// <none>


// ====  Includes    ==========================================================

#include "GAPOC_BSP_General.h"

#include "hyperbus_api.h"

#include "mbed_wait_api.h"


// ====  Application's Custom Types    ========================================
// <none>

    
// ==== Application's public global variables  ==============================
// <none>


// ==== Application's own global variables  ====================================
// <none>


// ==== Application's Function Prototypes    ===================================
// <none>


// #############################################################################
// ##### MAIN APPLICATION  ###########################################

int main()
{

    DBG_PRINT("\nGAPOC LED Blink Test\n\n");
    DBG_PRINT("\n\n** DIP Switch position #6 must be closed (ON) to enable the on-board LED **\n");
    DBG_PRINT("and Pin #2 of Connector3 then bears the GPIO signal (from GAP8 pin B2) that controls the LED\n\n");    

      
    //  Initalize Board (GPIO direction and default level, supplies, etc.)       
    GAPOC_BSP_Board_Init();

    // Initialize HyperBus I/Os to limit consumption from Memory
    // Ultimately this should go into GAPOC_BSP_Board_Init();
/*
    hyperbus_t hyperbus0;
    hyperbus_init(&hyperbus0, HYPERBUS_DQ0, HYPERBUS_DQ1, HYPERBUS_DQ2, HYPERBUS_DQ3,
                  HYPERBUS_DQ4, HYPERBUS_DQ5, HYPERBUS_DQ6, HYPERBUS_DQ7,
                  HYPERBUS_CLK, HYPERBUS_CLKN, HYPERBUS_RWDS, HYPERBUS_CSN0, HYPERBUS_CSN1);
*/   
   // On GAPOC B 1.0:
   // When ULIS IR Proxy is connected, if disabling 4V boost converter we still get some analog voltage to IR sensor on GAPOC_B 1.0
   // and ~80mA power consumption !
   // Better turn 4V to sensor on then...
   // Else, on GAPOC_B1.1 turn them off:
    GAPOC_GPIO_Set_High( GPIO_CIS_APWRON );   // Turn Off/on AVDD                                
    GAPOC_GPIO_Set_High( GPIO_CIS_DPWRON );   // Turn Off/on DVDD           
    GAPOC_GPIO_Set_High( GPIO_CIS_CLK );      // Turn Off/on MCLK (active low control)       


    // Initialize relevant GPIO as pure output, starting @high logic level
    GAPOC_GPIO_Init_Pure_Output_High( GAPOC_HEARTBEAT_LED );

    //  ---   Main Loop                  --------------------------------------------------------------------------------       
    
    #define LED_ON_TIME_ms   150
    #define LED_OFF_TIME_ms  850
    while(1)
    {
        wait( (float)(LED_ON_TIME_ms)/1000.0 );
        GAPOC_GPIO_Toggle( GAPOC_HEARTBEAT_LED );
        
        wait( (float)(LED_OFF_TIME_ms)/1000.0 );
        GAPOC_GPIO_Toggle( GAPOC_HEARTBEAT_LED );        

    }
    
}
// ## END OF FILE ##############################################################################


