#include "pmsis.h"
#include "stdio.h"
#include "bsp/bsp.h"
#include "ble_protocol.h"

#define QUEUE_SIZE 4
#define AT_RESP_ARRAY_LENGTH 64

#define MIN(a, b) (((a)<(b))?(a):(b))

unsigned char face_image_buffer[QUEUE_SIZE][128*128];
short face_descriptor[QUEUE_SIZE][512];
char name[QUEUE_SIZE][16] = {"Lena", "Francesco"};

unsigned int queue_head = 0;
unsigned int queue_tail = 2;

char string_buffer[127];
char current_name[17];
short current_descriptor[512];

pi_nina_ble_t ble;
pi_task_t name_task;
pi_task_t teskriptor_task;

char empty_response = '\0';
unsigned char ack = BLE_ACK;
unsigned char action = 0;
char* ptr = NULL;

static uint8_t rx_buffer[AT_RESP_ARRAY_LENGTH];

static void name_ready()
{
    printf("name_ready called!\n");
    current_name[16] = '\0';
    printf("Name %s got\n", current_name);
    pi_nina_b112_send_data(&ble, &ack, 1, NULL);
    printf("BLE_ACK responded\n");
}

void body(void* parameters)
{
    char Resp_String[AT_RESP_ARRAY_LENGTH];

    if (pi_nina_b112_open(&ble))
    {
        printf("Failed to open nina ble\n");
        return ;
    }
    printf("BLE UART init done\n");
    // --  Start NINA-B1 BLE module   -----

    /* // Init GPIO that will control NINA DSR in deasserted position */
    /* pi_gpio_pin_write(0, GPIOA21_NINA17, 0); */

    /* // Enable BLE (release reset) */
    /* pi_gpio_pin_write(0, GPIOA2_NINA_RST, 1); */

    /* pi_time_wait_us(1*1000*1000); // some waiting needed after BLE reset... */

    printf("Sending cmd using pmsis bsp\n");
    pi_nina_b112_AT_send(&ble, "E0");
    printf("Echo disabled\n");
    pi_nina_b112_AT_send(&ble, "+UFACTORY");
    printf("Factory configuration restored\n");
    pi_nina_b112_AT_send(&ble, "+UBTUB=FFFFFFFFFFFF");
    printf("Set UBTUB\n");
    pi_nina_b112_AT_send(&ble, "+UBTLE=2");
    printf("Set UBTLE\n");
    pi_nina_b112_AT_send(&ble, "+UBTLN=GreenWaves-GAPOC");
    printf("Set UBTLN\n");
    pi_nina_b112_AT_query(&ble, "+UMRS?", (char *) rx_buffer);
    printf("BLE configuration : %s\n", rx_buffer);
    pi_nina_b112_AT_send(&ble, "+UMRS=115200,2,8,1,1");
    pi_nina_b112_AT_query(&ble, "+UMRS?", (char *) rx_buffer);
    printf("BLE configuration : %s\n", rx_buffer);
    pi_nina_b112_AT_query(&ble, "+UBTLN?", (char *) rx_buffer);
    printf("BLE name : %s\n", rx_buffer);
    //pi_nina_b112_close(&ble);

    printf("AT Config Done\n");

    // After Reboot of NINA,  central connects to NINA and NINA will provide
    // unsollicited AT event: +UUBTACLC:<peer handle,0,<remote BT address>)
    // (...but sometimes just provides empty event instead !?)

    // Just make sure NINA sends something as AT unsolicited response, therefore is ready :
    pi_nina_b112_wait_for_event(&ble, Resp_String);
    printf("Received Event after reboot: %s\n", Resp_String);

    /*
    GAPOC_GPIO_Set_High(GAPOC_HEARTBEAT_LED);
    #ifdef __FREERTOS__
    vTaskDelay( 100 / portTICK_PERIOD_MS );
    #else
    wait(0.1);
    #endif

    GAPOC_GPIO_Set_Low(GAPOC_HEARTBEAT_LED);
    #ifdef __FREERTOS__
    vTaskDelay( 100 / portTICK_PERIOD_MS );
    #else
    wait(0.1);
    #endif
    */

    // Enter Data Mode
    pi_nina_b112_AT_send(&ble, "O");
    printf("Data Mode Entered!\n");

    pi_time_wait_us(1*1000*1000); // leave some time for Central to be properly configured

    while(1)
    {
        pi_nina_b112_get_data_blocking(&ble, &action, 1);
        pi_nina_b112_get_data_blocking(&ble, (uint8_t *) current_name, 16);
        current_name[16] = '\0';
        int action_id = action;
        printf("Action: %d\n", action_id);
        printf("Text got: %s\n",current_name);
    }

    printf("Exiting BLE test\n");

    pmsis_exit(0);
}

int main(void)
{
    printf("Start NINA BLE module test\n");
    return pmsis_kickoff((void *) body);
}
