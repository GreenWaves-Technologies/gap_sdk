#include "cmsis.h"
#include "gap_common.h"
// SPI CMSIS driver
#include "spi_multi_api.h"
#include "mbed_wait_api.h"

/* Variables used. */
/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

spi_command_sequence_t s_command;

uint32_t reg;

static uint32_t read_register(spi_t *spim, uint8_t cmd, int size)
{
    memset(&s_command, 0, sizeof(spi_command_sequence_t));
    s_command.cmd       = cmd;
    s_command.cmd_bits  = 8;
    s_command.cmd_mode  = uSPI_Single;
    s_command.rx_bits   = (size << 3);
    s_command.rx_buffer = (uint8_t *)&reg;
    s_command.data_mode = uSPI_Single;

    spi_master_transfer_command_sequence(spim, &s_command);

    return reg;
}

void send_boot_signal()
{
    spi_t spim;
    /* SPI pins init, SPI udma channel init */
    spi_init(&spim, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CSN0);

    /* SPI bits, cpha, cpol configuration */
    spi_format(&spim, 8, 0, 0);

    /* Set fequence to 10MHz */
    spi_frequency(&spim, 10000000);

    /* Read ID in single mode */
    int id = read_register(&spim, 0x9f, 4);
    printf("Boot End\n");
}

void test_gpio_init (PinName pin, gpio_pin_direction_t direction)
{
    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT(pin);
    uint32_t gpio_number = GET_GPIO_NUM(pin);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(pin);

    PORT_SetPinMux(port_addrs[0], pin_number,  uPORT_MuxGPIO);

    /* Init GPIO - OUTPUT. */
    gpio_pin_config_t gpio_config = { .pinDirection  = direction,
                                      .outputLogic   = uGPIO_LOW,
                                      .pullSelect    = uGPIO_PullUpEnable,
                                      .driveStrength = uGPIO_LowDriveStrength,
    };

    GPIO_PinInit ( gpio_addrs[port_number], gpio_number, &gpio_config );
}

void test_gpio_write (PinName pin, gpio_pin_state_t state)
{
    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT(pin);
    uint32_t gpio_number = GET_GPIO_NUM(pin);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(pin);

    /* Write 1 to GPIO. */
    GPIO_WritePinOutput( gpio_addrs[port_number], gpio_number, state );
    printf("Led[%d] = %d\n", pin_number, GPIO_ReadPinInput( gpio_addrs[port_number], gpio_number ));
}

void test_gpio_read (PinName pin)
{
    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT(pin);
    uint32_t gpio_number = GET_GPIO_NUM(pin);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(pin);

    /* Write 1 to GPIO. */
    printf("Button[%d] = %d\n", pin_number, GPIO_ReadPinInput( gpio_addrs[port_number], gpio_number ));
}

int main( void )
{
    printf("Fabric controller code execution for mbed_os GPIO \n");

    /*
     * Notify testbench to pad muxing dunamically yo SDIO
     * This fucntion should hided in Boot Code or crt0.c
     */
    send_boot_signal();

    #define TEST_TIME 2

    /* 1. LED OUT */
    for(int i = 0; i < 32; i++)
    {
        test_gpio_init(GPIO_A[i], uGPIO_DigitalOutput);
        test_gpio_init(GPIO_B[i], uGPIO_DigitalOutput);
    }

    /* blink */
    for(int n = 0; n < TEST_TIME; n++)
    {
        for(int i = 0; i < 32; i++)
        {
            test_gpio_write (GPIO_A[i], uGPIO_HIGH);
            test_gpio_write (GPIO_B[i], uGPIO_HIGH);

        }
        /* Wait 10 ms */
        wait(0.001);

        for(int i = 0; i < 32; i++)
        {
            test_gpio_write (GPIO_A[i], uGPIO_LOW);
            test_gpio_write (GPIO_B[i], uGPIO_LOW);
        }
        /* Wait 10 ms */
        wait(0.001);
    }


    /* 2. BUTTON IN */
    for(int i = 0; i < 32; i++)
    {
        test_gpio_init(GPIO_A[i], uGPIO_DigitalInput);
        test_gpio_init(GPIO_B[i], uGPIO_DigitalInput);
    }

    /* Read BUTTON */
    for(int n = 0; n < TEST_TIME; n++)
    {
        for(int i = 0; i < 32; i++)
        {
            test_gpio_read (GPIO_A[i]);
            test_gpio_read (GPIO_B[i]);

        }
        /* Wait 10 ms */
        wait(0.001);

        for(int i = 0; i < 32; i++)
        {
            test_gpio_read (GPIO_A[i]);
            test_gpio_read (GPIO_B[i]);
        }
        /* Wait 10 ms */
        wait(0.001);
    }

    return 0;
}
