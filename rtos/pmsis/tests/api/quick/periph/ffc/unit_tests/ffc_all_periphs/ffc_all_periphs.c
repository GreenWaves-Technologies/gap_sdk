/**
 * FFC open close test
 **/

#include <stdio.h>

#include "pmsis.h"
#include "ffc_test_utils.h"

/* --------- */
/* Constants */
/* --------- */

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
    uint32_t precision;
    uint32_t scale;
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

static test_fixed_data_t test_fixed_datas = {
    .type = PI_FFC_FIXED_16,
    .size = 16,
    .precision = 8,
    .scale = 4,
};

static test_float_data_t test_float_datas = {
    .type = PI_FFC_FLOAT_FP32,
    .sign_bit = 1,
    .exponent = 8,
    .mantissa = 23,
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
static void gen_data_fixed(test_fixed_data_t* fx_data)
{
    /* generate data on the whole range */
    switch(fx_data->type)
    {
        case PI_FFC_FIXED_16:
            for (size_t i = 0; i < TEST_SIZE; i++)
            {
                ((uint16_t*)fx2fl_points_src)[i] = ((uint32_t) (i * UINT16_MAX)) / TEST_SIZE;
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
        case PI_FFC_FIXED_16:
            data_size = sizeof(uint16_t);
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
                             test_fixed_data_t* fx_data, test_float_data_t* fl_data)
{
    switch(fx_data->type)
    {
        case PI_FFC_FIXED_16:
            switch(fl_data->type)
            {
                case PI_FFC_FLOAT_FP32:
                    for (size_t i = 0; i < size; i++)
                    {
                        ((uint16_t*)dst)[i] = FloatToFixedFormat(((uint32_t*)src)[i],
                            fx_data->size, fx_data->precision, fx_data->scale,
                            fl_data->sign_bit, fl_data->exponent, fl_data->mantissa);
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
                             test_fixed_data_t* fx_data, test_float_data_t* fl_data)
{
    switch(fl_data->type)
    {
        case PI_FFC_FLOAT_FP32:
            switch(fx_data->type)
            {
                case PI_FFC_FIXED_16:
                    for (size_t i = 0; i < size; i++)
                    {
                        dst[i] = FixedToFloatFormat(((uint16_t*)src)[i],
                                fx_data->size, fx_data->precision, fx_data->scale,
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

static int ffc_convert(size_t periph_id, test_fixed_data_t* fx_data, test_float_data_t* fl_data)
{
    int ret = 0;
    struct pi_device ffc_fx2fl_device;
    struct pi_ffc_conf fx2fl_conf;
    struct pi_device ffc_fl2fx_device;
    struct pi_ffc_conf fl2fx_conf;

    {
        /* open fx2fl */
        pi_ffc_conf_init(&fx2fl_conf);

        /* customize configuration */
        fx2fl_conf.itf = periph_id;
        fx2fl_conf.mode = PI_FFC_FIXED_TO_FLOAT;
        fx2fl_conf.fixed_type = fx_data->type;
        fx2fl_conf.float_type = fl_data->type;
        fx2fl_conf.fixed_precision = fx_data->precision;
        fx2fl_conf.fixed_scale = fx_data->scale;

        pi_open_from_conf(&ffc_fx2fl_device, &fx2fl_conf);

        /* open */
        ret = pi_ffc_open(&ffc_fx2fl_device);

        if(ret)
        {
            printf("FFC fx2fl open failed\n");
            pmsis_exit(-1);
        }
    }
    {
        /* open fl2fx */
        pi_ffc_conf_init(&fl2fx_conf);

        /* customize configuration */
        fl2fx_conf.itf = periph_id;
        fl2fx_conf.mode = PI_FFC_FLOAT_TO_FIXED;
        fl2fx_conf.fixed_type = fx_data->type;
        fl2fx_conf.float_type = fl_data->type;
        fl2fx_conf.fixed_precision = fx_data->precision;
        fl2fx_conf.fixed_scale = fx_data->scale;

        pi_open_from_conf(&ffc_fl2fx_device, &fl2fx_conf);

        /* open */
        ret = pi_ffc_open(&ffc_fl2fx_device);

        if(ret)
        {
            printf("FFC fl2fx open failed\n");
            pmsis_exit(-1);
        }
    }

    /* generate random data */
    gen_data_float(fl_data);
    gen_data_fixed(fx_data);

    /* launch FFC conversions (async) */
    printf("(peripheral #%d) Launching conversion\n", periph_id);
    pi_task_t block_fx2fl;
    pi_task_t block_fl2fx;
    pi_task_block(&block_fx2fl);
    pi_task_block(&block_fl2fx);

    pi_ffc_convert_async(&ffc_fx2fl_device, fx2fl_points_src, fx2fl_points_ffc_dst, TEST_SIZE, &block_fx2fl);
    pi_ffc_convert_async(&ffc_fl2fx_device, fl2fx_points_src, fl2fx_points_ffc_dst, TEST_SIZE, &block_fl2fx);

    /* Launch SW conversions */
    convert_to_float(fx2fl_points_src, fx2fl_points_sw_dst, TEST_SIZE, fx_data, fl_data);
    convert_to_fixed(fl2fx_points_src, fl2fx_points_sw_dst, TEST_SIZE, fx_data, fl_data);

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

    return ret;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
static int test_main(void)
{
    printf("FFC All peripherals test\n");

    int ret = 0;

    for (size_t periph_id = 0; periph_id < UDMA_NB_FFC; periph_id++)
    {
        ret = ffc_convert(periph_id, &test_fixed_datas, &test_float_datas);
        if(ret)
        {
            printf("(peripheral #%d) Test returned with %d errors\n", periph_id, ret);
            pmsis_exit(ret);
        }
        else
        {
            printf("(peripheral #%d) Test succeeded\n", periph_id);
        }
    }

    pmsis_exit(0);
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

