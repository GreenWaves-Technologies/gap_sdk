// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"

#include "conv1d_sw.h"
#include "filter_data.h"

uint32_t data_size[6]      = {32,64,128,256,512,1024};
uint32_t kernel_size[10]   = {3,5,7,9,11,33,65,129,257,513};
uint32_t mat_size[5]       = {3,7,15,21,27};
uint32_t window_sizes[3]   = {2,3,4};

/* Change the size to change the example run time */
#define DATA_SIZES      3
#define KERNEL_SIZES    10
#define TRANSP_MAT_SIZE 5
#define WIN_SIZES       3

uint16_t vector_res_sw[1024];
uint16_t vector_res_hw[1024];
uint16_t output_sw[1024];
uint16_t output_hw[1024];


static FILTER_Type *const filter_address[] = FILTER_BASE_PTRS;

uint32_t power_th_uint16(uint32_t data_size, uint32_t window_size, uint16_t *data_src, uint32_t threshold)
{
    uint32_t loc_window_size = (1 << window_size);
    uint32_t num_windows  = data_size - loc_window_size + 1;
    uint32_t counter;
    uint32_t acc;
    //printf("power_th_uint16 ds=%d ws=%d nw=%d th=%X\n",data_size,loc_window_size,num_windows,threshold);
    counter = 0;
    for(uint32_t i=0;i<num_windows;i++) {
        acc=0;
        //printf("Window %d/%d\n",i,num_windows);
        for(uint32_t j=0;j<loc_window_size;j++) {
            acc=acc+data_src[i+j]*data_src[i+j];
        }
        acc=acc/loc_window_size;
        //printf("Acc value %X\n",acc);
        if(acc > threshold)
            counter++;
    }
    //printf("power_th_uint16 done with count %d\n",counter);
    return counter;
}

void mat_bin_uint16_to_char(uint16_t *src, uint8_t *dst, uint32_t size, uint16_t threshold)
{
    for(uint32_t i=0;i<size;i++)
        if(src[i] > threshold)
            dst[i] = 1;
        else
            dst[i] = 0;
}

void mat_transpose_uint16( uint32_t col_num,
                           uint32_t row_num,
                           uint32_t src_stride,
                           uint32_t dst_stride,
                           uint16_t *data_src,
                           uint16_t *data_dst)
{
    uint32_t src_row_start = 0;
    for(uint32_t row=0;row<row_num;row++) {
        for(uint32_t col=0;col<col_num;col++) {
            data_dst[row+col*dst_stride] = data_src[src_row_start+col];
        }
        src_row_start = src_row_start + src_stride;
    }
}

void memcpy_uint16(uint16_t *src, uint16_t *dst, uint32_t size)
{
    for(uint32_t i=0;i<size;i++)
        dst[i] = src[i];
}

void memcpy_uint32(uint32_t *src, uint32_t *dst, uint32_t size)
{
    for(uint32_t i=0;i<size;i++)
        dst[i] = src[i];
}

int test_power_th_uint16 ()
{
    printf("Threshold.\n");

    int error = 0;
    int pwr_count_sw = 0;
    int pwr_count_hw = 0;

    int threshold = 0x1000;

    for (int ws = 0; ws < WIN_SIZES; ws++) {
        for (int ds = 0;ds < DATA_SIZES; ds++) {
            /* SW */
            pwr_count_sw = power_th_uint16(data_size[ds], window_sizes[ws], in_signal, threshold);

            /* HW */
            filter_threshold_config_t threshold_config;

            threshold_config.in_format   = uFILTER_16_Bits;
            threshold_config.mode        = uFILTER_In_Sliding;
            threshold_config.operation   = uFILTER_AU_accAxA;
            threshold_config.control     = uFILTER_L2_OFF_OFF_ON;
            threshold_config.input       = in_signal;
            threshold_config.size        = data_size[ds];
            threshold_config.window_size = window_sizes[ws];
            threshold_config.threshold   = threshold;
            threshold_config.out_format  = uFILTER_32_Bits;

            pwr_count_hw = FILTER_Threshold(filter_address[0], &threshold_config);

            /* CHECK */
            int err = 0;
            if(pwr_count_sw != pwr_count_hw)
                err++;

            #if DEBUG == 1
            printf("Done power thersholding with %d errors DATA_SIZE=%d, cycle sw=%d, cyclehw=%d\n",
                   err, data_size[ds], 0, 0);
            #endif

            error += err;
        }
    }

    return error;
}

