// File GAPOC_BSP_Nina.h
#ifndef  __GAPOC_BSP_NINA_H
#define  __GAPOC_BSP_NINA_H  // prevent multiple inclusions


// ==  Includes relied upon in this file   ========================================
#include "GAPOC_BSP_General.h"


// ==  Defines Useful to this Module       ========================================

#define S3char  '\r'     // = ASCII 0x0D, factory default S3 response formatting character for NINA Module =CR (see AT commands spec)
#define S4char  '\n'     // = ASCII 0x0A, factory default S4 response formatting character for NINA Module =LF (see AT commands spec)   
// Same, but to use in string functions (strcmp etc.) :  
#define S3str  "\r"      
#define S4str  "\n"    

#define  AT_CMD_ARRAY_LENGTH       32   //arbitrary limit
#define  AT_RESP_ARRAY_LENGTH      64   //arbitrary limit


// ==  Custom Types  ==============================================================
typedef  enum _AT_Resp_State
 {  AT_RESP_NOT_STARTED,
    AT_RESP_IN_PROGRESS,
    AT_RESP_DONE 
 }   AT_Resp_State_t;
    
    
// ==  Public Functions Prototypes        ============================================
    
void GAPOC_NINA_AT_Uart_Init();

void GAPOC_NINA_Send_AT_Blocking(const char* pCmd_Core);

uint32_t GAPOC_NINA_AT_Send( const char* pCmd_Core);       

void GAPOC_NINA_AT_Query( const char* pCmd_Core, char* Response_String);

void GAPOC_NINA_AT_WaitForEvent(char* Response_String);  

void GAPOC_NINA_Send_ByteArray_Blocking(const uint8_t* pArray, uint32_t Num_Items);

void GAPOC_NINA_Get_ByteArray_NonBlocking(const uint8_t* pArray, uint32_t Num_Items,  void(*Callback)() );


#endif //__GAPOC_BSP_NINA_H
