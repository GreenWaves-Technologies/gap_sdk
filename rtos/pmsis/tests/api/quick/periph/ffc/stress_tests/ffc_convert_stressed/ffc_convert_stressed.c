/**
 * FFC open close test
 **/

#include <stdio.h>

#include "pmsis.h"
#include "ffc_test_utils.h"
#include "testbench.h"
#include "testlib.h"

/* --------- */
/* Constants */
/* --------- */

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)

#ifndef TEST_SIZE
#define TEST_SIZE (100)
#endif

/* ---------- */
/* structures */
/* ---------- */

typedef struct test_fixed_data_s
{
    pi_ffc_fixed_type_e type;
    uint32_t size;
    uint32_t precisions[1];
    uint32_t scales[1];
} test_fixed_data_t;

typedef struct test_float_data_s
{
    pi_ffc_float_type_e type;
    uint32_t sign_bit;
    uint32_t exponent;
    uint32_t mantissa;
} test_float_data_t;

/* ---------------- */
/* static variables */
/* ---------------- */

static testlib_hyperram_trafficgen_config_t config0;
static testlib_hyperram_trafficgen_t traffic_gen0;

static testlib_hyperram_trafficgen_config_t config1;
static testlib_hyperram_trafficgen_t traffic_gen1;

static test_fixed_data_t test_fixed_datas[4] = {
    {
        .type = PI_FFC_FIXED_8,
        .size = 8,
        .precisions = { 7 },
        .scales = { 5 },
    },
    {
        .type = PI_FFC_FIXED_16,
        .size = 16,
        .precisions = { 12 },
        .scales = { 3 },
    },
    {
        .type = PI_FFC_FIXED_24,
        .size = 24,
        .precisions = { 21 },
        .scales = { 7 },
    },
    {
        .type = PI_FFC_FIXED_32,
        .size = 32,
        .precisions = { 29},
        .scales = { 6 },
    },
};

static test_float_data_t test_float_datas[3] = {
    {
        .type = PI_FFC_FLOAT_FP16,
        .sign_bit = 1,
        .exponent = 5,
        .mantissa = 10,
    },
    {
        .type = PI_FFC_FLOAT_BFP16,
        .sign_bit = 1,
        .exponent = 8,
        .mantissa = 7,
    },
    {
        .type = PI_FFC_FLOAT_FP32,
        .sign_bit = 1,
        .exponent = 8,
        .mantissa = 23,
    }
};

static int32_t  fx2fl_points_src[TEST_SIZE] = { 0xFF };
static uint32_t fx2fl_points_ffc_dst[TEST_SIZE] = {1.0};
static uint32_t fx2fl_points_sw_dst[TEST_SIZE] = {2.0};

static uint32_t fl2fx_points_src[TEST_SIZE] = {0.5};
static int32_t  fl2fx_points_ffc_dst[TEST_SIZE] = {0xFE};
static int32_t  fl2fx_points_sw_dst[TEST_SIZE] = {0xFD};

/* ---------------- */
/* static functions */
/* ---------------- */

static void start_traffic_generator(void)
{
    testlib_hyperram_trafficgen_conf_init(&config0);
    config0.transfer_size = 1024 * 20;
    testlib_hyperram_trafficgen_init(&traffic_gen0, &config0);
    testlib_hyperram_trafficgen_start(&traffic_gen0);

    testlib_hyperram_trafficgen_conf_init(&config1);
    config1.transfer_size = 1024 * 20;
    config1.itf = 1;
    config1.cs = 0;
    testlib_hyperram_trafficgen_init(&traffic_gen1, &config1);
    testlib_hyperram_trafficgen_start(&traffic_gen1);
}

static int32_t stop_traffic_generator(void)
{
    int32_t errors = testlib_hyperram_trafficgen_stop(&traffic_gen0);
    errors += testlib_hyperram_trafficgen_stop(&traffic_gen1);
    testlib_hyperram_trafficgen_deinit(&traffic_gen0);
    testlib_hyperram_trafficgen_deinit(&traffic_gen1);

    return errors;
}

