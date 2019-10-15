/*
 * Test two-channel CSI2 read including two-channel slice mode,
 * To test in modelsim environment, please modify the file:
 *  <sim/vsim/tcl_files/config/vsim.tcl>, line:
 * "+UVM_TESTNAME=csi2_rx_pkt_raw8" => "+UVM_TESTNAME=csi2_rx_pkt_vc_raw8"
 * which will generate 2 VC channels.
 */

// MBED OS
#include "cmsis_os2.h"
#include "gap_common.h"
#include "mbed_wait_api.h"

#define LANE    2
#define PCLK    46870000

#define CAM_H_RES     640
#define CAM_V_RES     10

/* User can change here according to different format */
#define RAW8    1

/* Image Format */
#if defined   (RAW6)
#define BITS_PER_PIXEL      8
#elif defined (RAW7)
#define BITS_PER_PIXEL      8
#elif defined (RAW8)
#define BITS_PER_PIXEL      8
#elif defined (RAW10)
#define BITS_PER_PIXEL      16
#elif defined (RAW12)
#define BITS_PER_PIXEL      16
#elif defined (RAW14)
#define BITS_PER_PIXEL      16
#elif defined (RGB444)
#define BITS_PER_PIXEL      16
#elif defined (RGB555)
#define BITS_PER_PIXEL      16
#elif defined (RGB565)
#define BITS_PER_PIXEL      16
#elif defined (RGB666)
#define BITS_PER_PIXEL      64/3
#elif defined (RGB888)
#define BITS_PER_PIXEL      32
#elif defined (LEG_YUV420_8)
#define BITS_PER_PIXEL      16
#elif defined (YUV420_8)
#define BITS_PER_PIXEL      15
#elif defined (YUV420_8_CSPS)
#define BITS_PER_PIXEL      15
#elif defined (YUV420_10)
#define BITS_PER_PIXEL      24
#elif defined (YUV420_10_CSPS)
#define BITS_PER_PIXEL      24
#elif defined (YUV422_8)
#define BITS_PER_PIXEL      16
#elif defined (YUV422_10)
#define BITS_PER_PIXEL      32
#endif

#define BYTE_PER_PIXEL   (BITS_PER_PIXEL / 8)
#define FRAME_SIZE       (CAM_V_RES * CAM_H_RES * BYTE_PER_PIXEL)
#define PIXEL_PER_CYCLE  (8 / BYTE_PER_PIXEL)

/* Slice Example, read pixels in rectangle [(160, 3),  (480, 7)] */
#define LL0_X             160
#define LL0_Y             3
#define UR0_X             480
#define UR0_Y             7
#define SLICE0_SIZE       ((UR0_X-LL0_X + PIXEL_PER_CYCLE) * (UR0_Y-LL0_Y + 1))

#define LL1_X             80
#define LL1_Y             1
#define UR1_X             240
#define UR1_Y             5
#define SLICE1_SIZE       ((UR1_X-LL1_X + PIXEL_PER_CYCLE) * (UR1_Y-LL1_Y + 1))

int     error = 0;
uint8_t frame_counter[2] = {0, 0};
uint8_t rx_buffer[FRAME_SIZE];

/* Variables used. */
/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

void test_gpio_init (PinName pin, gpio_pin_direction_t direction)
{
    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT(pin);
    uint32_t gpio_number = GET_GPIO_NUM(pin);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(pin);

    PORT_SetPinMux(port_addrs[0], pin_number,  uPORT_MuxGPIO);

    /* Init GPIO - OUTPUT. */
    gpio_pin_config_t gpio_config = { .pinDirection  = direction,
                                      .outputLogic   = uGPIO_LOW,
                                      .pullSelect    = uGPIO_PullUpEnable,
                                      .driveStrength = uGPIO_LowDriveStrength,
    };

    GPIO_PinInit ( gpio_addrs[port_number], gpio_number, &gpio_config );
}

void test_gpio_write (PinName pin, gpio_pin_state_t state)
{
    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT(pin);
    uint32_t gpio_number = GET_GPIO_NUM(pin);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(pin);

    /* Write 1 to GPIO. */
    GPIO_WritePinOutput( gpio_addrs[port_number], gpio_number, state );
}

void csi2_read(uint32_t l2_addr, int size, int channel)
{
    if(channel) {
        CSI2->UDMA_CSI2.TX_SADDR = l2_addr;
        CSI2->UDMA_CSI2.TX_SIZE  = size;
        CSI2->UDMA_CSI2.TX_CFG   = UDMA_CFG_EN(1);
    } else {
        CSI2->UDMA_CSI2.RX_SADDR = l2_addr;
        CSI2->UDMA_CSI2.RX_SIZE  = size;
        CSI2->UDMA_CSI2.RX_CFG   = UDMA_CFG_EN(1);
    }
}

void CSI2_RX0_IRQHandler() {
    frame_counter[0]++;

    /* Check error for each frame */
    if (CSI2_PIXEL->ERR_LSB != 0x00 || CSI2_PIXEL->ERR_MSB != 0x04 || CSI2_PIXEL->ERR_MSB1 != 0x00) {
        error++;
        return;
    }

    if (frame_counter[0] < 3) {
        csi2_read((uint32_t)rx_buffer, FRAME_SIZE, 0);
    } else {
        CSI2->SLICE_ROWLEN |= (CAM_H_RES / PIXEL_PER_CYCLE - 1);
        CSI2->SLICE0_LXY = ( (LL0_X / PIXEL_PER_CYCLE) << 16) | LL0_Y;
        CSI2->SLICE0_RXY = ( (UR0_X / PIXEL_PER_CYCLE) << 16) | UR0_Y;
        CSI2->SLICE_ENABLE |= CSI2_SLICE_ENABLE_CH0(1);

        csi2_read((uint32_t)rx_buffer, SLICE0_SIZE, 0);
    }
}

