/**
 * FFC open close test
 **/

#include <stdio.h>

#include "pmsis.h"

/* --------- */
/* Constants */
/* --------- */

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)

#define NB_ITER (10)

/* ---------- */
/* structures */
/* ---------- */

/* ---------------- */
/* static variables */
/* ---------------- */

static uint32_t test_sizes[] = {
    1,
    10,
    100,
    1000,
    2000,
    5000,
    8000,
    10000,
    16000
};

static uint32_t bench_results[sizeof(test_sizes)/sizeof(test_sizes[0])] = { 0 };

/* ---------------- */
/* static functions */
/* ---------------- */

static int ffc_convert(void)
{
    int ret = 0;
    struct pi_device ffc_fx2fl_device;
    struct pi_ffc_conf fx2fl_conf;

    {
        /* open fx2fl */
        pi_ffc_conf_init(&fx2fl_conf);

        /* customize configuration */
        fx2fl_conf.mode = PI_FFC_FIXED_TO_FLOAT;
        fx2fl_conf.fixed_type = PI_FFC_FIXED_32;
        fx2fl_conf.float_type = PI_FFC_FLOAT_FP32;
        fx2fl_conf.fixed_precision = 0;
        fx2fl_conf.fixed_scale = 0;

        pi_open_from_conf(&ffc_fx2fl_device, &fx2fl_conf);

        /* open */
        ret = pi_ffc_open(&ffc_fx2fl_device);

        if(ret)
        {
            PRINTF("FFC fx2fl open failed\n");
            pmsis_exit(-1);
        }
    }

    for (uint32_t iter = 0; iter < NB_ITER; iter++)
    {
        for (uint64_t i = 0; i < sizeof(test_sizes)/sizeof(test_sizes[0]); i++)
        {
            /* alloc data */
            uint32_t* data_in = (uint32_t*) pi_l2_malloc(sizeof(uint32_t) * test_sizes[i]);
            uint32_t* data_out = (uint32_t*) pi_l2_malloc(sizeof(uint32_t) * test_sizes[i]);

            /* generate random data */
            for (uint32_t j = 0; j < test_sizes[i]; j++)
            {
                data_in[j] = j;
            }

            /* launch FFC conversions */
            //printf("Launching conversion for size %d\n", test_sizes[i]);
            pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
            pi_perf_reset();
            pi_perf_start();
            pi_ffc_convert(&ffc_fx2fl_device, data_in, data_out, test_sizes[i]);
            pi_perf_stop();
            uint32_t perf = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
            //printf("Perf: %d cycles\n", perf);
            bench_results[i] += perf;


            /* free data */
            pi_l2_free(data_in, sizeof(uint32_t) * test_sizes[i]);
            pi_l2_free(data_out, sizeof(uint32_t) * test_sizes[i]);
        }
    }

    /* close FFCs */
    pi_ffc_close(&ffc_fx2fl_device);


    for (uint64_t i = 0; i < sizeof(test_sizes)/sizeof(test_sizes[0]); i++)
    {
        printf("Perf: Size (uint32_t) %5d - Cycles %6d\n", test_sizes[i], bench_results[i] / NB_ITER);
    }

    return ret;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
static int test_main(void)
{
    PRINTF("Application start\n");

    int ret = 0;

    ret = ffc_convert();

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

/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
    return pmsis_kickoff((void*) test_main);
}

