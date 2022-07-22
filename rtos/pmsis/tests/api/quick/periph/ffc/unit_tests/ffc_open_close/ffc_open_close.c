/**
 * FFC open close test
 **/

#include <stdio.h>

#include "pmsis.h"

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)


int ffc_open_close(void)
{
    struct pi_device ffc_device;
    struct pi_ffc_conf conf;
    pi_ffc_conf_init(&conf);
    pi_open_from_conf(&ffc_device, &conf);
    int ret = pi_ffc_open(&ffc_device);

    if(!ret)
    {
        PRINTF("FFC open succeeded\n");
    }
    else
    {
        PRINTF("FFC open failed\n");
        pmsis_exit(-1);
    }

    pi_ffc_close(&ffc_device);

    return ret;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
int test_main(void)
{
    PRINTF("Application start\n");

    int ret = 0;

    ret = ffc_open_close();

    if(ret)
    {
        printf("test returned with %d errors\n", ret);
    }
    else
    {
        printf("Test succeeded\n");
    }
    pmsis_exit(ret);
    while(1);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void*) test_main);
}