static void gen_data_fixed(test_fixed_data_t* fx_data)
{
    /* generate data on the whole range */
    switch(fx_data->type)
    {
        case PI_FFC_FIXED_8:
            for (size_t i = 0; i < TEST_SIZE; i++)
            {
                ((uint8_t*)fx2fl_points_src)[i] = ((uint32_t) (i * UINT8_MAX)) / TEST_SIZE;
            }
            break;
        case PI_FFC_FIXED_16:
            for (size_t i = 0; i < TEST_SIZE; i++)
            {
                ((uint16_t*)fx2fl_points_src)[i] = ((uint32_t) (i * UINT16_MAX)) / TEST_SIZE;
            }
            break;
        case PI_FFC_FIXED_24:
            {
                const uint32_t UINT24_MAX = (1 << 24) - 1;
                for (size_t i = 0; i < TEST_SIZE; i++)
                {
                    fx2fl_points_src[i] = ((uint64_t) (i * UINT24_MAX)) / TEST_SIZE;
                }
            }
            break;
        case PI_FFC_FIXED_32:
            for (size_t i = 0; i < TEST_SIZE; i++)
            {
                fx2fl_points_src[i] = ((uint64_t) (i * UINT32_MAX)) / TEST_SIZE;
            }
            break;
        default:
            printf("Invalid fixed type\n");
            pmsis_exit(-1);
            break;
    }
}

static void gen_data_float(test_float_data_t* fl_data)
{
    /* generate data on the whole range */
    switch(fl_data->type)
    {
        case PI_FFC_FLOAT_FP16:
        case PI_FFC_FLOAT_BFP16:
            for (size_t i = 0; i < TEST_SIZE; i++)
            {
                ((uint16_t*) fl2fx_points_src)[i] = ((uint32_t) (i * UINT16_MAX)) / TEST_SIZE ;
            }
            break;
        case PI_FFC_FLOAT_FP32:
            for (size_t i = 0; i < TEST_SIZE; i++)
            {
                ((uint32_t*) fl2fx_points_src)[i] = ((uint64_t) (i * UINT32_MAX)) / TEST_SIZE ;
            }
            break;
        default:
            printf("Invalid float type\n");
            pmsis_exit(-1);
            break;
    }
}

static void verify_result_fx2fl(test_float_data_t* fl_data)
{
    /* memcmp the whole array */
    size_t data_size = 1;
    switch(fl_data->type)
    {
        case PI_FFC_FLOAT_BFP16:
        case PI_FFC_FLOAT_FP16:
            data_size = sizeof(uint16_t);
            break;
        case PI_FFC_FLOAT_FP32:
            data_size = sizeof(uint32_t);
            break;
        default:
            printf("Invalid float type\n");
            pmsis_exit(-1);
            break;
    }
    if (0 != memcmp(&fx2fl_points_ffc_dst, &fx2fl_points_sw_dst, TEST_SIZE * data_size))
    {
        for (size_t i = 0; i < TEST_SIZE * data_size; i++)
        {
            if (0 != memcmp(&((uint8_t*)fx2fl_points_ffc_dst)[i], &((uint8_t*)fx2fl_points_sw_dst)[i], sizeof(uint8_t)))
            {
                printf("fx2fl [%d](ffc) = 0x%x\n", i, ((uint8_t*)fx2fl_points_ffc_dst)[i]);
                printf("fx2fl [%d](sw)  = 0x%x\n", i, ((uint8_t*)fx2fl_points_sw_dst)[i]);
                pmsis_exit(-1);
            }
        }
    }
}

static void verify_result_fl2fx(test_fixed_data_t* fx_data)
{
    /* memcmp the whole array */
    size_t data_size = 1;
    switch(fx_data->type)
    {
        case PI_FFC_FIXED_8:
            data_size = 1;
            break;
        case PI_FFC_FIXED_16:
            data_size = sizeof(uint16_t);
            break;
        case PI_FFC_FIXED_24:
        case PI_FFC_FIXED_32:
            data_size = sizeof(uint32_t);
            break;
        default:
            printf("Invalid fixed type\n");
            pmsis_exit(-1);
            break;
    }
    if (0 != memcmp(&fl2fx_points_ffc_dst, &fl2fx_points_sw_dst, TEST_SIZE * data_size))
    {
        for (size_t i = 0; i < TEST_SIZE * data_size; i++)
        {
            if (0 != memcmp(&((uint8_t*)fl2fx_points_ffc_dst)[i], &((uint8_t*)fl2fx_points_sw_dst)[i], sizeof(uint8_t)))
            {
                printf("fl2fx [%d](ffc) = 0x%x\n", i, ((uint8_t*)fl2fx_points_ffc_dst)[i]);
                printf("fl2fx [%d](sw)  = 0x%x\n", i, ((uint8_t*)fl2fx_points_sw_dst)[i]);
                pmsis_exit(-1);
            }
        }
    }
}

