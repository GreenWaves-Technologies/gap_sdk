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
 
 
#include "GAPOC_BSP_General.h"

#include "hyperbus_api.h"
#include "mbed_wait_api.h"

#define RETENTIVE  1
#define DEEP 2

// ===   USER CHOICES  ==============================================
#define  SLEEP_MODE     RETENTIVE   // either DEEP or RETENTIVE
#define  USE_PADSLEEP   false   // set to true or false
// ===================================================================

/* Latency before going to first sleep */
#define  NODEEPSLEEP_DURATION_sec  1

/* Wake up after several second*/
#define SLEEP_DURATION_sec         3


//---------------------------------------------------------------------------
// -- Define the behaviour of all pads in SLEEP mode ---      
// -- This should ultimately go into BSP 

// See registers SAFE_SLEEPPADCFGx
// bit[2*k+1] = to set value Low (if 0)/High (if 1), bit[2k]= to set as input (if 0)/output (if 1)  
// (assuming High/Low is irrelevant when pin is input) (TODO: Confirm this)
        
#define GAPOC_SLEEP_PADCFG_SETDIR_IN    0
#define GAPOC_SLEEP_PADCFG_SETDIR_OUT   1
#define GAPOC_SLEEP_PADCFG_SETVAL_0    0
#define GAPOC_SLEEP_PADCFG_SETVAL_1     1  


// TODO: Check this:
// If 1V8 going to GAP8 I/Os is OFF in sleep mode then using PADSLEEPCFG for those I/Os is useless... (?)
// Other GAP8 I/Os are always on in GAPOC_A (...correct?)



// -- Register SAFE_SLEEPPADCFG0  ----

// Prepare each bit for this register :

#define     GAPOC_SLEEP_PADCFG_A4    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<0  // to GAPOC connector
#define     GAPOC_SLEEP_PADCFG_B3    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<2  // to GAPOC connector 
#define     GAPOC_SLEEP_PADCFG_A5    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<4  // to GAPOC connector        
#define     GAPOC_SLEEP_PADCFG_B4    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<6  // to GAPOC connector        
#define     GAPOC_SLEEP_PADCFG_A3    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<8  // to GAPOC connector       
//#define     GAPOC_SLEEP_PADCFG_B2    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<10   // to Nina LED_G/GPIOGAP_SW1
#define     GAPOC_SLEEP_PADCFG_B2    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN) ) <<10   // to Nina LED_G/GPIOGAP_SW1
        
#define     GAPOC_SLEEP_PADCFG_A2    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<12    // NINA_RST#
//TODO - Strategy on BLE -- keep on , not in reset but in standby mode ?

#define     GAPOC_SLEEP_PADCFG_B1    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<14   // CIS_EXP    
/*
#if   GAPOC_APPLY_HYPERBUS_1V8_IN_SLEEP == 0
  #define     GAPOC_SLEEP_PADCFG_A44    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<16  // GPIO_1V8_EN       
#elif GAPOC_APPLY_HYPERBUS_1V8_IN_SLEEP == 1
  #define     GAPOC_SLEEP_PADCFG_A44    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<16  // GPIO_1V8_EN       

//#else   #error GAPOC_APPLY_HYPERBUS_1V8_IN_SLEEP not (or not properly) defined
#endif
*/
#if   SLEEP_MODE == DEEP
  #define     GAPOC_SLEEP_PADCFG_A44    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<16  // GPIO_1V8_EN, keep 1V8 for Mem in deep sleep       
#elif SLEEP_MODE == RETENTIVE
  #define     GAPOC_SLEEP_PADCFG_A44    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<16  // GPIO_1V8_EN, can cut 1V8 in Ret Sleep       
//#else   #error SLEEP_MODE not (or not properly) defined
#endif

#define     GAPOC_SLEEP_PADCFG_B40    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<18   // GPIO_CIS_PWRON
// TBD: might be worth setting CPI pins as outputs rather than inputs else floating as CIS is off...
#define     GAPOC_SLEEP_PADCFG_A43    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<20    // CIS_CLK
#define     GAPOC_SLEEP_PADCFG_A37    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<22    // CIS_HSYNC
#define     GAPOC_SLEEP_PADCFG_B39    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<24    // CPI_D0       
#define     GAPOC_SLEEP_PADCFG_A42    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<26    // CPI_D1        
#define     GAPOC_SLEEP_PADCFG_B38    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<28    // CPI_D2      
#define     GAPOC_SLEEP_PADCFG_A41    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<30    // CPI_D3                       

