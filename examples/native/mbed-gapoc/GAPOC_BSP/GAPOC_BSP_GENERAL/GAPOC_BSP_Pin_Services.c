// File: GAPOC_BSP_Pin_Services.c

// #############################################################################
// ##### DECLARATIONS  #########################################################

// ====  Handy "defines" for application, used locally   =======================
// <none>


// ====  Includes    ===========================================================

#include "GAPOC_BSP_gap8.h"
#include "GAPOC_BSP_Pin_Services.h"


// ====  Imported Global Variables  ============================================
// <none>


// ====  Exported Global Variables  ============================================
// <none>


// ====  File-wide Variables  ===================================================

/*  (Moved to .h file as used by in-lined functions)
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
// constant array containing a structure of type GPIO_Type, its first element is at address GPIO_BASE_PTRS
// (which is the base address of GPIO control registers)
// Note - the GPIO type structure holds 16x 32-bit registers i.e. 64 bytes -- OK to keep statically in memory
*/          
            
            
// ####   USEFUL PUBLIC FUNCTIONS  #############################################
            
// ---  I/O pin control   --------------------------------------------  

// See GAPOC_BSP_Services.h describing a bunch of in-lined functions


// ---------------------------------------------------------------------------- 
// Note on GPIO-style name of pins (see also PinNames.h)  :
// GPIO Pin Name (e.g. GPIO_A2_A5) is an alias for value built as follows:    
//  [1bit]   |         [8bits]         |     [4bits]      |          [8bits]          |
// IS_GPIO   |       GPIO NUM.         |    PORT NUM      |        PAD NUM            |
//           | (if is gpio else n/a)   |(always 0 in GAP8)|  (and PIN_NUM=PAD_NUM-8)  |
// ---------------------------------------------------------------------------- 


// ==========  NOTES         ==============================================================

// Valid pin names and where relevant valid aliases such as GPIO names can be found in file:
//   xxx/gap_sdk/mbed-os/targets/TARGET_GWT/TARGET_GAP8/pins/PinNames.h

// Valid alternate function IDs are: 
//    uPORT_MuxAlt0 = 0U,           /*!< Default */
//    uPORT_MuxAlt1 = 1U,           /*!< Corresponding pin is configured as GPIO. */
//    uPORT_MuxGPIO = uPORT_MuxAlt1,/*!< Corresponding pin is configured as GPIO. */
//    uPORT_MuxAlt2 = 2U,           /*!< Chip-specific */
//    uPORT_MuxAlt3 = 3U,           /*!< Chip-specific */

// ========================================================================================


/** Function : GAPOC_GPIO_Init_Pure_Output_Low 
    Action : Configures pin 'GPIO_Name' as GPIO in pure output mode and initializes it to low logic level
*/

void GAPOC_GPIO_Init_Pure_Output_Low(PinName GPIO_Name)
{
    GAPOC_AnyPin_Config(GPIO_Name, NOPULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GPIO_Set_Low(GPIO_Name);
    GAPOC_GpioPin_As_Pure_Output(GPIO_Name);
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_Pure_Output_High 
    Action : Configures pin 'GPIO_Name' as GPIO in pure output mode and initializes it to high logic level
*/

void GAPOC_GPIO_Init_Pure_Output_High(PinName GPIO_Name)
{
    GAPOC_AnyPin_Config(GPIO_Name, NOPULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GPIO_Set_High(GPIO_Name);
    GAPOC_GpioPin_As_Pure_Output(GPIO_Name);
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_HighZ 
    Action : Configures pin 'GPIO_Name' as GPIO in high-impedance mode 
*/

// In High-Z mode, pin is configured as input but GPIO clock is not enabled (to save power) so pin data can't be read
void GAPOC_GPIO_Init_HighZ(PinName GPIO_Name) 
{
    GAPOC_AnyPin_Config(GPIO_Name, NOPULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GpioPin_As_HighZ(GPIO_Name);    
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_JustPull 
    Action : Configures pin 'GPIO_Name' as GPIO in high-impedance mode with weak pull-up/down enabled.
             This is same as High-Z mode (GPIO input data capture clock off), except pin is pulled rather than left floating
*/

void GAPOC_GPIO_Init_JustPull(PinName GPIO_Name) 
{
    GAPOC_AnyPin_Config(GPIO_Name, PULL, uPORT_MuxGPIO );  // set pin as GPIO with pull-up/down
    GAPOC_GpioPin_As_HighZ(GPIO_Name);    
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_Input_Float 
    Action : Configures pin 'GPIO_Name' as GPIO in floating input mode (i.e. w/o pull-up/down)
*/

void GAPOC_GPIO_Init_Input_Float(PinName GPIO_Name) 
{
    GAPOC_AnyPin_Config(GPIO_Name, NOPULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GpioPin_As_Input(GPIO_Name);
}
// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_Input_Pull 
    Action : Configures pin 'GPIO_Name' as GPIO in input mode, with pull-up/down resistor enabled
*/

void GAPOC_GPIO_Init_Input_Pull(PinName GPIO_Name) 
{
    GAPOC_AnyPin_Config(GPIO_Name, PULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GpioPin_As_Input(GPIO_Name);
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_Bidir_High 
    Action : Configures pin 'GPIO_Name' as GPIO in bidirectional mode and initializes it to high logic level
*/

void GAPOC_GPIO_Init_Bidir_High(PinName GPIO_Name)
{
    GAPOC_AnyPin_Config(GPIO_Name, NOPULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GPIO_Set_High(GPIO_Name);
    GAPOC_GpioPin_As_Bidir(GPIO_Name);
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Init_Bidir_Low 
    Action : Configures pin 'GPIO_Name' as GPIO in bidirectional mode and initializes it to low logic level
*/

void GAPOC_GPIO_Init_Bidir_Low(PinName GPIO_Name)
{
    GAPOC_AnyPin_Config(GPIO_Name, NOPULL, uPORT_MuxGPIO );  // set pin as GPIO with no pull-up/down
    GAPOC_GPIO_Set_Low(GPIO_Name);
    GAPOC_GpioPin_As_Bidir(GPIO_Name);
}


 // TODO - Cope with GPIO as IRQ source
 
 


// ###   END OF FILE  ##################################################################################################################"""