int test_mat_bin_uint16_to_char()
{
    printf("Binarization.\n");

    int error = 0;
    int threshold = 0x4000;

    for (int ds = 0; ds < DATA_SIZES ; ds++) {
        /* SW */
        mat_bin_uint16_to_char(input_buffer, (uint8_t *)output_sw, data_size[ds], threshold);

        /* HW */
        filter_binarization_config_t binarization_config;

        binarization_config.in_format   = uFILTER_16_Bits;
        binarization_config.in_mode     = uFILTER_In_Linear;
        binarization_config.control     = uFILTER_L2_OFF_ON_ON;
        binarization_config.input       = input_buffer;
        binarization_config.size        = data_size[ds];
        binarization_config.out_mode    = uFILTER_Out_Linear;
        binarization_config.output      = output_hw;
        binarization_config.threshold   = threshold;
        binarization_config.out_format  = uFILTER_8_Bits;

        FILTER_Binarization(filter_address[0], &binarization_config);

        /* CHECK */
        int err = 0;
        for(size_t i = 0;i < data_size[ds]; i++)
        {
            if(output_sw[i] != output_hw[i])
                err++;
        }

        #if DEBUG == 1
        printf("Done binarization with %d/%d errors DATA_SIZE=%d, cycle sw=%d, cyclehw=%d\n",
               err, data_size[ds], data_size[ds], 0, 0);
        #endif

        error += err;
    }

    return error;
}

int test_mat_transpose_uint16()
{
    printf("Transpose.\n");

    int error = 0;

    /* Initialize Output Buffer */
    for (int i = 0; i < 1024; i++) {
        output_sw[i] = 0;
        output_hw[i] = 0;
    }

    for (int i = 0; i < TRANSP_MAT_SIZE; i++) {
        for (int j = 0; j < TRANSP_MAT_SIZE; j++) {
            int row_num = mat_size[i];
            int col_num = mat_size[j];

            /* SW */
            mat_transpose_uint16(col_num, row_num, 32, 32, input_buffer, output_sw);

            /* HW */
            filter_transpose_config_t transpose_config;

            transpose_config.in_format   = uFILTER_16_Bits;
            transpose_config.in_mode     = uFILTER_In_2D;
            transpose_config.control     = uFILTER_L2_OFF_ON_OFF;
            transpose_config.col_num     = col_num;
            transpose_config.row_num     = row_num;
            transpose_config.in_stride   = 32;
            transpose_config.out_stride  = 32;
            transpose_config.input       = input_buffer;
            transpose_config.out_mode    = uFILTER_Out_2D_COL;
            transpose_config.output      = output_hw;

            FILTER_Transpose(filter_address[0], &transpose_config);

            /* CHECK */
            int err = 0;
            for(int row=0; row < col_num; row++)
                for(int col=0; col < row_num; col++)
                {
                    if(output_sw[row*32+col] != output_hw[row*32+col])
                        err++;
                }

            #if DEBUG == 1
            printf("Done transpose with %d/%d errors ROW=%d, COL=%d cycle sw=%d, cyclehw=%d\n",
                   err, row_num*col_num, row_num, col_num, 0, 0);
            #endif

            error += err;
        }
    }

    return error;
}

int test_memcpy_uint16()
{
    printf("MEMCPY.\n");

    int error = 0;

    for (int ds = 0 ; ds< DATA_SIZES; ds++) {
        /* SW */
        memcpy_uint16(input_buffer, output_sw, data_size[ds]);

        /* HW */
        filter_memcpy_config_t memcpy_config;

        memcpy_config.in_format   = uFILTER_16_Bits;
        memcpy_config.in_mode     = uFILTER_In_Linear;
        memcpy_config.control     = uFILTER_L2_OFF_ON_OFF;
        memcpy_config.input       = input_buffer;
        memcpy_config.size        = data_size[ds];
        memcpy_config.out_mode    = uFILTER_Out_Linear;
        memcpy_config.output      = output_hw;

        FILTER_Memcpy(filter_address[0], &memcpy_config);

        /* CHECK */
        int err = 0;
        for(size_t i = 0;i < data_size[ds]; i++)
        {
            if(output_sw[i] != output_hw[i])
                err++;
        }

        #if DEBUG == 1
        printf("Done memcpy with %d/%d errors DS=%d, cycle sw=%d, cyclehw=%d\n",
               err, data_size[ds], data_size[ds], 0, 0);
        #endif

        error += err;
    }

    return error;
}

