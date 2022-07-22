#include "stdio.h"
/* PMSIS includes. */
#include "pmsis.h"
#include "bsp/bsp.h"

#define AT_RESP_ARRAY_LENGTH       ( 64 )

static struct pi_device ble;

unsigned char action = 0;

#define TEST_MESSAGE_SIZE (200)
uint8_t snd_message[TEST_MESSAGE_SIZE];
uint8_t rcv_message[TEST_MESSAGE_SIZE];

static uint8_t rx_buffer[AT_RESP_ARRAY_LENGTH];

void init_snd_message(void)
{
    for (size_t i = 0; i < TEST_MESSAGE_SIZE; i++)
    {
        snd_message[i] = i & 0xFF;
    }
}

void verify_rcv_message(void)
{
    int error = 0;
    for (size_t i = 0; i < TEST_MESSAGE_SIZE; i++)
    {
        if (snd_message[i] != rcv_message[i])
        {
            printf("Received incorrect response\n");
            printf("[%d] Expected %x, got %x\n", i, snd_message[i], rcv_message[i]);
            error = 1;
        }
    }
    if (error != 0)
    {
            pmsis_exit(-1);
    }
}

void test_ble(void)
{
#ifdef __PLATFORM_RTL__
    pi_pad_set_mux_group(PI_PAD_067, PI_PAD_MUX_GROUP_UART1_CTS);
    pi_pad_set_mux_group(PI_PAD_068, PI_PAD_MUX_GROUP_UART1_RTS);
#endif

    int32_t errors = 0;
    struct pi_nina_b112_conf conf = {0};
    pi_ble_nina_b112_conf_init(&ble, &conf);
    pi_open_from_conf(&ble, &conf);
    errors += pi_ble_open(&ble);
    if (errors)
    {
        printf("Failed to open nina ble %ld\n", errors);
        pmsis_exit(-1);
    }
    printf("BLE UART init done\n");

    errors += pi_ble_ioctl(&ble, PI_NINA_B112_CLIENT_CONFIGURE, NULL);

    /* Retrieve BLE module name. */
    pi_ble_at_cmd(&ble, "+UBTLN?", (void *) rx_buffer, 1);
    printf("BLE name: %s\n", rx_buffer);

    /* reconfigure serial parameters */
    conf.baudrate = 230400;
    /* Not working */
    //conf.baudrate = 1000000;
    //conf.baudrate = 460800;
    pi_ble_ioctl(&ble, PI_NINA_B112_UART_CONFIGURE, &conf);

    /* Retrieve BLE HCI configuration. */
    pi_ble_at_cmd(&ble, "+UMRS?", (void *) rx_buffer, 1);
    printf("BLE UART settings: %s\n", rx_buffer);

    // After Reboot of NINA,  central connects to NINA and NINA will provide
    // unsollicited AT event: +UUBTACLC:<peer handle,0,<remote BT address>)
    // (...but sometimes just provides empty event instead !?)

    // Just make sure NINA sends something as AT unsolicited response, therefore is ready :
#ifndef __PLATFORM_RTL__
    printf("Waiting for event after reboot\n");
    pi_ble_ioctl(&ble, PI_NINA_B112_WAIT_FOR_EVENT, (void *) rx_buffer);
    printf("Received Event after reboot: %s\n", rx_buffer);
#endif

    // Enter Data Mode
    pi_ble_ioctl(&ble, PI_NINA_B112_DATA_MODE_ENTER, NULL);
    printf("Data Mode Entered!\n");

    /* Wait 50 ms. */
#ifndef __PLATFORM_RTL__
    pi_time_wait_us(50 * 1000);
#endif

    pi_task_t block;

    for (int i = 0; i < 3; i++)
    {
        init_snd_message();

        pi_task_block(&block);
        pi_ble_data_get_async(&ble, rcv_message, 200, &block);
        pi_ble_data_send(&ble, snd_message, 200);
        pi_task_wait_on(&block);

        verify_rcv_message();
        printf("Verified\n");
    }

    printf("Exiting BLE test\n");

    pmsis_exit(0);
}

int main(void)
{
    printf("Start NINA BLE module test\n");
    return pmsis_kickoff((void *) test_ble);
}
