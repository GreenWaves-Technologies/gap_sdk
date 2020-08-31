#include "stdio.h"
/* PMSIS includes. */
#include "pmsis.h"
#include "bsp/bsp.h"
#include "ble_protocol.h"

#define AT_RESP_ARRAY_LENGTH       ( 64 )

static struct pi_device ble;

unsigned char action = 0;

static uint8_t rx_buffer[AT_RESP_ARRAY_LENGTH];

void test_ble(void)
{
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
    /* Retrieve BLE HCI configuration. */
    pi_ble_at_cmd(&ble, "+UMRS?", (void *) rx_buffer, 1);
    printf("BLE UART settings: %s\n", rx_buffer);

    // After Reboot of NINA,  central connects to NINA and NINA will provide
    // unsollicited AT event: +UUBTACLC:<peer handle,0,<remote BT address>)
    // (...but sometimes just provides empty event instead !?)

    // Just make sure NINA sends something as AT unsolicited response, therefore is ready :
    pi_ble_ioctl(&ble, PI_NINA_B112_WAIT_FOR_EVENT, (void *) rx_buffer);
    printf("Received Event after reboot: %s\n", rx_buffer);

    // Enter Data Mode
    pi_ble_ioctl(&ble, PI_NINA_B112_DATA_MODE_ENTER, NULL);
    printf("Data Mode Entered!\n");

    /* Wait 50 ms. */
    pi_time_wait_us(50 * 1000);

    while (1)
    {
        pi_ble_data_get(&ble, &action, 1);
        printf("Action: %d\n", action);
    }

    printf("Exiting BLE test\n");

    pmsis_exit(0);
}

int main(void)
{
    printf("Start NINA BLE module test\n");
    return pmsis_kickoff((void *) test_ble);
}
