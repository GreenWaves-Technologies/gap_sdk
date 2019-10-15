// File GAPOC_BSP_Pin_services.h

#ifndef  __GAPOC_BSP_PIN_SERVICES_H
#define  __GAPOC_BSP_PIN_SERVICES_H  // prevent multiple inclusions
// ##########################################################################################


// ==  Includes relied upon in this file   ================================================
#include "GAPOC_BSP_Platform_Defs.h"


// ==========  Custom Types  ==============================================================

typedef enum  _PullEn {NOPULL=0, PULL=1}  GAPOC_PullEn_t;


// ==========  Public Defines  ============================================================
// <none>

    
// ==========   USEFUL PUBLIC MACROS   ====================================================

#define  DBG_PRINT(fmt, ...)  \
            do { if (GAPOC_DEBUG) printf(fmt, ##__VA_ARGS__); } while(0)


      
// ==========  Public Functions Prototypes        =========================================

void GAPOC_GPIO_Init_Pure_Output_Low(PinName GPIO_Name);
void GAPOC_GPIO_Init_Pure_Output_High(PinName GPIO_Name);
void GAPOC_GPIO_Init_HighZ(PinName GPIO_Name); // In High-Z mode, pin is configured as input but GPIO clock is not enabled (to save power) so pin data can't be read
void GAPOC_GPIO_Init_JustPull(PinName GPIO_Name);
void GAPOC_GPIO_Init_Input_Float(PinName GPIO_Name);
void GAPOC_GPIO_Init_Input_Pull(PinName GPIO_Name);
void GAPOC_GPIO_Init_Bidir_Low(PinName GPIO_Name);
void GAPOC_GPIO_Init_Bidir_High(PinName GPIO_Name);

//int GAPOC_BSP_Board_Init(void);
    
    
// ==========  Public In-lined Functions          =========================================


// Constants (register addresses) used by most of these functions
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
// constant array containing a structure of type GPIO_Type, its first element is at address GPIO_BASE_PTRS
// (which is the base address of GPIO control registers)
// Note - the GPIO type structure holds 16x 32-bit registers i.e. 64 bytes -- OK to keep statically in memory


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

/** Function : GAPOC_AnyPin_Config 
    Action : For any pin (GPIO-capable or not) 'Name_Of_Pin', define its function ( Alternate0 a.k.a default / Alternate1 a.k.a GPIO if available /
                Alternate2 if available / etc.) through parameter Alternate_Fn_Id and define if the pull resistor (pull-down in most cases,
                pull-up for some pins, see DS) shall be activated or not
*/
// PinName and PinMode used below are enumerated types with valid values listed in PinNames.h

static inline void GAPOC_AnyPin_Config(PinName Name_Of_Pin, GAPOC_PullEn_t PullEn, port_mux_t Alternate_Fn_Id )
{
    // Enable internal pull-up or pull-down if required :
    pin_mode(Name_Of_Pin, (PinMode) PullEn);  
        // Function pin_mode() part of pinmap.c API -- 
        // It accepts both pins axpressed as e.g. A16 or, when it exists, its GPIO name counterpart e.g. GPIO_A31
        // The possible (enumerated) values of the Pull_En parameter in this function are a bit weird as they
        // call PullUp somehting that actually enables in the chip a pull resistor that can pull either low (most cases)
        // or high (for a few pins e.g. UART). But remapping a boolean PullEn value works well and presents a more
        // sensible naming
        
        
    // Set pin function (main or alternate functions -- alternate_fn_id=0 for main, 1 for alternate-1, etc.) :
    pin_function(Name_Of_Pin, (int) Alternate_Fn_Id);
       // Function function() part of pinmap.c API -- we make sure the parameter passed as alternate_fn_id is 0 to 3 by using
       // enum type port_mux_t, although pin_function() simply requires an integer (but expects it to be between 0 and 3!)
}

// ---------------------------------------------------- 

// TODO - here and below,
//        use an assert functoin that wont generate actual code in release version to check IS_GPIO

/** Function : GAPOC_GpioPin_As_Input 
    Action : Configures GPIO 'GPIO_Name' as input 
    The pin must have been set as Alternate-1 to behave as GPIO first
*/

static inline uint8_t GAPOC_GpioPin_As_Input(PinName GPIO_Name )
{
    if (GET_IS_GPIO(GPIO_Name))
    {
       GPIO_SetPinDirection(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name), uGPIO_DigitalInput);
            // using API from gap_gpio.h
            
       // !!! Need to also enable the GPIO clock to be able to actually read back input data:
       gpio_addrs[GET_GPIO_PORT(GPIO_Name)]->EN |= (1U << GET_GPIO_NUM(GPIO_Name));
       return 0;
    }
    else 
    {
        DBG_PRINT("Use the GPIO name, i.e. GPIO_Axx_xx \n");
        return -1;
    }
}

// ---------------------------------------------------- 

/** Function : GAPOC_GpioPin_As_HighZ 
    Action : Configures GPIO 'GPIO_Name' as high-impedance (same as input but data is not clocked in) 
    The pin must have been set as Alternate-1 to behave as GPIO first
*/

static inline uint8_t GAPOC_GpioPin_As_HighZ(PinName GPIO_Name )
// This is putting pin in input mode, but with GPIO clock disabled
// to save power (pin can't actually be read back then)
{
    if (GET_IS_GPIO(GPIO_Name))
    {
       GPIO_SetPinDirection(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name), uGPIO_DigitalInput);
            // using API from gap_gpio.h
            
       // disconnect any pull-up/down:
       pin_mode(GPIO_Name, (PinMode) NOPULL);  
            
       // Disable the GPIO clock 
       gpio_addrs[GET_GPIO_PORT(GPIO_Name)]->EN &= ~(1U << GET_GPIO_NUM(GPIO_Name));
       return 0;
    }
    else 
    {
        DBG_PRINT("Use the GPIO name, i.e. GPIO_Axx_xx \n");
        return -1;
    }
}