// Aggregate this into value to write in register :
#define     GAPOC_SLEEP_PADCFG_REG0  \
        (     GAPOC_SLEEP_PADCFG_A41 | GAPOC_SLEEP_PADCFG_B38 | GAPOC_SLEEP_PADCFG_A42 | GAPOC_SLEEP_PADCFG_B39  \
            | GAPOC_SLEEP_PADCFG_A37 | GAPOC_SLEEP_PADCFG_A43 | GAPOC_SLEEP_PADCFG_B40 | GAPOC_SLEEP_PADCFG_A44  \
            | GAPOC_SLEEP_PADCFG_B1 | GAPOC_SLEEP_PADCFG_A2 | GAPOC_SLEEP_PADCFG_B2 | GAPOC_SLEEP_PADCFG_A3      \
            | GAPOC_SLEEP_PADCFG_B4 | GAPOC_SLEEP_PADCFG_A5 | GAPOC_SLEEP_PADCFG_B3 | GAPOC_SLEEP_PADCFG_A4  )
                  
                               
// --  Register SAFE_SLEEPPADCFG1  -----         

// Prepare each bit for this register :
          
#define     GAPOC_SLEEP_PADCFG_B37    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<0  // CPI_D4
#define     GAPOC_SLEEP_PADCFG_A40    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<2  // CPI_D5
#define     GAPOC_SLEEP_PADCFG_B36    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<4  // CPI_D6
#define     GAPOC_SLEEP_PADCFG_A38    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<6  // CPI_D7                                 
#define     GAPOC_SLEEP_PADCFG_A36    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<8  // CIS_VSYNC
#define     GAPOC_SLEEP_PADCFG_B34    ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<10  // I2C1_SDA_CIS, ext. pull-up to 3V3D (assumed OFF if GAP8 put in sleep mode)
#define     GAPOC_SLEEP_PADCFG_D1     ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  ) <<12  // I2C1_SCL_CIS, ext. pull_up to 3V3D (assumed OFF if GAP8 put in sleep mode)
#define     GAPOC_SLEEP_PADCFG_B11    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<14  // GAP_PWM_CISCLK                               
#define     GAPOC_SLEEP_PADCFG_A13    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )   <<16 // On GAPOC_A_0.1: is GPIOGAP_NINA16 = Nina UART_DTR (Nina output), on 0.2: is GAP_A13_CONN
#define     GAPOC_SLEEP_PADCFG_B12    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<18  // GAP_B12_CONN
#define     GAPOC_SLEEP_PADCFG_A14    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<20  // GPIO_GAP_STM8
#define     GAPOC_SLEEP_PADCFG_B13    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<22  // GPIOGAP_NINA17 = Nina UART_DSR (Nina input)             
#define     GAPOC_SLEEP_PADCFG_A15    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0)  <<24 // HYPER_CKN  (assume HyperMem's 1V8 is off when setting GAP8 to sleep        
  // !! TODO: actually 1V8 will be ON in deep sleep, (most likely) off in retentive sleep -- Handle this in the above !                           
#define     GAPOC_SLEEP_PADCFG_B14    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<26  // HYPER_CK
#define     GAPOC_SLEEP_PADCFG_B6     ( (GAPOC_SLEEP_PADCFG_SETDIR_IN) )   <<28  // UART_TO_GAP  // TODO - may need changing as UART GAP8 in on 1V8 which may be off so line may float
//#define     GAPOC_SLEEP_PADCFG_A7     ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  )  <<30  // UART_FROM_GAP  -- set to 0 as going to translator which is off
#define     GAPOC_SLEEP_PADCFG_A7     ( (GAPOC_SLEEP_PADCFG_SETDIR_IN)  )  <<30  // UART_FROM_GAP  -- set to 0 as going to translator which is off


// Aggregate this into value to write in register :
#define     GAPOC_SLEEP_PADCFG_REG1  \
        (     GAPOC_SLEEP_PADCFG_A7 | GAPOC_SLEEP_PADCFG_B6 | GAPOC_SLEEP_PADCFG_B14 | GAPOC_SLEEP_PADCFG_A15   \
            | GAPOC_SLEEP_PADCFG_B13 | GAPOC_SLEEP_PADCFG_A14 | GAPOC_SLEEP_PADCFG_B12 | GAPOC_SLEEP_PADCFG_A13 \
            | GAPOC_SLEEP_PADCFG_B11 | GAPOC_SLEEP_PADCFG_D1 | GAPOC_SLEEP_PADCFG_B34 | GAPOC_SLEEP_PADCFG_A36  \
            | GAPOC_SLEEP_PADCFG_A38 | GAPOC_SLEEP_PADCFG_B36 | GAPOC_SLEEP_PADCFG_A40 | GAPOC_SLEEP_PADCFG_B37  )


