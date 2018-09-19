#include<string.h>
#include "gap_cpi.h"

#define CHKSUM_IMG0 0xBEAC6B
#define CHKSUM_IMG1 0x93B5AA
#define CAM_H_RES       320
#define CAM_V_RES       240
#define CAM_PIXEL_IMAGE_SIZE (CAM_H_RES * CAM_V_RES)

static CPI_Type *const cpi_address[] = CPI_BASE_PTRS;
static cpi_config_t cpi_config;
static image_filter_t filter;

/* Two image buffers */
GAP_L2_DATA uint8_t first_img[CAM_PIXEL_IMAGE_SIZE];
GAP_L2_DATA uint8_t second_img[CAM_PIXEL_IMAGE_SIZE];

/* #define   NON_BLOCKING   1 */

void callback() {
    printf("hello world !\n");
}

int main()
{
    printf("Fabric controller code execution for mbed_os CPI driver test\n");

    cpi_transfer_t cpiTransfer;

    CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7);

    CPI_GetDefaultConfig(&cpi_config);
    cpi_config.format = RGB565;
    cpi_config.shift = 0x2;

    filter.r_coeff = 0x1;
    filter.g_coeff = 0x1;
    filter.b_coeff = 0x1;

    cpiTransfer.data = first_img;
    cpiTransfer.dataSize = CAM_PIXEL_IMAGE_SIZE;
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1) | UDMA_CFG_EN(1);

    CPI_Filter(cpi_address[0], &filter);
    CPI_Enable(cpi_address[0], &cpi_config);
    CPI_ReceptionBlocking(cpi_address[0], &cpiTransfer);

    cpiTransfer.data = second_img;
    cpiTransfer.dataSize = CAM_PIXEL_IMAGE_SIZE;
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1) | UDMA_CFG_EN(1);

    #ifndef NON_BLOCKING

    /* Blocking CPI transfer */
    CPI_ReceptionBlocking(cpi_address[0], &cpiTransfer);

    #else
    /* Non-blocking CPI transfer */
    cpi_handle_t handle;

    CPI_ReceptionCreateHandle(cpi_address[0], &handle, callback, NULL);
    CPI_ReceptionNonBlocking(cpi_address[0], &handle, &cpiTransfer);
    #endif

    printf("CPI tranfer is finished\n");
    CPI_Disable(cpi_address[0]);

    unsigned int checksum=0, err=0;
    for (int i=0;i<CAM_PIXEL_IMAGE_SIZE;i++) {
      checksum += first_img[i];
    }
    if (checksum != CHKSUM_IMG0 && checksum != CHKSUM_IMG1) {
      err++;
      printf("[ERROR] Checksum0 is %X expected %X\n",checksum,CHKSUM_IMG0);
    }

    checksum = 0;
    for (int i=0;i<CAM_PIXEL_IMAGE_SIZE;i++) {
      checksum += second_img[i];
    }
    if (checksum != CHKSUM_IMG0 && checksum != CHKSUM_IMG1) {
      err++;
      printf("[ERROR] Checksum1 is %X expected %X\n",checksum,CHKSUM_IMG1);
    }
    return err;
}
