#include <stdio.h>
#include <pmsis.h>
#include <stdint.h>

#define BUFFER_SIZE 256
#define NB_ITER 4

#define TIMESTAMP_SIZE     (16)

static pi_device_t spi;
static pi_device_t spim;

/* fifo for timestamp */
static pi_device_t ts_fifo_device = {0};
static pi_udma_fifo_conf_t ts_fifo_conf = {0};
static uint32_t timestamp_val[TIMESTAMP_SIZE];
static pi_device_t spi_timestamp;

static uint32_t init_timestamp_fifo(pi_device_t *fifo_timestamp, pi_udma_fifo_conf_t *conf)
{
    uint32_t fifo_id = 0;
    uint32_t timestamp_size = sizeof(uint32_t) * TIMESTAMP_SIZE;

    pi_udma_fifo_conf_init(conf);
    pi_open_from_conf(fifo_timestamp, conf);
    if (0 != pi_udma_fifo_open(fifo_timestamp))
    {
        printf("Could not open udma_fifo\n");
        return -1;
    }

    pi_udma_fifo_ioctl(fifo_timestamp, PI_UDMA_FIFO_GET_ID, &fifo_id);
    pi_udma_fifo_ioctl(fifo_timestamp, PI_UDMA_FIFO_SET_SIZE, &timestamp_size);

    return fifo_id;
}

static int test_entry()
{
    struct pi_spi_conf conf;
    struct pi_spi_conf spim_conf;
    pi_task_t task_spis;
    pi_task_t task_spim;

    printf("STA\n");

    pi_pad_set_function(PI_PAD_026, 0); //spi0_sck
    pi_pad_set_function(PI_PAD_027, 0); //spi0_cs
    pi_pad_set_function(PI_PAD_031, 0); //spi0_sdo
    pi_pad_set_function(PI_PAD_032, 0); //spi0_sdi

    pi_pad_set_function(PI_PAD_033, 0); //spi1_sck
    pi_pad_set_function(PI_PAD_034, 0); //spi1_cs
    pi_pad_set_function(PI_PAD_038, 0); //spi1_sdo
    pi_pad_set_function(PI_PAD_039, 0); //spi1_sdi

// Open SPI Master
    pi_spi_conf_init(&spim_conf);

    spim_conf.wordsize = PI_SPI_WORDSIZE_8;
    spim_conf.big_endian = 1;
    spim_conf.max_baudrate = 10000000;
    spim_conf.polarity = POLARITY;
    spim_conf.phase = PHASE;
    spim_conf.itf = SPIM_ITF;
    spim_conf.cs = SPIM_CS;

    pi_open_from_conf(&spim, &spim_conf);

    if (pi_spi_open(&spim))
        return -1;

    /* Set up timestamp */
    struct pi_timestamp_conf ts_conf;
    pi_timestamp_conf_init(&ts_conf);
    ts_conf.cnt_src = PI_TIMESTAMP_CNT_REF_CLK_QUICK;
    ts_conf.prescaler = 0x80;
    pi_open_from_conf(&spi_timestamp, &ts_conf);
    pi_udma_timestamp_open(&spi_timestamp);
    pi_udma_timestamp_ioctl(&spi_timestamp, PI_UDMA_TIMESTAMP_IOCTL_CLR, NULL);

    uint32_t ts_fifo_id = init_timestamp_fifo(&ts_fifo_device, &ts_fifo_conf);
    if (!ts_fifo_id)
    {
        printf("Timestamp fifo open failed\n");
        return -1;
    }

    /* Setup timestamp for master on both tx and rx*/
    pi_timestamp_event_t evt[2];
    evt[0].ts_evt_id = 0xFF;
    evt[1].ts_evt_id = 0xFF;
    pi_udma_timestamp_ioctl(&spi_timestamp, PI_UDMA_TIMESTAMP_IOCTL_EVT_ALLOC, &evt[0]);
    pi_udma_timestamp_ioctl(&spi_timestamp, PI_UDMA_TIMESTAMP_IOCTL_EVT_ALLOC, &evt[1]);
    if (evt[0].ts_evt_id == 0xFF || evt[1].ts_evt_id == 0xFF )
    {
        printf("TS evt alloc failed\n");
        return -2;
    }
    // Both using the same fifo
    evt[0].dest_id = ts_fifo_id;
    evt[1].dest_id = ts_fifo_id;
    pi_udma_timestamp_ioctl(&spi_timestamp, PI_UDMA_TIMESTAMP_IOCTL_SET_EVT, &evt[0]);
    pi_udma_timestamp_ioctl(&spi_timestamp, PI_UDMA_TIMESTAMP_IOCTL_SET_EVT, &evt[1]);

    // Set up for TX ch
    conf.ts_ch = 0;
    conf.ts_evt_id = evt[0].ts_evt_id;
    pi_spi_ioctl(&spim, PI_SPI_CTRL_SET_TIMESTAMP, &conf);

    // Set up for RX ch
    conf.ts_ch = 1;
    conf.ts_evt_id = evt[1].ts_evt_id;
    pi_spi_ioctl(&spim, PI_SPI_CTRL_SET_TIMESTAMP, &conf);



#ifdef LOOPBACK_TEST
// Open SPI slave
    pi_spi_conf_init(&conf);

    conf.is_slave = 1;
    conf.wordsize = PI_SPI_WORDSIZE_8;
    conf.big_endian = 1;
    conf.max_baudrate = 10000000;
    conf.itf = SPIS_ITF;
    conf.polarity = POLARITY;
    conf.phase = PHASE;
    conf.cs = SPIS_CS;

    pi_open_from_conf(&spi, &conf);

    if (pi_spi_open(&spi))
        goto error;
#endif

    void *tx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (tx_buffer == NULL)
        goto error;

    void *rx_buffer = pmsis_l2_malloc(BUFFER_SIZE);
    if (rx_buffer == NULL)
        goto error;


    for (int i=0; i<BUFFER_SIZE; i++)
    {
        ((uint8_t *)tx_buffer)[i] = i;
    }

    for(int i=0; i<NB_ITER; i++)
    {
#ifdef LOOPBACK_TEST
        pi_spi_receive_async(&spi, rx_buffer, (BUFFER_SIZE-1)*8, 0, pi_task_block(&task_spis));
#endif
        pi_spi_send_async(&spim, tx_buffer, BUFFER_SIZE*8, 0, pi_task_block(&task_spim));

#ifdef LOOPBACK_TEST
        pi_task_wait_on(&task_spis);
        printf("slave done\n");
#endif

        pi_task_wait_on(&task_spim);
        printf("transfer done \n");
    }

    uint32_t fifo_size=0;
    //timestamp_reg_dump();
    printf("start to pop timestamp \n");
    pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_GET_LEVEL, &fifo_size);
    printf("[Before pop]: Totally %d bytes in fifo\n", fifo_size);

    for (uint32_t i=0; i<(fifo_size/4); i++)
    {
        pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_POP32, &timestamp_val[i]);
        printf("timestamp val %d : %x\n", i, timestamp_val[i]);
    }

#ifdef LOOPBACK_TEST
    for (int i=0; i<BUFFER_SIZE; i++)
    {
        uint8_t expected = i+1;
        if (((uint8_t *)rx_buffer)[i] != expected)
        {
            printf("Error detected during RX (index: %d, value: %x, expected: %x)\n", i, ((uint8_t *)rx_buffer)[i], expected);
            goto error;
        }
    }
#endif

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