// --  Register SAFE_SLEEPPADCFG2 -----

// Prepare each bit for this register :
          
#define     GAPOC_SLEEP_PADCFG_D2    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<0  // HYPER_DQ0                              
#define     GAPOC_SLEEP_PADCFG_A11   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<2  // HYPER_DQ1                              
#define     GAPOC_SLEEP_PADCFG_B10   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<4  // HYPER_DQ2                              
#define     GAPOC_SLEEP_PADCFG_A10   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<6  // HYPER_DQ3                              
#define     GAPOC_SLEEP_PADCFG_B8    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<8  // HYPER_DQ4
#define     GAPOC_SLEEP_PADCFG_A8    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<10  // HYPER_DQ5                              
#define     GAPOC_SLEEP_PADCFG_B7    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<12  // HYPER_DQ6                              
#define     GAPOC_SLEEP_PADCFG_A9    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<14  // HYPER_DQ7                              
#define     GAPOC_SLEEP_PADCFG_B15   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<16 // HYPER_CSN0       
#define     GAPOC_SLEEP_PADCFG_A16   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<18 // HYPER_CSN1                              
#define     GAPOC_SLEEP_PADCFG_B9    ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<20  // HYPER_RWDS                              
#define     GAPOC_SLEEP_PADCFG_B22   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<22  // GAP_B22_CONN ** also I2C STM8 w/ PU to 3V !!!  so drive to 1 (3V)                  
#define     GAPOC_SLEEP_PADCFG_A25   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_1 <<1  ) <<24  // GAP_A25_CONN ** also I2C STM8 w/ PU to 3V !!!  so drive to 1 (3V)                                            
#define     GAPOC_SLEEP_PADCFG_A24   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<26  // NC on GAPOC_A_0.1, to CONN on 0.2                             
#define     GAPOC_SLEEP_PADCFG_A26   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<28  // NC on GAPOC_A_0.1, to CONN on 0.2                              
#define     GAPOC_SLEEP_PADCFG_B23   ( (GAPOC_SLEEP_PADCFG_SETDIR_OUT) | GAPOC_SLEEP_PADCFG_SETVAL_0 <<1  ) <<30  // NC on GAPOC_A_0.1, to CONN on 0.2                              

// Aggregate this into value to write in register :
#define     GAPOC_SLEEP_PADCFG_REG2  \
        (     GAPOC_SLEEP_PADCFG_B23 | GAPOC_SLEEP_PADCFG_A26 | GAPOC_SLEEP_PADCFG_A24 | GAPOC_SLEEP_PADCFG_A25 \
            | GAPOC_SLEEP_PADCFG_B22 | GAPOC_SLEEP_PADCFG_B9 | GAPOC_SLEEP_PADCFG_A16 | GAPOC_SLEEP_PADCFG_B15  \
            | GAPOC_SLEEP_PADCFG_A9  | GAPOC_SLEEP_PADCFG_B7 | GAPOC_SLEEP_PADCFG_A8  | GAPOC_SLEEP_PADCFG_B8   \
            | GAPOC_SLEEP_PADCFG_A10 | GAPOC_SLEEP_PADCFG_B10 | GAPOC_SLEEP_PADCFG_A11 | GAPOC_SLEEP_PADCFG_D2   )
//---------------------------------------------------------------------------


                               
GAP_L2_DATA static int retentive_counter = 0;  // GAP_L2_DATA directive needed for variable to survive retentive sleep mode (tbc)


static void pmu_wakeup_by_rtc()
{
   int repeat_en = 0;  
//    int repeat_en = 1;  
       
    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);
    
    // Set Timer 
    RTC_SetTimer(RTC_APB, SLEEP_DURATION_sec);

    // Start Timer 
    RTC_StartTimer(RTC_APB, repeat_en);  // if no repeat, timer counter will reload but not restart
}


//---------------------------------------------------------------------------


