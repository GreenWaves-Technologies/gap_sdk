/* PMSIS includes */
#include "pmsis.h"

/* --------- */
/* constants */
/* --------- */
#define NB_DEVICES (10)

static const int32_t test_number = 20;

/* --------- */
/* variables */
/* --------- */

static pi_device_t udma_datamove_device[NB_DEVICES] = {0};
static pi_udma_datamove_conf_t conf[NB_DEVICES] = {0};

/* ---------------- */
/* static functions */
/* ---------------- */

/* main test */
static void test_open_close(void)
{
    /* iterate for some time */
    for (int32_t i = 0; i < test_number; i++)
    {
        printf("Iteration #%d\n", i);
        /* open a bunch of datamove devices */
        for (size_t j = 0; j < NB_DEVICES; j++)
        {
            pi_udma_datamove_conf_init(&conf[j]);
            conf[j].device_id = j % 2;
            pi_open_from_conf(&udma_datamove_device[j], &conf[j]);
            int ret = pi_udma_datamove_open(&udma_datamove_device[j]);

            if (0 != ret)
            {
                printf("Could not open udma_datamove(%d)\n", j);
                pmsis_exit(-1);
            }
        }

        /* close devices */
        for (size_t j = 0; j < NB_DEVICES; j++)
        {
            pi_udma_datamove_close(&udma_datamove_device[j]);
        }
    }

    pmsis_exit(0);
}

/* -------------- */
/* Program Entry. */
/* -------------- */
int main(void)
{
    printf("\n\n\t *** PMSIS UDMA DATAMOVE Open/Close Tests ***\n\n");
    return pmsis_kickoff((void *) test_open_close);
}

