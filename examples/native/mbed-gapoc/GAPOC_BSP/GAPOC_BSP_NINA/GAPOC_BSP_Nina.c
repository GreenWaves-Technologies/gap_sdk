// File: GAPOC_BSP_Nina.c

// #############################################################################
// ##### DECLARATIONS  #########################################################

// ====  Handy "defines" for application, used locally   =======================

// <none>


// ====  Includes    ===========================================================

#include "GAPOC_BSP_Nina.h"

// ====  Imported Global Variables  ============================================


// ====  Exported Global Variables  ============================================


// ====  File-wide Variables  ===================================================

static uart_handle_t hUart_Tx, hUart_Rx;
static volatile uint32_t Tx_Index =0;
static volatile AT_Resp_State_t  AT_Resp_State;
static volatile bool New_RxByte = false;
static volatile unsigned char Uart_RxByte;
static bool Resp_OK = false;
static volatile char Rx_String[AT_RESP_ARRAY_LENGTH] ;
static    UART_Type *const uart_addrs[] = UART_BASE_PTRS;


// ====  Local helper functions  ================================================

//static void GAPOC_NINA_Send_AT_Blocking(const char* pCmd_Core);


// #############################################################################
// ##### FUNCTION DEFINITIONS        ###########################################



void GAPOC_NINA_AT_Uart_Init()
{
serial_t hUart;

    serial_init(&hUart, UART_TX, UART_RX);  // TX on pin A7, RX on B6 -- default config has pull-ups
    GAPOC_AnyPin_Config(UART_TX, NOPULL, uPORT_MuxAlt0);  // Remove pull-up on UART_TX
    GAPOC_AnyPin_Config(UART_RX, NOPULL, uPORT_MuxAlt0);  // Remove pull-up on UART_TX

    serial_format(&hUart, 8, 0, 1); // Update format -- 8 bits, No parity, 1 Stop bit for NINA

    serial_baud(&hUart, NINA_UART_AT_BAUDRATE_bps);   // Update baudrate for NINA

}
 
// -----------------------------------------------------


void GAPOC_NINA_Send_AT_Blocking(const char* pCmd_Core)
{

static uint8_t Cmd_string[AT_CMD_ARRAY_LENGTH];  
    // !!! BEWARE -  did'nt (always) without static keywork -- we're passing a pointer to a string that must stay alive in memory

   strcpy((char*)Cmd_string, (char*)"AT");
   strcat((char*)Cmd_string, (char*)pCmd_Core);
   strcat((char*)Cmd_string, (char*)S3str);

   // Now senc ommand over UART :
   UART_TransferSendBlocking(uart_addrs[0], (uint8_t *)Cmd_string, strlen((char *)Cmd_string));     
   
}

// -----------------------------------------------------


static  void Callback_Uart_SingleByte_Rx()
  // TODO: can callback function return some data, or use *UserData, to avoid using global Rx_String?
{
static uint32_t index = 0;
static unsigned char RxByte_Previous;

    New_RxByte = true;  

    if ((AT_Resp_State == AT_RESP_NOT_STARTED) &&
           (RxByte_Previous==S3char) && (Uart_RxByte==S4char))  
    {
        index = 0;
        AT_Resp_State = AT_RESP_IN_PROGRESS;
    }
    else if (AT_Resp_State == AT_RESP_IN_PROGRESS) 
    { 
        if ((RxByte_Previous==S3char) && (Uart_RxByte==S4char))  //end of response (1st part if multiple)
        {
            Rx_String[--index]='\0';  // obliterate final S3S4 and append end of string marker char
            AT_Resp_State = AT_RESP_DONE;
        }        
        else 
        {
            Rx_String[index++] =Uart_RxByte;            // Receive chars between leading S3S4 and 2nd S3SA
        }
    }

    RxByte_Previous = Uart_RxByte;


  }
  
// -----