int test_memcpy_non_aligned_uint16()
{
    printf("MEMCPY Non-Aligned.\n");

    int error = 0;

    uint8_t *input_ptr;

    /* Input buffer non algined - 16-bit */
    input_ptr = (uint8_t *) input_buffer;

    for (int na_off = 0; na_off < 4; na_off++) {
        for (int ds = 0; ds < DATA_SIZES; ds++) {
            /* lean output buffers */
            for (int i = 0; i< 1024; i++) {
                output_sw[i] = 0;
                output_hw[i] = 0;
            }

            /* SW */
            memcpy_uint16((uint16_t *)input_ptr, output_sw, data_size[ds]);

            /* HW */
            filter_memcpy_config_t memcpy_config;

            memcpy_config.in_format   = uFILTER_16_Bits;
            memcpy_config.in_mode     = uFILTER_In_Linear;
            memcpy_config.control     = uFILTER_L2_OFF_ON_OFF;
            memcpy_config.input       = (uint16_t *)input_ptr;
            memcpy_config.size        = data_size[ds];
            memcpy_config.out_mode    = uFILTER_Out_Linear;
            memcpy_config.output      = output_hw;

            FILTER_Memcpy(filter_address[0], &memcpy_config);

            /* CHECK */
            int err = 0;
            for(size_t i = 0;i < data_size[ds]; i++)
            {
                if(output_sw[i] != output_hw[i])
                    err++;
            }

            #if DEBUG == 1
            printf("[MEMCPY N.A.] 16bit NA input %d bytes offset Done memcpy with %d/%d errors DS=%d, cycle sw=%d, cyclehw=%d\n",
                   na_off, err, data_size[ds], data_size[ds], 0, 0);
            #endif

            error += err;
        }
        input_ptr++;
    }

    /* Input buffer non algined - 32-bit */
    input_ptr = (uint8_t *) input_buffer;

    for (int na_off = 0; na_off < 4; na_off++) {
        for (int ds = 0; ds < DATA_SIZES; ds++) {
            /* lean output buffers */
            for (int i = 0; i< 1024; i++) {
                output_sw[i] = 0;
                output_hw[i] = 0;
            }

            /* SW */
            memcpy_uint32((uint32_t *)input_ptr, (uint32_t *)output_sw, data_size[ds]/2);

            /* HW */
            filter_memcpy_config_t memcpy_config;

            memcpy_config.in_format   = uFILTER_32_Bits;
            memcpy_config.in_mode     = uFILTER_In_Linear;
            memcpy_config.control     = uFILTER_L2_OFF_ON_OFF;
            memcpy_config.input       = (uint32_t *)input_ptr;
            memcpy_config.size        = data_size[ds]/2;
            memcpy_config.out_mode    = uFILTER_Out_Linear;
            memcpy_config.output      = (uint32_t *)output_hw;

            FILTER_Memcpy(filter_address[0], &memcpy_config);

            /* CHECK */
            int err = 0;
            for(size_t i = 0;i < data_size[ds]; i++)
            {
                if(output_sw[i] != output_hw[i])
                    err++;
            }

            #if DEBUG == 1
            printf("[MEMCPY N.A.] 32bit NA input %d bytes offset Done memcpy with %d/%d errors DS=%d, cycle sw=%d, cyclehw=%d\n",
                   na_off, err, data_size[ds]/2, data_size[ds]/2, 0, 0);
            #endif

            error += err;
        }
        input_ptr++;
    }

    /* Output buffer non aligned - 16-bit */
    uint8_t *output_ptr_sw;
    uint8_t *output_ptr_hw;

    output_ptr_sw = (uint8_t *)output_sw;
    output_ptr_hw = (uint8_t *)output_hw;

    for (int na_off = 0; na_off < 4; na_off++) {
        for (int ds = 0; ds < DATA_SIZES; ds++) {
            /* Clean output buffers */
            for (int i=0;i<1024;i++) {
                output_sw[i] = 0;
                output_hw[i] = 0;
            }

            /* SW */
            memcpy_uint16(input_buffer, (uint16_t *)output_ptr_sw, data_size[ds]);

            /* HW */
            filter_memcpy_config_t memcpy_config;

            memcpy_config.in_format   = uFILTER_16_Bits;
            memcpy_config.in_mode     = uFILTER_In_Linear;
            memcpy_config.control     = uFILTER_L2_OFF_ON_OFF;
            memcpy_config.input       = input_buffer;
            memcpy_config.size        = data_size[ds];
            memcpy_config.out_mode    = uFILTER_Out_Linear;
            memcpy_config.output      = output_ptr_hw;

            FILTER_Memcpy(filter_address[0], &memcpy_config);

            /* CHECK */
            int err = 0;
            for(size_t i=0; i < data_size[ds]; i++)
            {
                if(output_sw[i] != output_hw[i])
                    err++;
            }

            #if DEBUG == 1
            printf("[MEMCPY N.A.] 16bit NA output %d bytes offset Done memcpy with %d/%d errors DS=%d, cycle sw=%d, cyclehw=%d\n",
                   na_off,err, data_size[ds], data_size[ds], 0, 0);
            #endif

            error += err;
        }
        output_ptr_sw++;
        output_ptr_hw++;
    }

    /* Output buffer non aligned - 32-bit */
    output_ptr_sw = (uint8_t *)output_sw;
    output_ptr_hw = (uint8_t *)output_hw;

    for (int na_off = 0; na_off < 4; na_off++) {
        for (int ds = 0; ds < DATA_SIZES; ds++) {
            /* Clean output buffers */
            for (int i=0;i<1024;i++) {
                output_sw[i] = 0;
                output_hw[i] = 0;
            }

            /* SW */
            memcpy_uint32((uint32_t *)input_buffer, (uint32_t *)output_ptr_sw, data_size[ds]/2);

            /* HW */
            filter_memcpy_config_t memcpy_config;

            memcpy_config.in_format   = uFILTER_32_Bits;
            memcpy_config.in_mode     = uFILTER_In_Linear;
            memcpy_config.control     = uFILTER_L2_OFF_ON_OFF;
            memcpy_config.input       = (uint32_t *)input_buffer;
            memcpy_config.size        = data_size[ds]/2;
            memcpy_config.out_mode    = uFILTER_Out_Linear;
            memcpy_config.output      = (uint32_t *)output_ptr_hw;

            FILTER_Memcpy(filter_address[0], &memcpy_config);

            /* CHECK */
            int err = 0;
            for(size_t i=0; i < data_size[ds]; i++)
            {
                if(output_sw[i] != output_hw[i])
                    err++;
            }

            #if DEBUG == 1
            printf("[MEMCPY N.A.] 32bit NA output %d bytes offset Done memcpy with %d/%d errors DS=%d, cycle sw=%d, cyclehw=%d\n",
                   na_off,err, data_size[ds]/2, data_size[ds]/2, 0, 0);
            #endif
            error += err;
        }
        output_ptr_sw++;
        output_ptr_hw++;
    }

    return error;
}