int main()
{

    PORTA->PAD_SLEEP = 0x00; // restore normal pad behaviour if coming back from sleep (NEEDED ???)
        
    // Initalize Board (GPIO direction and default level, supplies, etc.)
    GAPOC_BSP_Board_Init();  // only in cold boot ?? TBD

   
    // Rely on default init of HyperFlash
     
    if ( (PMU_WakeupState() == uPMU_COLD_BOOT) || (PMU_WakeupState() == uPMU_DEEP_SLEEP_BOOT))
    {

        GAPOC_GPIO_Set_High(GAPOC_HEARTBEAT_LED);                
        
        if (PMU_WakeupState() == uPMU_COLD_BOOT)
        {

            DBG_PRINT("Entering test from cold boot, retentive_counter=%d \n", (int)retentive_counter);
        }      

        retentive_counter = 1;
                

        DBG_PRINT("Going to sleep, retentive_counter=%d \n", (int)retentive_counter); 
        
        wait(NODEEPSLEEP_DURATION_sec);         
        GAPOC_GPIO_Set_Low(GAPOC_HEARTBEAT_LED);

//        pmu_wakeup_by_rtc();      
// !!!    TEMPORARY FOR CUT1.0 :  !!!
// Unable to restart prog properly upon exit from first deep sleep with PadSleep used
// ...so don't wake up - to allow measuring consumption in Deep Sleep
#if   (SLEEP_MODE != DEEP)  || (USE_PADSLEEP == false)
        pmu_wakeup_by_rtc();      
#endif


        // Enable sleep mode for pads :   ----------  
        // (To move into a function of GAPOC_BSP !)
        PORTA->SLEEP_PADCFG[0] = GAPOC_SLEEP_PADCFG_REG0;
        PORTA->SLEEP_PADCFG[1] = GAPOC_SLEEP_PADCFG_REG1;
        PORTA->SLEEP_PADCFG[2] = GAPOC_SLEEP_PADCFG_REG2;
        if (USE_PADSLEEP)
        {        
            PORTA->PAD_SLEEP = 0x01; 
        }
        else
        {        
            PORTA->PAD_SLEEP = 0x00; 
        }        
// !! Limitations with Cut1.0 & current SDK
//     Retentive sleep + Boot from Flash (normally useless)+ use or not PadSleep--> OK, can enter/Exit ret. sleep undefinitely
//     Retentive sleep + Boot from JTAG + use or not PadSleep --> Can wake up correctly ONCE 
//     Deep sleep + Boot from Flash (normal) + PadSleep not used --> OK, can enter/Exit deep sleep undefinitely     
//     Deep sleep + Boot from Flash (normal) + PadSleep used --> wake-up FAILS  
 
        // Should also put ext Flash/RAM in Deep Power Down here !!
        // or, in retentive mode only, possibly switch its 1V8 supply off

        
        // Use argument uPMU_SWITCH_RETENTIVE_SLEEP for retentive sleep / uPMU_SWITCH_DEEP_SLEEP for Deep Sleep

#if (SLEEP_MODE == DEEP)
        PMU_StateSwitch(uPMU_SWITCH_DEEP_SLEEP, uPMU_SWITCH_FAST);  // 2nd argument = ??
#else      
        PMU_StateSwitch(uPMU_SWITCH_RETENTIVE_SLEEP, uPMU_SWITCH_FAST);  // 2nd argument = ??
#endif
        
        while(1);  // Stop here until power actually cut off  
        
        
    } 
    else 
    {

        retentive_counter++;
        
        // After wake-up: Blink Green LED N times then go back to sleep
        #define LED_OFFTIME_NOM_ms  250.0    
        #define NUM_BLINKS  4
        for (uint32_t i=0; i<NUM_BLINKS; i++)
        {

            GAPOC_GPIO_Set_High(GAPOC_HEARTBEAT_LED);
            wait(0.1); //sec.
        
            GAPOC_GPIO_Set_Low(GAPOC_HEARTBEAT_LED);
            wait(LED_OFFTIME_NOM_ms/1000.0);  // slower blink rate at each iteration
        }

        // Relaunch RTC
        // (would be better to set it up once for all in repeat mode, but doesnt work...)
        //
        // First, stop timer in case it was already enabled earlier (typ when coming back from power down)
        RTC_Deinit(RTC_APB);


        wait(0.1); //TMP
        

         // Then relaunch RTC wake-up counter:
        pmu_wakeup_by_rtc();

                 
        // GO back to sleep :
        PORTA->SLEEP_PADCFG[0] = GAPOC_SLEEP_PADCFG_REG0;
        PORTA->SLEEP_PADCFG[1] = GAPOC_SLEEP_PADCFG_REG1;
        PORTA->SLEEP_PADCFG[2] = GAPOC_SLEEP_PADCFG_REG2;
        if (USE_PADSLEEP)
        {        
            PORTA->PAD_SLEEP = 0x01; 
        }
        else
        {        
            PORTA->PAD_SLEEP = 0x00; 
        }

 
#if SLEEP_MODE == DEEP
        PMU_StateSwitch(uPMU_SWITCH_DEEP_SLEEP, uPMU_SWITCH_FAST);  // 2nd argument is useless ?
#else        
        PMU_StateSwitch(uPMU_SWITCH_RETENTIVE_SLEEP, uPMU_SWITCH_FAST);  // 2nd argument is useless ?
#endif

        while(1);  // Stop here until power actually cut off 

    }

return 0;


}
