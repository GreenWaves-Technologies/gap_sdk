/* PMSIS includes */
#include "pmsis.h"

#include "pmsis/drivers/udma_fifo.h"

/* --------- */
/* constants */
/* --------- */
static const int32_t test_number = 20;

/* --------- */
/* variables */
/* --------- */

static pi_device_t udma_fifo_device[UDMA_NB_CHAN_FIFO+1] = {0};
static pi_udma_fifo_conf_t conf[UDMA_NB_CHAN_FIFO+1] = {0};

/* ---------------- */
/* static functions */
/* ---------------- */

/* main test */
static void test_open_close(void)
{
    /* open a dummy udma peripheral for udma fifo to be enabled */
    pi_aes_conf_t aes_conf;
    pi_device_t aes_device;
    pi_aes_conf_init(&aes_conf);
    pi_open_from_conf(&aes_device, &aes_conf);
    if(PI_OK != pi_aes_open(&aes_device))
    {
        printf("Could not open dummy aes device\n");
        pmsis_exit(-1);
    }


    printf("UDMA_NB_CHAN_FIFO %d\n", UDMA_NB_CHAN_FIFO);
    /* iterate for some time */
    for (int32_t i = 0; i < test_number; i++)
    {
        /* open the maximum number of fifo channels */
        for (int32_t j = 0; j < UDMA_NB_CHAN_FIFO; j++)
        {
            pi_udma_fifo_conf_init(&conf[j]);
            pi_open_from_conf(&udma_fifo_device[j], &conf[j]);
            int ret = pi_udma_fifo_open(&udma_fifo_device[j]);

            if (0 != ret)
            {
                printf("Could not open udma_fifo(%d)\n", j);
                pmsis_exit(-1);
            }
        }
        /* open one more channel than maximum number */

        pi_udma_fifo_conf_init(&conf[UDMA_NB_CHAN_FIFO]);
        pi_open_from_conf(&udma_fifo_device[UDMA_NB_CHAN_FIFO], &conf[UDMA_NB_CHAN_FIFO]);
        int ret = pi_udma_fifo_open(&udma_fifo_device[UDMA_NB_CHAN_FIFO]);

        if (0 == ret)
        {
            printf("Opened a fifo that does not exist...\n");
            pmsis_exit(-1);
        }

        /* free channels */
        for (int32_t j = 0; j < UDMA_NB_CHAN_FIFO; j++)
        {
            pi_udma_fifo_close(&udma_fifo_device[j]);
        }
    }

    pi_aes_close(&aes_device);
    pmsis_exit(0);
}

/* -------------- */
/* Program Entry. */
/* -------------- */
int main(void)
{
    printf("\n\n\t *** PMSIS UDMA FIFO Open/Close Tests ***\n\n");
    return pmsis_kickoff((void *) test_open_close);
}