// ------------------------------------------------------

/** Function : GAPOC_GpioPin_As_Pure_Output 
    Action : Configures GPIO 'GPIO_Name' as pure output (input data is not clocked in) 
    The pin must have been set as Alternate-1 to behave as GPIO first
*/

static inline uint8_t GAPOC_GpioPin_As_Pure_Output(PinName GPIO_Name )
// NOTE: even if the pad is bidir, GPIO clock is not enabled here (to save power) so pin value cannot be read back 
{
    if (GET_IS_GPIO(GPIO_Name))
    {
       GPIO_SetPinDirection(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name), uGPIO_DigitalOutput);
            // using API from gap_gpio.h

       // Disable the GPIO clock to save power 
       gpio_addrs[GET_GPIO_PORT(GPIO_Name)]->EN &= ~(1U << GET_GPIO_NUM(GPIO_Name));
       return 0;
    }
    else 
    {
        DBG_PRINT("Use the GPIO name, i.e. GPIO_Axx_xx \n");
        return -1;
    }
}

// ------------------------------------------------------

/** Function : GAPOC_GpioPin_As_Bidir 
    Action : Configures GPIO 'GPIO_Name' as input-output  
    The pin must have been set as Alternate-1 to behave as GPIO first
*/

static inline uint8_t GAPOC_GpioPin_As_Bidir(PinName GPIO_Name )
// NOTE: pad is put in output mode but GPIO clock is enabled so pin value can also be read back 
{
    if (GET_IS_GPIO(GPIO_Name))
    {
       GPIO_SetPinDirection(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name), uGPIO_DigitalOutput);
            // using API from gap_gpio.h

       // Enable the GPIO clock  
       gpio_addrs[GET_GPIO_PORT(GPIO_Name)]->EN |= (1U << GET_GPIO_NUM(GPIO_Name));
       return 0;
    }
    else 
    {
        DBG_PRINT("Use the GPIO name, i.e. GPIO_Axx_xx \n");
        return -1;
    }
}
// ------------------------------------------------------

/** Function : GAPOC_GPIO_Is_High 
    Action : Returns true if GPIO_Name is at high logic level, else false
    The pin must have been initialized as GPIO first, in input or bidir mode, eg. using GAPOC_GPIO_Init_xxx(xx)
    // !!! *BEWARE* if pin is bidir i.e. both input and output functions are enabled,
    // !!!  then GPIO_ReadPinInput() returns the *programmed output value* rather than the actual value present
    // !!!  on pin -- so does not reflect reality in case of external driver overrriding the value on external pin
*/

static inline bool GAPOC_GPIO_Is_High(PinName GPIO_Name)
{
    if ( GPIO_ReadPinInput(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name) ) ==1 )
        return true;
    else
        return false;
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Set_High 
    Action : Sets 'GPIO_Name' to high logic level  
    The pin must have been initialized as GPIO first, in output or bidir mode, eg. using GAPOC_GPIO_Init_xxx(xx)
*/

static inline void GAPOC_GPIO_Set_High(PinName GPIO_Name)
{
    GPIO_WritePinOutput(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name), 1);
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Set_Low 
    Action : Sets 'GPIO_Name' to low logic level  
    The pin must have been initialized as GPIO first, in output or bidir mode, eg. using GAPOC_GPIO_Init_xxx(xx)
*/

static inline void GAPOC_GPIO_Set_Low(PinName GPIO_Name)
{
    GPIO_WritePinOutput(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], GET_GPIO_NUM(GPIO_Name), 0);
}

// ------------------------------------------------------

/** Function : GAPOC_GPIO_Toggle 
    Action : Toggle 'GPIO_Name'  
    The pin must have been initialized as GPIO first, in output or bidir mode, eg. using GAPOC_GPIO_Init_xxx(xx)
*/

static inline void GAPOC_GPIO_Toggle(PinName GPIO_Name)
{
    GPIO_TogglePinsOutput(gpio_addrs[GET_GPIO_PORT(GPIO_Name)], 1U<<GET_GPIO_NUM(GPIO_Name));  
}


         


#endif   //####  __GAPOC_BSP_PIN_SERVICES_H  #####################################################
