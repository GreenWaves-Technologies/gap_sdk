#include <stdio.h>

#include "pmsis.h"
#include "pmsis/implem/drivers/udma/aes_dual_core/aes_dual_core_internal.h"

static uint8_t aes_key[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
};

static int32_t aes_dual_core_simple(void)
{
    pi_device_t aes_dc;
    _pi_aes_dual_core_conf_t conf;

    _pi_aes_dual_core_conf_init(&conf);


    /* configure keys */
    conf.core0_conf.key = (uint32_t*) aes_key;
    conf.core1_conf.key = (uint32_t*) aes_key;

    /* open */
    pi_open_from_conf(&aes_dc, &conf);
    int32_t ret = _pi_aes_dual_core_open(&aes_dc);

    if (PI_OK != ret)
    {
        printf("Failed to open AES dual core\n");
        pmsis_exit(-1);
    }

    /* close */
    _pi_aes_dual_core_close(&aes_dc);

    return 0;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
int test_main(void)
{
    printf("Application start\n");

    int ret = 0;

    ret = aes_dual_core_simple();

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

