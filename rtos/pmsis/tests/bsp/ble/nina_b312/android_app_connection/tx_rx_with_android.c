#include "pmsis.h"
#include "bsp/ble/nina_b312.h"
#include "setup.h"
#include "tx_rx_with_android.h"

/* Test without flow control,
   puts FLOW_CONTROL to 0
   VS
   Test with flow control,
   puts FLOW_CONTROL to 1
 */
#define FLOW_CONTROL 1;

#define PAD_GPIO_3V3_PERIPH (PI_PAD_035)

// #define PRINTF printf

uint8_t rx_byte;
uint8_t size_rx_buffer[4];
uint8_t temp_rx_buffer[PI_AT_RESP_ARRAY_LENGTH];
uint8_t rx_buffer[200];
pi_task_t size_cb = {0};
pi_task_t rx_cb = {0};
int rx_size_data;
int index_rx_buffer;

static struct pi_device ble;

typedef enum
{
    PI_AT_RESP_NOT_STARTED,       /*!< Response not received yet from BLE module.  */
    PI_AT_RESP_IN_PROGRESS,       /*!< Response transmission has started. */
    PI_AT_RESP_DONE               /*!< Response transmission received, with end of resp. */
} at_resp_state_e;
static volatile at_resp_state_e g_at_resp_state;

char to_send[200];

/*
 * FUNCTIONS DECLARATIONS
 */

void get_size_cb(void *arg);
void coucou(void *arg);
void uart_rx_cb(void *arg);
void uart_tx_cb(void *arg);

/*
 * FUNCTIONS DEFINITIONS
 */

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
    pi_ble_ioctl(&ble, PI_NINA_B312_WAIT_FOR_EVENT, (void *) rx_buffer);
    PRINTF("Received Event after reboot: %s\n", rx_buffer);

    // /* Set connectability mode */
    // pi_ble_at_cmd(&ble, "+UBTCM=2", (void *) rx_buffer, 1); // 2=connectable
    // PRINTF("Connectability mode: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    // /* Set discoverability mode */
    // pi_ble_at_cmd(&ble, "+UBTDM=3", (void *) rx_buffer, 1); // 3=general discoverable
    // PRINTF("Discoverability mode: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    /* Software version */
    // pi_ble_at_cmd(&ble, "+GMR", (void *) rx_buffer, 1);
    // PRINTF("Software version: %s\n", rx_buffer);
    // int sw_version = (int) rx_buffer[1];
    // pi_time_wait_us(50 * 1000);


    PRINTF("\nLaunch test\n\n");

    // /* Retrieve BLE module name. */
    memset(rx_buffer, 0, PI_AT_RESP_ARRAY_LENGTH);
    pi_ble_at_cmd(&ble, "+UBTLN?", (void *) rx_buffer, 1);
    PRINTF("BLE name: %s\n", rx_buffer);
    char ble_name[strlen(rx_buffer) - 10];
    for (int i = 0; i < (strlen(rx_buffer) - 10); i++)
    {
        ble_name[i] = rx_buffer[7 + i];
    }
    pi_time_wait_us(50 * 1000);
    // if (sw_version >= 51) // int 51 == '3'
    // {
    //     /* Bluetooth mode */
    //     pi_ble_at_cmd(&ble, "+UBTMODE?", (void *) rx_buffer, 1);
    //     PRINTF("Bluetooth mode: %s\n", rx_buffer);
    //     pi_time_wait_us(50 * 1000);
    // }
    // /* Retrieve BLE HCI configuration. */
    // pi_ble_at_cmd(&ble, "+UMRS?", (void *) rx_buffer, 1);
    // PRINTF("BLE UART settings: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    // /* Discoverability mode */ // Default is 3 : general discoverable
    // pi_ble_at_cmd(&ble, "+UBTDM?", (void *) rx_buffer, 1);
    // PRINTF("Discoverability mode: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    // /* Pairing mode */ // Default is 2 : pairing enable
    // pi_ble_at_cmd(&ble, "+UBTPM?", (void *) rx_buffer, 1);
    // PRINTF("Pairing mode: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    // /* Connectability mode */ // Default is 2 : connectable
    // pi_ble_at_cmd(&ble, "+UBTCM?", (void *) rx_buffer, 1);
    // PRINTF("Connectability mode: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    // /* Bonded devices */ // Should have none because we unbond them when we go in Client mode earlier
    // pi_ble_at_cmd(&ble, "+UBTBD", (void *) rx_buffer, 1);
    // PRINTF("Bonded devices: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);
    // /* BLE Role */ // Should be 2 (Peripheral) because we set it like this in Client mode
    // pi_ble_at_cmd(&ble, "+UBTLE?", (void *) rx_buffer, 1);
    // PRINTF("BLE Role: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);

    // Clear buffer to parse it correctly after
    memset(rx_buffer, 0, PI_AT_RESP_ARRAY_LENGTH);

    printf("Please pair/connect a device with %s\n", ble_name);
    // This requires having a device to connect with Nina.
    // Waiting +UUBTACLC
    pi_ble_ioctl(&ble, PI_NINA_B312_WAIT_FOR_EVENT, (void *) rx_buffer);
    PRINTF("Received event after a pairing has been requested: %s\n", rx_buffer);
    uint8_t conn_handle = (uint8_t) rx_buffer[10];
    uint8_t type = (uint8_t) rx_buffer[12];
    uint8_t len_bd_addr = strlen(rx_buffer) - 14;
    char bd_addr[len_bd_addr+1];
    for (int i = 0; i < len_bd_addr; i++)
    {
        bd_addr[i] = rx_buffer[14 + i];
    }
    bd_addr[len_bd_addr] = '\0';
    printf("conn_handle %c, type %c, bd_addr %s\n", conn_handle, type, bd_addr);

    printf("Please go into test section\n");
    // Waiting for +UUDPC
    pi_ble_ioctl(&ble, PI_NINA_B312_WAIT_FOR_EVENT, (void *) rx_buffer);
    PRINTF("Received event: %s\n", rx_buffer);

    /* List peers */
    // pi_ble_at_cmd(&ble, "+UDLP?", (void *) rx_buffer, 1);
    // PRINTF("Peers: %s\n", rx_buffer);
    // pi_time_wait_us(50 * 1000);

    // Enter Data Mode
    pi_ble_ioctl(&ble, PI_NINA_B312_DATA_MODE_ENTER, NULL);
    PRINTF("Data Mode Entered!\n");

    /* Wait 50 ms. */
    pi_time_wait_us(50 * 1000);

    /* Read data */
    for (int i = 0; ; i++)
    {
        index_rx_buffer = 0;
        g_at_resp_state = PI_AT_RESP_NOT_STARTED;
        pi_task_callback(&size_cb, get_size_cb, NULL);
        pi_ble_data_get_async(&ble, size_rx_buffer, 4, &size_cb);

        sprintf(to_send, "Hello World (%d)\r", i);
        pi_ble_data_send(&ble, to_send, strlen(to_send));
        printf("sent (%d)\n", strlen(to_send));

        while (g_at_resp_state != PI_AT_RESP_DONE)
        {
            pi_yield();
        }
        
        if (i == 9) i = 0;
    }

    return errors;
}