void CSI2_RX1_IRQHandler() {
    frame_counter[1]++;

    /* Check error for each frame */
    if (CSI2_PIXEL->ERR_LSB != 0x00 || CSI2_PIXEL->ERR_MSB != 0x04 || CSI2_PIXEL->ERR_MSB1 != 0x00) {
        error++;
        return;
    }

    if (frame_counter[0] < 5) {
        csi2_read((uint32_t)rx_buffer, FRAME_SIZE, 1);
    } else {
        CSI2->SLICE_ROWLEN |= ((CAM_H_RES / PIXEL_PER_CYCLE - 1) << 16);
        CSI2->SLICE1_LXY = ( (LL1_X / PIXEL_PER_CYCLE) << 16) | LL1_Y;
        CSI2->SLICE1_RXY = ( (UR1_X / PIXEL_PER_CYCLE) << 16) | UR1_Y;
        CSI2->SLICE_ENABLE |= CSI2_SLICE_ENABLE_CH1(1);

        csi2_read((uint32_t)rx_buffer, SLICE1_SIZE, 1);
    }
}

void csi2_init(void)
{
    /* Power Up */
    PMU_CSI2PowerOn();

    /* Disable clock gating */
    UDMA_GC->CG |= 1 << PER_ID_CSI2;

    /* set event mask */
    SOC_EU_SetFCMask(UDMA_EVENT_CSI2_RX0);
    SOC_EU_SetFCMask(UDMA_EVENT_CSI2_RX1);

    /* 2 lane raw 8 : pixel_clk = 42.67/2 ns (46.87 Mhz), apb_clk = 10 Mhz, cci_clk = 400 Khz */
    CSI2->CLK_DIV = CSI2_CLK_DIV_PIXEL(0) | CSI2_CLK_DIV_APB(5) | CSI2_CLK_DIV_CCI(117);

    CSI2->RX_ENABLE = CSI2_RX_ENABLE_CH0(0);
}

void csi2_deinit(void)
{
    CSI2->RX_ENABLE = CSI2_RX_ENABLE_CH0(0);

    /* Power Down */
    PMU_CSI2PowerOff();

    /* Disable clock gating */
    UDMA_GC->CG &= ~(1 << PER_ID_CSI2);

    /* set event mask */
    SOC_EU_ClearFCMask(UDMA_EVENT_CSI2_RX0);
    SOC_EU_ClearFCMask(UDMA_EVENT_CSI2_RX1);
}

void csi2_phy_config(void)
{
    printf("PHY 00 = 0x%x \n", CSI2_DPHY->REG00);
    printf("PHY 4A = 0x%x \n", CSI2_DPHY->REG4a);
    printf("PHY 7A = 0x%x \n", CSI2_DPHY->REG7a);

    /* Enable lane and clock */
    CSI2_DPHY->REG00 = 0x7D;

    /* Config clock0 */
    CSI2_DPHY->REG4a = 0x3F;
    /* Config lane0 */
    CSI2_DPHY->REG7a = 0xFF;

    if(LANE == 2) {
        /* Config lane1 */
        CSI2_DPHY->REG9a = 0xFF;
    }

    printf("PHY 00 = 0x%x \n", CSI2_DPHY->REG00);
    printf("PHY 4A = 0x%x \n", CSI2_DPHY->REG4a);
    printf("PHY 7A = 0x%x \n", CSI2_DPHY->REG7a);
}

void csi2_pixel_config(void)
{
    /* Virtual Channel 0 Enabled */
    CSI2_PIXEL->VCCFG  = 0x3;
    /* Number of lanes :
       00 - Single lane;
       01 - Two lane;
       11 - Four lane
    */
    CSI2_PIXEL->CONFIG = (LANE == 2) ? 0x01 : 0x00;
}

int csi2_pixel_read(void)
{
    CSI2->RX_ENABLE = CSI2_RX_ENABLE_CH1(1) | CSI2_RX_ENABLE_CH0(1);

    csi2_read((uint32_t)rx_buffer, FRAME_SIZE, 0);
    csi2_read((uint32_t)rx_buffer, FRAME_SIZE, 1);

    while(!error && frame_counter[0] < 8) {
        ITC_WaitEvent_IRQ(FC_SOC_EVENT_IRQn);
    }

    return error;
}

int main()
{
    printf("Fabric controller code execution for mbed_os CSI2 driver test\n");

    FLL_SetFrequency(uFLL_SOC, PCLK * LANE, 0);

    csi2_init();

    /* CSI2 PHY configure */
    csi2_phy_config();

    /* CSI2 PIXEL configure */
    csi2_pixel_config();

    /* Trigger GPIO to enable CSI2 stream
     * In real practice, we use i3c/i2c to enable ouside camera device stream
     */
    test_gpio_init  (GPIO_A[0], uGPIO_DigitalOutput);
    test_gpio_write (GPIO_A[0], uGPIO_HIGH);

    /* Read pixel */
    csi2_pixel_read();

    csi2_deinit();

    if (error) printf("Test failed %d errors\n", error);
    else printf("Test success\n");

    return error;
}
