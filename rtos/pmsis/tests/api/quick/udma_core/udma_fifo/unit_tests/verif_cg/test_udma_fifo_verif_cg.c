/* PMSIS includes */
#include "pmsis.h"

/* --------- */
/* constants */
/* --------- */

/* --------- */
/* variables */
/* --------- */

/* ---------------- */
/* static functions */
/* ---------------- */

/* main test */
static void test_verif_cg(void)
{
    pi_device_t udma_fifo_device = {0};
    pi_udma_fifo_conf_t conf = {0};

    /* open fifo without a udma peripheral enabled */
    pi_udma_fifo_conf_init(&conf);
    pi_open_from_conf(&udma_fifo_device, &conf);
    if(PI_OK == pi_udma_fifo_open(&udma_fifo_device))
    {
        printf("Opened a fifo without a udma peripheral enabled...\n");
        pmsis_exit(-1);
    }

    /* open/enable a udma peripheral */
    pi_device_t aes_device;
    pi_aes_conf_t aes_conf;

    pi_aes_conf_init(&aes_conf);
    pi_open_from_conf(&aes_device, &aes_conf);
    if(pi_aes_open(&aes_device))
    {
        printf("Failed to open AES device\n");
        pmsis_exit(-1);
    }

    /* open fifo with a udma peripheral enabled */

    if(PI_OK != pi_udma_fifo_open(&udma_fifo_device))
    {
        printf("Could not open udma fifo even with a udma peripheral enabled...\n");
        pmsis_exit(-2);
    }


    /* free channels */
    pi_udma_fifo_close(&udma_fifo_device);

    pi_aes_close(&aes_device);
    pmsis_exit(0);
}

/* -------------- */
/* Program Entry. */
/* -------------- */
int main(void)
{
    printf("\n\n\t *** PMSIS UDMA FIFO UDMA CG verification Tests ***\n\n");
    return pmsis_kickoff((void *) test_verif_cg);
}