static void convert_to_fixed(uint32_t* src, int32_t* dst, size_t size,
                             test_fixed_data_t* fx_data, test_float_data_t* fl_data,
                             size_t precision_index, size_t scale_index)
{
#if 1
    printf("-- fx_data->size=%d, fx_data->precision=%d, fx_data->scale=%d\n",
            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index]);
    printf("-- fl_data->sign_bit=%d, fl_data->exponent=%d, fl_data->mantissa=%d\n",
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
#endif
    switch(fx_data->type)
    {
        case PI_FFC_FIXED_8:
            switch(fl_data->type)
            {
                case PI_FFC_FLOAT_BFP16:
                case PI_FFC_FLOAT_FP16:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint8_t*) dst)[i] = FloatToFixedFormat(((uint16_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                case PI_FFC_FLOAT_FP32:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint8_t*)dst)[i] = FloatToFixedFormat(((uint32_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                    break;
                default:
                    printf("Invalid float type\n");
                    break;
            }
            break;
        case PI_FFC_FIXED_16:
            switch(fl_data->type)
            {
                case PI_FFC_FLOAT_BFP16:
                case PI_FFC_FLOAT_FP16:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint16_t*) dst)[i] = FloatToFixedFormat(((uint16_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                case PI_FFC_FLOAT_FP32:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint16_t*)dst)[i] = FloatToFixedFormat(((uint32_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                    break;
                default:
                    printf("Invalid float type\n");
                    break;
            }
            break;
        case PI_FFC_FIXED_24:
        case PI_FFC_FIXED_32:
            switch(fl_data->type)
            {
                case PI_FFC_FLOAT_BFP16:
                case PI_FFC_FLOAT_FP16:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint32_t*) dst)[i] = FloatToFixedFormat(((uint16_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                        /* sign extension */
                        if ((fx_data->type == PI_FFC_FIXED_24) && ((uint32_t*)dst)[i] > 0x800000)
                        {
                            ((uint32_t*) dst)[i] = ((uint32_t*) dst)[i] | 0xFF000000;
                        }
                    }
                    break;
                case PI_FFC_FLOAT_FP32:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint32_t*)dst)[i] = FloatToFixedFormat(((uint32_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                        /* sign extension */
                        if ((fx_data->type == PI_FFC_FIXED_24) && ((uint32_t*)dst)[i] > 0x800000)
                        {
                            ((uint32_t*) dst)[i] = ((uint32_t*) dst)[i] | 0xFF000000;
                        }
                    }
                    break;
                    break;
                default:
                    printf("Invalid float type\n");
                    break;
            }
            break;
        default:
            printf("Invalid fixed type\n");
            pmsis_exit(-1);
            break;
    }
}

static void convert_to_float(int32_t* src, uint32_t* dst, size_t size,
                             test_fixed_data_t* fx_data, test_float_data_t* fl_data,
                             size_t precision_index, size_t scale_index)
{
    switch(fl_data->type)
    {
        case PI_FFC_FLOAT_BFP16:
        case PI_FFC_FLOAT_FP16:
            switch(fx_data->type)
            {
                case PI_FFC_FIXED_8:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint16_t*)dst)[i] = (uint16_t) FixedToFloatFormat(((uint8_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                case PI_FFC_FIXED_16:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint16_t*)dst)[i] = (uint16_t) FixedToFloatFormat(((uint16_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                case PI_FFC_FIXED_24:
                case PI_FFC_FIXED_32:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint16_t*)dst)[i] = (uint16_t) FixedToFloatFormat(((uint32_t*)src)[i],
                            fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                default:
                    printf("Invalid fixed type\n");
                    pmsis_exit(-1);
                    break;
            }
            break;
        case PI_FFC_FLOAT_FP32:
            switch(fx_data->type)
            {
                case PI_FFC_FIXED_8:
                    for (size_t i = 0; i < size; i++)
                    {
                        dst[i] = FixedToFloatFormat(((uint8_t*)src)[i],
                                fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                                fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                case PI_FFC_FIXED_16:
                    for (size_t i = 0; i < size; i++)
                    {
                        dst[i] = FixedToFloatFormat(((uint16_t*)src)[i],
                                fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                                fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                case PI_FFC_FIXED_24:
                case PI_FFC_FIXED_32:
                    for (size_t i = 0; i < size; i++)
                    {
                        dst[i] = FixedToFloatFormat(((uint32_t*)src)[i],
                                fx_data->size, fx_data->precisions[precision_index], fx_data->scales[scale_index],
                                fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
                    }
                    break;
                default:
                    printf("Invalid fixed type\n");
                    pmsis_exit(-1);
                    break;
            }
            break;
        default:
            printf("Invalid float type\n");
            pmsis_exit(-1);
            break;
    }
}

static int ffc_convert(test_fixed_data_t* fx_data, test_float_data_t* fl_data)
{
    int ret = 0;
    struct pi_device ffc_fx2fl_device;
    struct pi_ffc_conf fx2fl_conf;
    struct pi_device ffc_fl2fx_device;
    struct pi_ffc_conf fl2fx_conf;

    size_t prec_size = sizeof(fx_data->precisions)/sizeof(fx_data->precisions[0]);
    size_t scale_size = sizeof(fx_data->scales)/sizeof(fx_data->scales[0]);

    for (size_t i = 0; i < prec_size; i++)
    {
        for (size_t j = 0; j < scale_size; j++)
        {
            {
                /* open fx2fl */
                pi_ffc_conf_init(&fx2fl_conf);

                /* customize configuration */
                fx2fl_conf.mode = PI_FFC_FIXED_TO_FLOAT;
                fx2fl_conf.fixed_type = fx_data->type;
                fx2fl_conf.float_type = fl_data->type;
                fx2fl_conf.fixed_precision = fx_data->precisions[i];
                fx2fl_conf.fixed_scale = fx_data->scales[j];

                pi_open_from_conf(&ffc_fx2fl_device, &fx2fl_conf);

                /* open */
                ret = pi_ffc_open(&ffc_fx2fl_device);

                if(ret)
                {
                    PRINTF("FFC fx2fl open failed\n");
                    pmsis_exit(-1);
                }
            }
            {
                /* open fl2fx */
                pi_ffc_conf_init(&fl2fx_conf);

                /* customize configuration */
                fl2fx_conf.mode = PI_FFC_FLOAT_TO_FIXED;
                fl2fx_conf.fixed_type = fx_data->type;
                fl2fx_conf.float_type = fl_data->type;
                fl2fx_conf.fixed_precision = fx_data->precisions[i];
                fl2fx_conf.fixed_scale = fx_data->scales[j];

                pi_open_from_conf(&ffc_fl2fx_device, &fl2fx_conf);

                /* open */
                ret = pi_ffc_open(&ffc_fl2fx_device);

                if(ret)
                {
                    PRINTF("FFC fl2fx open failed\n");
                    pmsis_exit(-1);
                }
            }

            /* generate random data */
            gen_data_float(fl_data);
            gen_data_fixed(fx_data);

            /* launch FFC conversions (async) */
            printf("Launching conversion\n");
            pi_task_t block_fx2fl;
            pi_task_t block_fl2fx;
            pi_task_block(&block_fx2fl);
            pi_task_block(&block_fl2fx);

            pi_ffc_convert_async(&ffc_fx2fl_device, fx2fl_points_src, fx2fl_points_ffc_dst, TEST_SIZE, &block_fx2fl);
            pi_ffc_convert_async(&ffc_fl2fx_device, fl2fx_points_src, fl2fx_points_ffc_dst, TEST_SIZE, &block_fl2fx);

            /* Launch SW conversions */
            convert_to_float(fx2fl_points_src, fx2fl_points_sw_dst, TEST_SIZE, fx_data, fl_data, i, j);
            convert_to_fixed(fl2fx_points_src, fl2fx_points_sw_dst, TEST_SIZE, fx_data, fl_data, i, j);

            /* wait for FFC conversions to end */
            pi_task_wait_on(&block_fx2fl);
            pi_task_wait_on(&block_fl2fx);

            pi_task_destroy(&block_fx2fl);
            pi_task_destroy(&block_fl2fx);

            /* close FFCs */
            pi_ffc_close(&ffc_fx2fl_device);
            pi_ffc_close(&ffc_fl2fx_device);

            /* verify result */
            verify_result_fx2fl(fl_data);
            verify_result_fl2fx(fx_data);
        }
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
    PRINTF("=======================\n");
    PRINTF("WARNING: TEST IS LONG !\n");
    PRINTF("=======================\n");

    /* start traffic generator */

    printf("starting traffic generator\n");
    start_traffic_generator();
    printf("traffic generator started\n");

    /* start test */
    int ret = 0;

    size_t fixed_datas_size = sizeof(test_fixed_datas)/sizeof(test_fixed_datas[0]);
    size_t float_datas_size = sizeof(test_float_datas)/sizeof(test_float_datas[0]);
    for (size_t i = 0; i < fixed_datas_size; i++)
    {
        for (size_t j = 0; j < float_datas_size; j++)
        {
            ret = ffc_convert(&(test_fixed_datas[i]), &(test_float_datas[j]));
        }
    }

    /* stop traffic generator */
    printf("Stopping traffic generator\n");
    if(stop_traffic_generator())
    {
        printf("ERROR in generated traffic\n");
        pmsis_exit(-1);
    }

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

