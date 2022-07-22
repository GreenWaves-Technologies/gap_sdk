#include "pmsis.h"
#include "bsp/ble/nina_b312.h"
#include "setup.h"
#include "bleNotifier.h"

/* Test without flow control,
   puts FLOW_CONTROL to 0
   VS
   Test with flow control,
   puts FLOW_CONTROL to 1
 */
#define FLOW_CONTROL 1;

#define PAD_GPIO_3V3_PERIPH (PI_PAD_035)

#define PRINTF printf

uint8_t rx_buffer[PI_AT_RESP_ARRAY_LENGTH];

static struct pi_device ble;

int initHandler()
{
    PRINTF("Entered test\n");

    PRINTF("Enabling BLE module...\n");
    int status = 1;
    int32_t errors = 0;

    struct pi_nina_b312_conf conf = {0};
    pi_ble_nina_b312_conf_init(&ble, &conf);
    conf.use_ctrl_flow = FLOW_CONTROL;
    pi_open_from_conf(&ble, &conf);

    errors += pi_ble_open(&ble);
    if (errors)
    {
        printf("Failed to open nina ble %ld\n", errors);
        pmsis_exit(-1);
    }
    PRINTF("BLE UART init done\n");
    pi_time_wait_us(50*1000);

    errors += pi_ble_ioctl(&ble, PI_NINA_B312_CLIENT_CONFIGURE, NULL);

    PRINTF("Launch test\n");

    /* Retrieve BLE module name. */
    pi_ble_at_cmd(&ble, "+UBTLN?", (void *) rx_buffer, 1);
    PRINTF("BLE name: %s\n", rx_buffer);
    pi_time_wait_us(50 * 1000);
    /* Retrieve BLE HCI configuration. */
    pi_ble_at_cmd(&ble, "+UMRS?", (void *) rx_buffer, 1);
    PRINTF("BLE UART settings: %s\n", rx_buffer);
    pi_time_wait_us(50 * 1000);

#ifdef DEVICE_CONNECTED
    // This requires having a device to connect with Nina.

    // After Reboot of NINA,  central connects to NINA and NINA will provide
    // unsollicited AT event: +UUBTACLC:<peer handle,0,<remote BT address>)
    // (...but sometimes just provides empty event instead !?)

    // Just make sure NINA sends something as AT unsolicited response, therefore is ready :
    pi_ble_ioctl(&ble, PI_NINA_B312_WAIT_FOR_EVENT, (void *) rx_buffer);
    PRINTF("Received Event after reboot: %s\n", rx_buffer);
#endif

    // Enter Data Mode
    pi_ble_ioctl(&ble, PI_NINA_B312_DATA_MODE_ENTER, NULL);
    PRINTF("Data Mode Entered!\n");

    /* Wait 50 ms. */
    pi_time_wait_us(50 * 1000);

    return errors;
}

void closeHandler()
{
    pi_ble_close(&ble);
}
