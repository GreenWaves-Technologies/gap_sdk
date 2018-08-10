#include "cmsis.h"
#include "gap_common.h"
#include "mbed_wait_api.h"

#define   TRIGGER_TIMES     10

static int count = 0;

/* Variables used. */
/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

void gpio_handler() {
    printf("GPIO triggered times = %d!\n", count++);
}

int main( void )
{
    printf("Fabric controller code execution for mbed_os GPIO Interrupt IN \n");

    PinName button = GPIO_A2_A2;

    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT   (button);
    uint32_t gpio_number = GET_GPIO_NUM    (button);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(button);

    /* Init PORT - MUX as GPIO/Alter1. */
    port_pin_config_t port_config = { .pullSelect    = uPORT_PullUpEnable,
                                      .driveStrength = uPORT_LowDriveStrength,
                                      .mux           = uPORT_MuxAlt1};

    PORT_SetPinConfig  ( port_addrs[port_number], pin_number,  &port_config);

    /* Init GPIO - INPUT. */
    gpio_pin_config_t gpio_config = { .pinDirection  = uGPIO_DigitalInput,
                                      .outputLogic   = uGPIO_LOW };

    GPIO_PinInit       ( gpio_addrs[port_number], gpio_number, &gpio_config );

    /* Bind GPIO IRQ handler */
    GPIO_IRQHandlerBind(gpio_addrs[port_number], (uint32_t)gpio_handler);

    /* GPIO IRQ enable */
    SOC_EU_SetFCMask(GPIO_EVENT);

    /* GPIO trigger IRQ by falling edge */
    GPIO_SetPinInterruptConfig(gpio_addrs[port_number], gpio_number, uGPIO_InterruptFallingEdge);

    while( count < TRIGGER_TIMES) {
        printf("Waiting...\n");

        wait(1);
    }

    return 0;
}
