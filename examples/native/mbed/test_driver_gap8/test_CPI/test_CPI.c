#include<string.h>
#include "gap_cpi.h"

#define CHKSUM_IMG0   0xBEAC6B
#define CHKSUM_IMG1   0x93B5AA
#define CAM_H_RES     320
#define CAM_V_RES     240
#define FULL_IMAGE_SIZE (CAM_H_RES * CAM_V_RES)

#define LL_X          30
#define LL_Y          70
#define UR_X          172
#define UR_Y          134
#define SLICE_IMAGE_SIZE ((UR_X-LL_X)*(UR_Y-LL_Y))

static cpi_transfer_t cpiTransfer;
static cpi_handle_t   handle;
static CPI_Type *const cpi_address[] = CPI_BASE_PTRS;

GAP_L2_DATA char fullImage[2][FULL_IMAGE_SIZE];
GAP_L2_DATA char sliceImage[2][SLICE_IMAGE_SIZE];

volatile int finish = 0;

void full_callback(CPI_Type *base, cpi_handle_t *handle, status_t status, void *userdata) {

    if((uint32_t)userdata == 1) {
        finish = 1;
    } else {
        CPI_ReceptionCreateHandle(cpi_address[0], handle, full_callback, (void *)(((uint32_t)userdata + 1)));

        cpiTransfer.data        = (uint8_t*) fullImage[((uint32_t)userdata + 1)];
        cpiTransfer.dataSize    = FULL_IMAGE_SIZE;
        cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(0) | UDMA_CFG_EN(1);
        CPI_ReceptionNonBlocking(cpi_address[0], handle, &cpiTransfer);
    }
}

void slice_callback(CPI_Type *base, cpi_handle_t *handle, status_t status, void *userdata) {

    if((uint32_t)userdata == 1) {
        finish = 1;
    } else {
        CPI_ReceptionCreateHandle(cpi_address[0], handle, slice_callback, (void *)(((uint32_t)userdata + 1)));

        cpiTransfer.data        = (uint8_t*)  sliceImage[((uint32_t)userdata + 1)];
        cpiTransfer.dataSize    = SLICE_IMAGE_SIZE;
        cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1) | UDMA_CFG_EN(1);
        CPI_ReceptionNonBlocking(cpi_address[0], handle, &cpiTransfer);
    }
}

int test_full_image()
{
    int error = 0;

    CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7);

    cpi_config_t cpi_config;
    CPI_GetDefaultConfig(&cpi_config);

    cpi_config.format = RGB565;
    cpi_config.shift  = 0x2;
    CPI_Enable(cpi_address[0], &cpi_config);

    image_filter_t filter;
    filter.r_coeff = 0x1;
    filter.g_coeff = 0x1;
    filter.b_coeff = 0x1;
    CPI_Filter(cpi_address[0], &filter);

    /* Create and binding handle */
    CPI_ReceptionCreateHandle(cpi_address[0], &handle, full_callback, (void *)0);

    cpiTransfer.data        = (uint8_t*) fullImage[0];
    cpiTransfer.dataSize    = FULL_IMAGE_SIZE;
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(0) | UDMA_CFG_EN(1);
    CPI_ReceptionNonBlocking(cpi_address[0], &handle, &cpiTransfer);

    while(!finish) {
        ITC_WaitIRQ();
    }
    finish = 0;

    uint32_t checksum0=0;
    uint32_t checksum1=0;
    for (int i=0;i<FULL_IMAGE_SIZE;i++) {
        checksum0 += fullImage[0][i];
        checksum1 += fullImage[1][i];
    }

    if ((checksum0 != CHKSUM_IMG0 && checksum0 != CHKSUM_IMG1) || (checksum1 != CHKSUM_IMG0 && checksum1 != CHKSUM_IMG1))
    {
        error++;
    }

    printf("CPI Full is finished\n");
    CPI_Deinit(cpi_address[0]);

    return error;
}

int test_slice_image()
{
    int error = 0;

    CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7);

    cpi_config_t cpi_config;
    CPI_GetDefaultConfig(&cpi_config);

    image_slice_t slice_config;
    image_coordinate_t ll = { .x = LL_X/2, .y = LL_Y };
    image_coordinate_t ur = { .x = UR_X/2, .y = UR_Y };
    slice_config.slice_ll = ll;
    slice_config.slice_ur = ur;
    CPI_ImageExtract(cpi_address[0], &cpi_config, &slice_config);

    // Two byte each pixel
    cpi_config.format   = BYPASS_BIGEND;
    cpi_config.slice_en = 1;
    cpi_config.row_len  = CAM_H_RES/2;

    CPI_Enable(cpi_address[0], &cpi_config);

    /* Create and binding handle */
    CPI_ReceptionCreateHandle(cpi_address[0], &handle, slice_callback, (void *)0);

    cpiTransfer.data        = (uint8_t*) sliceImage[0];
    cpiTransfer.dataSize    = SLICE_IMAGE_SIZE;
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1) | UDMA_CFG_EN(1);
    CPI_ReceptionNonBlocking(cpi_address[0], &handle, &cpiTransfer);

    while(!finish) {
        ITC_WaitIRQ();
    }
    finish = 0;

    for (int i=0; i<SLICE_IMAGE_SIZE; i++){
        if ( sliceImage[0][i] != sliceImage[1][i]){

            #ifdef DEBUG
            printf("%d: 0=%d, 1=%d\n" , i, sliceImage[0][i], sliceImage[1][i]);
            #endif

            error++;
        }
    }

    printf("CPI Slice is finished\n");
    CPI_Deinit(cpi_address[0]);

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os CPI driver test\n");

    int error = 0;

    error = test_full_image();
    if (error) {
        printf("Test failed with %d errors\n", error);
        return error;
    }

    error = test_slice_image();
    if (error) printf("Test failed with %d errors\n", error);
    else printf("Test success\n");

    return error;
}
