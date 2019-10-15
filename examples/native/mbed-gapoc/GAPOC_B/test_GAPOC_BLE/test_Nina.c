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



// ====  Includes    ==========================================================

#include "GAPOC_BSP_General.h"

#include "GAPOC_BSP_Nina.h"

#include "mbed_wait_api.h"


// ====  Application's Custom Types    ========================================
// <None>


// ==== Application's exported global variables  ==============================
// <None>


// ==== Application's own global variables  ====================================

volatile bool RxData_Rcvd = false;

#define BLE_RXDATA_NUM_BYTES    3
#define BLE_TXDATA_NUM_BYTES    3
GAP_L2_DATA  uint8_t RxData[BLE_RXDATA_NUM_BYTES];    
GAP_L2_DATA  uint8_t Tx_Array[BLE_TXDATA_NUM_BYTES] ={0};



// ==== Application's Function Prototypes    ===================================

static void Callback_RxData();



// #############################################################################
// #####     MAIN APPLICATION        ###########################################

int main()
{


GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
char Resp_String[AT_RESP_ARRAY_LENGTH] ;


    DBG_PRINT("GAP Nina test\n");
        
    // Initalize Board (GPIO direction and default level, supplies, etc.)
    GAPOC_BSP_Board_Init();
    
    // NOTICE:
    // With current silicon there may be problems to use UART Rx (GAP8 receiving) while HyperBus interface is
    // enabled. To use UART Rx, pin B7 (=HYPER_DQ[6] when used as HyperBus I/O) must be configured in its default functionality (Alt.0),
    // which means HyperBus is not usable at this time.
    // In other words, usage of HyperBus and UART Rx must be time multiplexed, toggling the functionality of pin B7.
    
    // To limit power consumption from HyperMem without initializing HyperBus interface, 
    // you may pull its nCS low (inactive) by using GPIO mode, e.g. as follows:
    //GAPOC_GPIO_Init_Pure_Output_High(GPIO_A30);  // CSN0 = GPIO30 on B15
    //GAPOC_GPIO_Init_Pure_Output_High(GPIO_A31);  // CSN1 = GPIO31 on A16

    // Bug work-around (see above):
    // Set pin B7 for default behavior (HyperBus not usable then)
    GAPOC_AnyPin_Config( B7, NOPULL, uPORT_MuxAlt0 );  // pin GAP_B7 keeps default function = SPIM0_SCK (output)


    // --  Initialize UART, no pull   -----
    GAPOC_NINA_AT_Uart_Init();


    // --  Start NINA-B1 BLE module   -----
          
    // Make sure NINA_SW1 is HIGH at BLE Start-up -- GPIO_LED_G/NINA_SW1 = GPIO_A1_B2 on GAPOC
    // GAPOC_GPIO_Init_Pure_Output_High(GPIO_A1_B2);

    // Init GPIO that will control NINA DSR in deasserted position
    GAPOC_GPIO_Init_Pure_Output_Low(GAPOC_NINA17_DSR);

    
    // Enable BLE (release reset)
    GAPOC_GPIO_Set_High(GAPOC_NINA_NRST);  

   
    wait(1); // some waiting needed after BLE reset... 
 
    // Now release GPIO_LED_G/NINA_SW1 so it can be driven by NINA
    GAPOC_GPIO_Init_HighZ(GPIO_A1_B2);   


    // Initiliaze NINA as BLE Peripheral
    GAPOC_NINA_AT_Send("E0");                   // Echo OFF    

    //GAPOC_NINA_AT_Query("+CGMI", Resp_String);            
    GAPOC_NINA_AT_Send("+UFACTORY");            // Restore factory defined configuration
    GAPOC_NINA_AT_Send("+UBTUB=FFFFFFFFFFFF");  // Unbond all devices
    GAPOC_NINA_AT_Send("+UBTLE=2");             // BLE Role = Peripheral
    GAPOC_NINA_AT_Send("+UBTLN=GreenWaves-GAPOC");         // Set Local Bluetooth Name
    //GAPOC_NINA_AT_Send("+UBTLECFG=1,480");      // BLE Configuration Param#1 =  Min adv. interval = 480x625ns
    //GAPOC_NINA_AT_Send("+UBTLECFG=2,640");      // BLE Configuration Param#2 =  Max adv. interval = 640x625ns

 
    DBG_PRINT("AT Config Done\n");
             
    // After Reboot of NINA,  central connects to NINA and NINA will provide
    // unsollicited AT event: +UUBTACLC:<peer handle,0,<remote BT address>)
    // (...but sometimes just provides empty event instead !?)
    
    // Just make sure NINA sends something as AT unsolicited response, therefore is ready :
    GAPOC_NINA_AT_WaitForEvent(Resp_String);
    DBG_PRINT("Received Event after reboot\n");


    // Enter Data Mode
    GAPOC_NINA_AT_Send("O");                   // AT Command to enter data mode   
    DBG_PRINT("Data Mode Entered!\n");


    wait(1); // leave some time for Central to be properly configured 
     
     
     
    // ***************************************************************************
    // SPS Service on NINA module seems to work unrilably in duplex mode  ???
    // --> Stick to unidirectional usage
#define SPS_TX_NRX  1    // set to 1 to select SPS transmission, 0 for reception
    // ***************************************************************************
    

#if SPS_TX_NRX==0               
    // If (simplex) Reception Mode selected,  
    // Prepare for reception of xx bytes from UART (<< BLE) at any time
    GAPOC_NINA_Get_ByteArray_NonBlocking(RxData, BLE_RXDATA_NUM_BYTES, Callback_RxData);   
#endif
    // ----------------------------------------------------------            
 
            

    #define LED_OFF_DURATION_sec  0.2   
    #define NB_ITERATIONS  10  
    #define DELAY_BETWEEN_ITERATIONs_MS     1000
    uint8_t j =0;
    
    while(1) 
    // for (uint8_t k=0; k<NB_ITERATIONS; k++)
    {
         
        // --    Briefly flash Green LED .....  --------------         
        GAPOC_GPIO_Set_High(GAPOC_HEARTBEAT_LED);
        wait(0.1); 
        
        GAPOC_GPIO_Set_Low(GAPOC_HEARTBEAT_LED);
        wait(LED_OFF_DURATION_sec);    //was 1.9
        
        
        // --    ...and then....                --------------         
        
#if   SPS_TX_NRX==1     // Transmission

        // --    If (simplex) Transmission Mode selected,  ---- 
        // --    send xx bytes over UART (>> BLE) now      ----
        for (uint8_t i=0; i<sizeof(Tx_Array); i++)
        {
            Tx_Array[i]=j++%10;
        }            
        GAPOC_NINA_Send_ByteArray_Blocking( Tx_Array, sizeof(Tx_Array) );
            
#elif SPS_TX_NRX==0     // Reception      
       
        // --    If (simplex) Reception Mode selected,      ---- 
        // --    display xx bytes when received from UART (<< BLE)               
        if (RxData_Rcvd)
        {
            for (uint8_t j=0; j<BLE_RXDATA_NUM_BYTES; j++)
            {
                putchar(RxData[j]+0x30);
            }
            putchar('\n');
          
            // Relaunch reception of xx bytes :
            RxData_Rcvd = false;           
            GAPOC_NINA_Get_ByteArray_NonBlocking(RxData, BLE_RXDATA_NUM_BYTES, Callback_RxData);   
        }         
        
#else
  #error "SPS_TX_NRX not properly set"
#endif


        wait (DELAY_BETWEEN_ITERATIONs_MS/1000.0);

    }
     
    return 0; 
 
}


// #############################################################################
// ##### LOCAL FUNCTION DEFINITIONS  ###########################################

static void Callback_RxData()
{
    RxData_Rcvd = true;
}



// ## END OF FILE ##############################################################################


