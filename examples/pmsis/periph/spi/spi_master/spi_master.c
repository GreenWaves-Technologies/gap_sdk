
#include "pmsis.h"

static uint8_t spi_tx_buffer[32] __attribute__((aligned(32))); // alignment is a requirement for SPI

void test_spi_master()
{
    printf("SPI example start\n");

    struct pi_spi_conf spi_conf = { 0 };
    struct pi_device spi_dev = { 0 };

    pi_spi_conf_init(&spi_conf);
    spi_conf.max_baudrate = 5 * 1000 * 1000;
    spi_conf.itf = 1; // on Gapuino SPI1 can be used freely
    spi_conf.cs = 0;
    spi_conf.wordsize = PI_SPI_WORDSIZE_8;

    pi_open_from_conf(&spi_dev, &spi_conf);

    if (pi_spi_open(&spi_dev)) {
        printf("SPI open failed\n");
        pmsis_exit(-1);
    }

    for (size_t i = 0; i < sizeof(spi_tx_buffer); i += 1) {
        spi_tx_buffer[i] = 0xaa + i;
    }

//#define SYNC

    const size_t loops = 10;
    for (size_t i = 0; i < loops; i += 1) {

        printf("SPI send (%i/%i)\n", i + 1, loops);

        // use pi_spi_send to only send data, pi_spi_receive to only receive data and pi_spi_transfer to send and receive data

#ifdef SYNC
        pi_spi_send(&spi_dev, spi_tx_buffer, 8 * sizeof(spi_tx_buffer), PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE);
#else
        pi_task_t wait_task = { 0 };
        pi_task_block(&wait_task);
        pi_spi_send_async(&spi_dev, spi_tx_buffer, 8 * sizeof(spi_tx_buffer), PI_SPI_CS_AUTO | PI_SPI_LINES_SINGLE, &wait_task);
        pi_task_wait_on(&wait_task);
#endif
        pi_time_wait_us(10 * 1000);
    }

    printf("SPI example end\n");

    pmsis_exit(0);
}

int main(void)
{
    printf("\n\n\t *** PMSIS SPI Master ***\n\n");
    return pmsis_kickoff((void *) test_spi_master);
}