int test_convolution_uint16()
{
    printf("Convolution.\n");

    int error = 0;

    for (int ds = 0; ds < DATA_SIZES; ds++) {
        for (int ks = 0; ks < KERNEL_SIZES; ks++) {

            if(data_size[ds] > kernel_size[ks]) {
                /* SW */
                conv1d_uint16(input_buffer,
                              output_sw,
                              kernel_buffer,
                              data_size[ds],
                              kernel_size[ks]);


                /* HW */
                filter_convolution_config_t convolution_config;

                convolution_config.in_format   = uFILTER_16_Bits;
                convolution_config.in_mode0    = uFILTER_In_Sliding;
                convolution_config.in_mode1    = uFILTER_In_Circular;
                convolution_config.operation   = uFILTER_AU_accAxB;
                convolution_config.control     = uFILTER_L2_L2_ON_OFF;
                convolution_config.input       = input_buffer;
                convolution_config.size        = data_size[ds];
                convolution_config.kernel      = kernel_buffer;
                convolution_config.kernel_size = kernel_size[ks];
                convolution_config.out_mode    = uFILTER_Out_Linear;
                convolution_config.output      = output_hw;

                FILTER_Convolution(filter_address[0], &convolution_config);

                /* CHECK */
                int err=0;
                for(uint32_t i=0;i<(data_size[ds]-kernel_size[ks]);i++)
                {
                    if(output_sw[i] != output_hw[i])
                        err++;
                }

                #if DEBUG == 1
                printf("Done 1d conv with %d/%d errors DS=%d, KS=%d, sw cycles=%d, hw cycles=%d\n",
                       err, (data_size[ds]-kernel_size[ks]), data_size[ds], kernel_size[ks], 0, 0);
                #endif

                error += err;
            }
        }
    }

    return error;
}