uint32_t GAPOC_NINA_AT_Send( const char* pCmd_Core)   
    // Send AT command and receive any response(s) (OK or ERROR or Unsollicited or truncated stuff), char per char
    //   (!!Beware - means interrupts at rate =baudrate)
{

    AT_Resp_State = AT_RESP_NOT_STARTED;
    New_RxByte = false;
   
    UART_TransferCreateHandle(uart_addrs[0], &hUart_Rx, Callback_Uart_SingleByte_Rx, NULL  );  
    // TODO - can we pass &RxByte to callback fn using *UserData ?  How ??
    UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)&Uart_RxByte, 1);   

    GAPOC_NINA_Send_AT_Blocking(pCmd_Core);  // or better do non-blocking in case several responses received during Tx ?
    // NOTE: the '?' at the end of the AT Read Command is not added automatically; if needed it is expected to be last char of string provided


    enum {WR_RES_NA, WR_RES_OK, WR_RES_ERR, WR_RES_UNSOL} Write_Result = WR_RES_NA;
    while ( Write_Result==WR_RES_UNSOL  || Write_Result==WR_RES_NA )
    { 

        while(AT_Resp_State != AT_RESP_DONE)
        {
            if (New_RxByte)
            {
                // 1 new byte received and wasn't last one --> relaunch Rx of 1 byte
                New_RxByte = false;
                UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)&Uart_RxByte, 1); 
            }
        }   
        
        DBG_PRINT("Got Write Resp: " );
        DBG_PRINT("%s", Rx_String );
        DBG_PRINT("\n" );
                  
        // At this point we have received full response from NINA -- what is it ?
        
        uint32_t Last_Char_Pos = strlen((const char*)Rx_String) -1;  // final S3,S4 already discarded 
        if ( (Rx_String[Last_Char_Pos-1]=='O') && (Rx_String[Last_Char_Pos]=='K') )   // Last 2 useful chars = "OK"
        {
           Write_Result =WR_RES_OK; 
        }
        else if ( (Rx_String[Last_Char_Pos-4]=='E') && (Rx_String[Last_Char_Pos-3]=='R') && (Rx_String[Last_Char_Pos-2]=='R') 
                && (Rx_String[Last_Char_Pos-1]=='O') && (Rx_String[Last_Char_Pos]=='R')  )
        {
           DBG_PRINT("Error response from NINA\n");
           Write_Result =WR_RES_ERR; 
        }
        else
        {
           DBG_PRINT("Unsollicited/unrecognized response from NINA = %s\n", Rx_String);
           Write_Result =WR_RES_UNSOL; 
        }                 
                        
    }  
     
    
    if (Write_Result ==WR_RES_OK)
    {
        return 0;
    }
    else 
    {
        return -1;
    }    

 } 

// -----

void GAPOC_NINA_AT_Query( const char* pCmd_Core, char* Response_String)   
    // Send AT read command and receive  response, char per char
    //   (!!Beware - means interrupts at rate =baudrate)
{

    AT_Resp_State = AT_RESP_NOT_STARTED;
    New_RxByte = false;
   
    UART_TransferCreateHandle(uart_addrs[0], &hUart_Rx, Callback_Uart_SingleByte_Rx, NULL  );  
    // TODO - can we pass &RxByte to callback fn using *UserData ?  How ??
    UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)&Uart_RxByte, 1);   

    GAPOC_NINA_Send_AT_Blocking(pCmd_Core);  // or better do non-blocking in case several responses received during Tx ?
    // NOTE: the '?' at the end of the AT Read Command is not added automatically; if needed it is expected to be last char of string provided


    while(AT_Resp_State != AT_RESP_DONE)
    {
        if (New_RxByte)
        {
            // 1 new byte received and wasn't last one --> relaunch Rx of 1 byte
            New_RxByte = false;
            UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)&Uart_RxByte, 1); 
        }
     }   
     
    strcpy((char*)Response_String, (const char*)Rx_String);
    
    DBG_PRINT("Got Query Resp: " );
    DBG_PRINT("%s", Response_String );
    DBG_PRINT("\n" );

 } 

// -----

void GAPOC_NINA_AT_WaitForEvent(char* Response_String)   
    // Expect unsollicited response due to some even, receive it char per char
    //   (!!Beware - means interrupts at rate =baudrate)
{

    AT_Resp_State = AT_RESP_NOT_STARTED;
    New_RxByte = false;
   
    UART_TransferCreateHandle(uart_addrs[0], &hUart_Rx, Callback_Uart_SingleByte_Rx, NULL  );  

    UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)&Uart_RxByte, 1);   // receive first char
    while(AT_Resp_State != AT_RESP_DONE)
    {
        if (New_RxByte)
        {
            // 1 new byte received and wasn't last one --> relaunch Rx of 1 byte
            New_RxByte = false;
            UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)&Uart_RxByte, 1);  //subsequent char
        }
     }   
     
    strcpy((char*)Response_String, (const char*)Rx_String);
    
    DBG_PRINT("Got Unsol. Resp: " );
    DBG_PRINT("%s", Response_String );
    DBG_PRINT("\n" );
 } 


// -----

void GAPOC_NINA_Send_ByteArray_Blocking(const uint8_t* pArray, uint32_t Num_Items)
{
    UART_TransferSendBlocking(uart_addrs[0], pArray, Num_Items);     
}  


// -----

void GAPOC_NINA_Get_ByteArray_NonBlocking(const uint8_t* pArray, uint32_t Num_Items,  void(*Callback)() )
{

    UART_TransferCreateHandle(uart_addrs[0], &hUart_Rx, Callback, NULL  );  
    UART_TransferReceiveNonBlocking(uart_addrs[0], &hUart_Rx, (unsigned char*)pArray, Num_Items);   // receive first char
}

// ===  Local Helper Functions ===================================================================================




// **** END OF FILE ***********************************************************************************************


