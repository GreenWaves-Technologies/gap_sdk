#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>
#include <string.h>
#include "testbench.h"


#define CMD_BUFFER_SIZE 256



typedef struct
{
    uint8_t frame_start;
    uint8_t cmd;
    uint32_t address;
    uint8_t size_minus_1;
    uint8_t crc;
} __attribute__((packed)) spi_boot_req_t;


#if TEST_DURATION >= 100
#define BUFFER_SIZE 4096
#define NB_ITER 4

#elif TEST_DURATION >= 75
#define BUFFER_SIZE 2048
#define NB_ITER 4

#elif TEST_DURATION >= 50
#define BUFFER_SIZE 1024
#define NB_ITER 4

#elif TEST_DURATION >= 25
#define BUFFER_SIZE 64
#define NB_ITER 1

#else
#define BUFFER_SIZE 1024
#define NB_ITER 4
#endif


static pi_device_t bench;
static pi_device_t spi;

static uint8_t cmd_rx_buffer[CMD_BUFFER_SIZE+2];
// TX buffer is init to 0 to not send X on netlist sim
static uint8_t cmd_tx_buffer[CMD_BUFFER_SIZE+2] = {0};


static void spi_entry()
{
    printf("ENTRY\n");
}


static int spim_verif_enable(int itf, int cs)
{
  struct pi_testbench_conf bench_conf;
  pi_testbench_conf_init(&bench_conf);

  bench_conf.uart_id = CONFIG_TESTBENCH_UART_ID;
  bench_conf.uart_baudrate = CONFIG_TESTBENCH_UART_BAUDRATE;

  pi_open_from_conf(&bench, &bench_conf);

  if (pi_testbench_open(&bench))
      return -1;

  pi_testbench_req_spim_verif_setup_t config = {
    .enabled=1, .itf=itf, .cs=cs, .is_master=1, .mem_size_log2=16
  };
  if (pi_testbench_spim_verif_setup(&bench, &config))
    return -1;

    return 0;
}


static void handle_spi_commands_for_size(int end_size)
{
    memset((void *)cmd_tx_buffer, 0, sizeof(spi_boot_req_t)+2);

    while(end_size)
    {
        // Wait for start of frame
        while(1)
        {
            cmd_tx_buffer[0] = 0xA5;
            pi_spi_transfer(&spi, cmd_tx_buffer, cmd_rx_buffer, sizeof(spi_boot_req_t)*8, 0);
            if (cmd_rx_buffer[0] == 0x5A)
                break;
        }

        // We's just receiced the full request (containing the start of frame), parse it
        spi_boot_req_t *req = (spi_boot_req_t *)cmd_rx_buffer;
        uint32_t address = req->address;
        int size = req->size_minus_1 + 1;

        // Prepare the reply, meanwhile the master is sending ACK requests
        cmd_tx_buffer[0] = 0x79;
        cmd_tx_buffer[1] = 0x00;  // This will contain CRC when supported

        if (req->cmd == 0x1)
        {
            // Read command. Just append the values to be sent after the ACK and the CRC
            for (int i=0; i<size; i++)
            {
                cmd_tx_buffer[2 + i] = ((uint8_t *)address)[i];
            }

            pi_spi_send(&spi, cmd_tx_buffer, (size+2)*8, 0);
        }
        else if (req->cmd == 0x2)
        {
            // Write command, do a full duplex transfer to send the ACK, and receive the data
            pi_spi_transfer(&spi, cmd_tx_buffer, cmd_rx_buffer, (size+2)*8, 0);

            for (int i=0; i<size; i++)
            {
                ((uint8_t *)address)[i] = cmd_rx_buffer[2 + i];
            }
        }
        else if (req->cmd == 0x3)
        {
            void (*entry)() = (void (*)()) address;
            entry();
        }

        end_size -= size;
    }

}


static int test_entry()
{
    struct pi_spi_conf conf;
    pi_task_t task;

    printf("STA\n");

    // First initialize testbench to get valid values on the pads
    spim_verif_enable(SPI_ITF, SPI_CS);

    pi_spi_conf_init(&conf);

    conf.is_slave = 1;
    conf.itf = SPI_ITF;

    pi_open_from_conf(&spi, &conf);

    if (pi_spi_open(&spi))
        goto error;

    uint8_t *tx_buffer = pi_l2_malloc(BUFFER_SIZE);
    if (tx_buffer == NULL)
        goto error;

    uint8_t *rx_buffer = pi_l2_malloc(BUFFER_SIZE);
    if (rx_buffer == NULL)
        goto error;



    for (int i=0; i<BUFFER_SIZE/2; i++)
    {
        ((uint16_t *)tx_buffer)[i] = i;
    }

    /*
     * First do a read from tx_buffer
     */

    // Ask testbench do initiate a read using the boot protocol

    pi_testbench_req_spim_verif_transfer_t config1 = {
        .frequency=10000000, .address=(uint32_t)tx_buffer, .size=BUFFER_SIZE, .is_master=0, .is_rx=1, .is_duplex=0, .is_boot_protocol=1
    };

    pi_testbench_spim_verif_transfer(&bench, SPI_ITF, SPI_CS, &config1);

    // And handles SPI commands until we processed the size of the buffer to be read
    handle_spi_commands_for_size(BUFFER_SIZE);


    /*
     * Then a write to rx_buffer
     */

    // Ask testbench do initiate a write using the boot protocol
    pi_testbench_req_spim_verif_transfer_t config2 = {
        .frequency=10000000, .address=(uint32_t)rx_buffer, .size=BUFFER_SIZE, .is_master=0, .is_rx=0, .is_duplex=0, .is_boot_protocol=1
    };

    pi_testbench_spim_verif_transfer(&bench, SPI_ITF, SPI_CS, &config2);

    // And handles SPI commands until we processed the size of the buffer to be written
    handle_spi_commands_for_size(BUFFER_SIZE);


    /*
     * Finally check that what we received tis what we sent
     */

    for (int i=0; i<BUFFER_SIZE/2; i++)
    {
        uint16_t expected = i;
        if (((uint16_t *)rx_buffer)[i] != expected)
        {
            printf("Error detected (index: %d, expected: %x, got: %x)\n", i, expected, ((uint16_t *)rx_buffer)[i]);
            goto error;
        }
    }

    printf("TOK\n");

    return 0;

error:
    printf("TKO\n");

    return -1;
}

static void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