int test_vector_uint16()
{
    printf("Vector.\n");

    int err   = 0;
    int error = 0;
    int register1 = 0x10;

    /* Operation : A x REG1 + B*/
    {
        /* SW */
        vect_AxREGpB_uint16(vector_a, vector_b, register1, vector_res_sw, 1024);

        /* HW */
        filter_vector_config_t vector_config;

        vector_config.in_format   = uFILTER_16_Bits;
        vector_config.in_mode0    = uFILTER_In_Linear;
        vector_config.in_mode1    = uFILTER_In_Linear;
        vector_config.operation   = uFILTER_AU_AxREG1pB;
        vector_config.control     = uFILTER_L2_L2_ON_OFF;
        vector_config.opa         = vector_a;
        vector_config.opb         = vector_b;
        vector_config.size        = 1024;
        vector_config.reg0        = 0;
        vector_config.reg1        = register1;
        vector_config.out_mode    = uFILTER_Out_Linear;
        vector_config.output      = vector_res_hw;

        FILTER_Vector(filter_address[0], &vector_config);

        /* CHECK */
        err=0;
        for(int i = 0; i < 1024; i++)
        {
            if(vector_res_sw[i] != vector_res_hw[i])
                err++;
        }

        #if DEBUG == 1
        printf("Done AxREG1+B with %d/%d errors %d,%d\n", err, 1024, 0, 0);
        #endif
        error += err;
    }

    /* Operation : A + B*/
    {
        /* SW */
        vect_ApB_uint16(vector_a, vector_b, vector_res_sw, 1024);

        /* HW */
        filter_vector_config_t vector_config;

        vector_config.in_format   = uFILTER_16_Bits;
        vector_config.in_mode0    = uFILTER_In_Linear;
        vector_config.in_mode1    = uFILTER_In_Linear;
        vector_config.operation   = uFILTER_AU_ApB;
        vector_config.control     = uFILTER_L2_L2_ON_OFF;
        vector_config.opa         = vector_a;
        vector_config.opb         = vector_b;
        vector_config.size        = 1024;
        vector_config.reg0        = 0;
        vector_config.reg1        = 0;
        vector_config.out_mode    = uFILTER_Out_Linear;
        vector_config.output      = vector_res_hw;

        FILTER_Vector(filter_address[0], &vector_config);

        /* CHECK */
        err=0;
        for(int i = 0; i < 1024; i++)
        {
            if(vector_res_sw[i] != vector_res_hw[i])
                err++;
        }

        #if DEBUG == 1
        printf("Done A+B with %d/%d errors %d,%d\n", err, 1024, 0, 0);
        #endif
        error += err;
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os FILTER driver test\n");

    int error = 0;

    filter_config_t config;

    FILTER_GetDefaultConfig(&config);

    FILTER_Init(filter_address[0], &config, SystemCoreClock);

    error = test_power_th_uint16();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_mat_bin_uint16_to_char();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_mat_transpose_uint16();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_memcpy_uint16();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_memcpy_non_aligned_uint16();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_convolution_uint16();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_vector_uint16();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    FILTER_Deinit(filter_address[0]);

    printf("Test success\n");

    return error;
}
