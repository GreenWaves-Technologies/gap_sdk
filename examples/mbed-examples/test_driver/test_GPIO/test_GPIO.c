#include "cmsis.h"
#include "gap_common.h"
#include "mbed_wait_api.h"

/* Variables used. */
/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

int main( void )
{
    printf("Fabric controller code execution for mbed_os GPIO \n");

    PinName led = GPIO_A6;

    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT(led);
    uint32_t gpio_number = GET_GPIO_NUM(led);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(led);

    PORT_SetPinMux(port_addrs[port_number], pin_number,  uPORT_MuxGPIO);

    /* Init GPIO - OUTPUT. */
    gpio_pin_config_t gpio_config = { .pinDirection  = uGPIO_DigitalOutput,
                                      .outputLogic   = uGPIO_LOW,
                                      .pullSelect    = uGPIO_PullUpEnable,
                                      .driveStrength = uGPIO_LowDriveStrength,
                                    };

    GPIO_PinInit ( gpio_addrs[port_number], gpio_number, &gpio_config );

    /* blink */
    for(int i = 0; i < 10; i++)
    {
        /* Write 1 to GPIO. */
        GPIO_WritePinOutput( gpio_addrs[port_number], gpio_number, uGPIO_HIGH );
        printf("Led = %d\n", GPIO_ReadPinInput( gpio_addrs[port_number], gpio_number ));

        /* Wait 10 ms */
        wait(0.01);

        /* Write 0 to GPIO. */
        GPIO_WritePinOutput( gpio_addrs[port_number], gpio_number, uGPIO_LOW );
        printf("Led = %d\n", GPIO_ReadPinInput( gpio_addrs[port_number], gpio_number ));

        /* Wait 10 ms */
        wait(0.01);
    }
}