int byteArrayToInt(uint8_t b[])
{
    return (b[3] & 0xFF) + ((b[2] & 0xFF) << 8) + ((b[1] & 0xFF) << 16) + ((b[0] & 0xFF) << 24);
}

void get_size_cb(void *arg)
{
    rx_size_data = byteArrayToInt(size_rx_buffer);

    memset(temp_rx_buffer, 0, PI_AT_RESP_ARRAY_LENGTH);
    pi_task_callback(&rx_cb, coucou, NULL);
    pi_ble_data_get_async(&ble, temp_rx_buffer, rx_size_data, &rx_cb);
}

void coucou(void *arg)
{
    if (temp_rx_buffer[rx_size_data - 1] == 13) // \r
    {
        for (int i = 0; i < rx_size_data - 1; i++, index_rx_buffer++)
        {
            rx_buffer[index_rx_buffer] = temp_rx_buffer[i];
        }
        printf("received (%d): ", index_rx_buffer);
        for (int i = 0; i < index_rx_buffer; i++) { printf("%c", rx_buffer[i]); }
        printf("\n");
        g_at_resp_state = PI_AT_RESP_DONE;
    }
    else
    {
        for (int i = 0; i < rx_size_data; i++, index_rx_buffer++)
        {
            rx_buffer[index_rx_buffer] = temp_rx_buffer[i];
        }
        printf("received %d\n", rx_size_data);
        pi_task_callback(&size_cb, get_size_cb, NULL);
        pi_ble_data_get_async(&ble, size_rx_buffer, 4, &size_cb);
    }
}

void closeHandler()
{
    pi_ble_close(&ble);
}
