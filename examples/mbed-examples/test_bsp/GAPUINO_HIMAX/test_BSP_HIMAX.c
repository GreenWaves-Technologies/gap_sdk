#include <string.h>
// MBED OS
#include "cmsis_os2.h"
#include "gapuino_himax.h"
#include "gap_common.h"
#include "ImgIO.h"

#define CAM_W_RES       324
#define CAM_H_RES       244
#define CAM_PIXEL_IMAGE_SIZE (CAM_H_RES * CAM_W_RES)

static CPI_Type *const cpi_address[] = CPI_BASE_PTRS;

typedef struct {
    unsigned int  L2_Addr;
    unsigned int  Size;
    unsigned int  imgReady;
    unsigned int  camReady;
} ImgDescriptor;

GAP_L2_DATA ImgDescriptor ImageOutHeader;
GAP_L2_DATA unsigned char image_buffer[CAM_PIXEL_IMAGE_SIZE + PPM_HEADER];

char imgName[50];

int main()
{
    printf("Fabric controller code execution for mbed_os HIMAX driver test\n");

    BRIDGE_Init();

    printf("Connecting to bridge\n");

    BRIDGE_Connect(0, NULL);

    printf("Connection done\n");

    //Open i2c channel and configure HIMAX camera
    HIMAX_Open();

    printf("Camera Opened and Configured\n");
    //Init camera parallel interface
    CPI_Init(cpi_address[0], CPI_PCLK, CPI_HSYNC, CPI_VSYNC,
             CPI_DATA0, CPI_DATA1, CPI_DATA2, CPI_DATA3,
             CPI_DATA4, CPI_DATA5, CPI_DATA6, CPI_DATA7);
    printf("CPI Inited\n");

    /* Set configuration */
    cpi_config_t cpi_config;
    CPI_GetDefaultConfig(&cpi_config);
    cpi_config.row_len         = (QVGA_W+4)/2; //Used only on slice mode
    /* HIMAX_MONO_COLOR */
    cpi_config.format          = BYPASS_BIGEND;
    cpi_config.shift           = DISABLE;
    cpi_config.slice_en        = DISABLE;
    cpi_config.frameDrop_en    = DISABLE;
    cpi_config.frameDrop_value = 0;
    cpi_config.wordWidth       = 16;

    printf("Ready to take a photo\n");

    /* Enable and configure channel */
    CPI_Enable(cpi_address[0], &cpi_config);

    cpi_transfer_t cpiTransfer;

    for (int i = 0; i < 2; i++){
        printf("Taking picture...\n");

        HIMAX_Mode(HIMAX_Streaming);

        /* Start channel */
        cpiTransfer.data        = image_buffer + PPM_HEADER;
        cpiTransfer.dataSize    = CAM_PIXEL_IMAGE_SIZE;
        cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1); //0 -> 8bit //1 -> 16bit //2 -> 32bit
        CPI_ReceptionBlocking(cpi_address[0], &cpiTransfer);

        /* Disable camera and UDMA channel */
        HIMAX_Mode(HIMAX_Standby);

        printf("Picture taken!\n");
        ImageOutHeader.camReady = 0;

        sprintf(imgName, "../../../img_OUT%d.ppm", i);
        printf("imgName: %s\n", imgName);

        WriteImageToFile(imgName, CAM_W_RES, CAM_H_RES, (image_buffer + PPM_HEADER));
    }

    CPI_Deinit(cpi_address[0]);

    BRIDGE_Disconnect(NULL);
    printf("Test success\n");

    return 0;
}
