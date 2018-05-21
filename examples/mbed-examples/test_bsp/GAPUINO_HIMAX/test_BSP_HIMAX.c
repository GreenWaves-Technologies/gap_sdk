#include<string.h>
// MBED OS
#include "cmsis_os2.h"
#include "gapuino_himax.h"
#include "gap_cpi.h"
#include "gap_i2c.h"

#define CAM_W_RES       324
#define CAM_H_RES       244
#define CAM_PIXEL_IMAGE_SIZE (CAM_H_RES * CAM_W_RES)
#define SKIP_FRAMES 10

static CPI_Type *const cpi_address[] = CPI_BASE_PTRS;

typedef struct {
  unsigned int  L2_Addr;
  unsigned int  Size;
  unsigned int  imgReady;
  unsigned int  camReady;
} ImgDescriptor;

GAP_L2_DATA ImgDescriptor ImageOutHeader;
GAP_L2_DATA unsigned char image_buffer[CAM_PIXEL_IMAGE_SIZE + PPM_HEADER];

static void CreatePPMHeader(unsigned char *ImgOut, ImgDescriptor *ImgOutHeader, unsigned int W, unsigned int H)
{
  unsigned char Buffer[PPM_HEADER];
  unsigned int Ind = 0, x, i, L;

  /* P5<cr>* */
  Buffer[Ind++] = 0x50; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

  /* W <space> */
  x = W; L=0;
  while (x>0) { x = x/10; L++; }
  x = W; i = 1;
  while (x>0) { Buffer[Ind+L-i] = 0x30 + (x%10); i++; x=x/10; }
  Ind += L;
  Buffer[Ind++] = 0x20;

  /* H <cr> */
  x = H; L=0;
  while (x>0) { x = x/10; L++; }
  x = H; i = 1;
  while (x>0) { Buffer[Ind+L-i] = 0x30 + (x%10); i++; x=x/10; }
  Ind += L;
  Buffer[Ind++] = 0xA;

  /* 255 <cr> */
  Buffer[Ind++] = 0x32; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0x35; Buffer[Ind++] = 0xA;

  for (i=0; i<Ind; i++) ImgOut[-Ind+i] = Buffer[i];

  /* Output image code as a ppm mono address in L2 */
  ImgOutHeader->L2_Addr = ((unsigned int) ImgOut) - Ind;
  /* Size of the ppm image including header */
  ImgOutHeader->Size = W*H+Ind;
  /* Image is ready */
  ImgOutHeader->imgReady = 1;
}

void skip_frames(int frames,cpi_transfer_t* cpiTransfer, cpi_config_t *masterConfig){
    for(int i = 0; i < frames; i++){
        HIMAX_Mode(HIMAX_Streaming);
        CPI_Enable(cpi_address[0], masterConfig);
        CPI_ReceptionBlocking(cpi_address[0], cpiTransfer);
        CPI_Disable(cpi_address[0]);
    }
}

int main()
{
    printf("Fabric controller code execution for mbed_os HIMAX driver test\n");

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
    cpi_config.row_len         = (QVGA_W+4)/2 - 1; //Used only on slice mode
    /* HIMAX_MONO_COLOR */
    cpi_config.format          = BYPASS_BIGEND;
    cpi_config.shift           = DISABLE;
    cpi_config.slice_en        = DISABLE;
    cpi_config.frameDrop_en    = DISABLE;
    cpi_config.frameDrop_value = 0;
    cpi_config.wordWidth       = 16;

    printf("Ready to take a photo\n");
    while (((volatile ImgDescriptor *) &ImageOutHeader)->camReady == 0) {};
    printf("Taking picture...\n");

    cpi_transfer_t cpiTransfer;
    cpiTransfer.data        = image_buffer + PPM_HEADER;
    cpiTransfer.dataSize    = CAM_PIXEL_IMAGE_SIZE;
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1); //0 -> 8bit //1 -> 16bit //2 -> 32bit

    /* Skip several frames */
    skip_frames(2, &cpiTransfer, &cpi_config);

    HIMAX_Mode(HIMAX_Streaming);

    /* Enable and configure channel */
    CPI_Enable(cpi_address[0], &cpi_config);

    /* Start channel */
    cpiTransfer.data        = image_buffer + PPM_HEADER;
    cpiTransfer.dataSize    = CAM_PIXEL_IMAGE_SIZE;
    cpiTransfer.configFlags = UDMA_CFG_DATA_SIZE(1); //0 -> 8bit //1 -> 16bit //2 -> 32bit
    CPI_ReceptionBlocking(cpi_address[0], &cpiTransfer);

    /* Disable camera and UDMA channel */
    HIMAX_Mode(HIMAX_Standby);
    CPI_Disable(cpi_address[0]);
    CPI_Deinit(cpi_address[0]);

    printf("Picture taken!\n");
    ImageOutHeader.camReady = 0;
    CreatePPMHeader((image_buffer + PPM_HEADER), &ImageOutHeader, CAM_W_RES, CAM_H_RES);
    while (((volatile ImgDescriptor *) &ImageOutHeader)->imgReady == 1) {};

    return 0;
}
