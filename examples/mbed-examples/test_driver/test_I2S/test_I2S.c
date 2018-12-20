// test I2S interface
// 2 channels using the traffic generator in test bench configured by I2C
// PDM mode chan1 and chan0, using vip i2s

#include <stdio.h>
#include <string.h>
#include "gap_sai.h"


#define SAMPLE_FREQ 22050
#define SOC_FREQ_COEFF 25 // at soc_freq=50MHz, I2S_CLK=2MHz

// 65408 16b samples @ 22.05kHz eq. to 2.99/2=1.495 sec of audio record in stereo
#define BUFFER_SIZE  ((32768*2)-128)
#define WAV_HEADER_SIZE 44

/*
 * Test I2S:
 * 0-00 = I2S0 Mono
 * 1-01 = I2S1 Mono
 * 2-10 = I2S0 Stereo
 * 3-11 = I2S1 Stereo
 *
 */


static uint8_t i2s_config = 0;

/* I2S0 */
/* Clock register */
#define CK_DIV_CHAN0                   ((SOC_FREQ_COEFF)>>1)
#define WORD_SIZE_CHAN0                16
#define EN_CHAN0                       ((i2s_config == 0) || (i2s_config == 2))

/* Mode register */
#define LSB_FIRST_CHAN0                0
#define PDM_EN_CHAN0                   1
#define PDM_FILT_EN_CHAN0              1
#define PDM_DDR_CHAN0                  (i2s_config == 2)

/* Filter register */
#define DECIMATION_CHAN0               99 // 250 - 8kHZ; 99 - 22.05kHz with i2s_clk=2MHz
#define SHIFT_CHAN0                    4

/* I2S1 */
/* Clock register */
#define CK_DIV_CHAN1                   ((SOC_FREQ_COEFF)>>1)
#define WORD_SIZE_CHAN1                16
#define EN_CHAN1                       !EN_CHAN0

/* Mode register */
#define LSB_FIRST_CHAN1                0
#define PDM_EN_CHAN1                   1
#define PDM_FILT_EN_CHAN1              1
#define PDM_DDR_CHAN1                  (i2s_config == 3)

/* Filter register */
#define DECIMATION_CHAN1               99 // 250 - 8kHZ; 99 - 22.05kHz with i2s_clk=2MHz
#define SHIFT_CHAN1                    4

typedef struct {
    uint32_t    l2Addr;
    uint32_t    size;
    uint32_t    recReady;
    uint32_t    micReady;
} I2sDescriptor;

GAP_L2_DATA I2sDescriptor I2sOutHeader;

GAP_L2_DATA uint8_t I2S_BUFFER_CH0[(BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE];

static I2S_Type *const i2s_address[] = I2S_BASE_PTRS;

static void CreateWAVHeader(uint8_t *I2sOut, I2sDescriptor *I2sOutHeader);

int main() {
    /*
     * Pin configuration.
     * SDI : Choose either I2S1_SDI_B14 ou I2S1_SDI_B13 for I2S1
     *
     * Mode configutration.
     * Always use uSAI_CLK0_INT_WS for internal clock, for I2S0 and I2S1.
     *
     * Clock configuration.
     * Always configure I2S0's clock.
     */
    if(EN_CHAN1)
    {
        /* I2S Init */
        SAI_Init(i2s_address[0], I2S1_SDI_B13, I2S1_WS, I2S1_SCK);

        /* I2S Filter Configuration  */
        SAI_FilterConfig(i2s_address[0], uSAI_Channel1, DECIMATION_CHAN1 - 1, SHIFT_CHAN1);

        /* I2S Mode Configuration  */
        SAI_ModeConfig(i2s_address[0], uSAI_Channel1, LSB_FIRST_CHAN1, PDM_FILT_EN_CHAN1,
                       PDM_EN_CHAN1, PDM_DDR_CHAN1, uSAI_CLK0_INT_WS);

        /* I2S Clock Configuration  */
        SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN1, EN_CHAN1, 0x1f);
    }

    if(EN_CHAN0)
    {
        /* I2S Init */
        SAI_Init(i2s_address[0], I2S0_SDI, I2S0_WS, I2S0_SCK);

        /* I2S Filter Configuration  */
        SAI_FilterConfig(i2s_address[0], uSAI_Channel0, DECIMATION_CHAN0 - 1, SHIFT_CHAN0);

        /* I2S Mode Configuration  */
        SAI_ModeConfig(i2s_address[0], uSAI_Channel0, LSB_FIRST_CHAN0, PDM_FILT_EN_CHAN0,
                       PDM_EN_CHAN0, PDM_DDR_CHAN0, uSAI_CLK0_INT_WS);

        /* I2S Clock Configuration  */
        SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, EN_CHAN0, 0x1f);
    }

    printf("I2S audio rec start on I2S%d, mode %s\n", ((EN_CHAN0) ? 0 : 1),  ((i2s_config >= 2) ? "Stereo" : "Mono") );

    for(;;) {
        printf("Ready to record audio from microphones\n");
        while (((volatile I2sDescriptor *) &I2sOutHeader)->micReady == 0) {};
        printf("*********************\n");
        printf("Start recording audio\n");
        printf("*********************\n");

        sai_transfer_t xfer;
        xfer.data        = (uint8_t*) (I2S_BUFFER_CH0 + WAV_HEADER_SIZE);
        xfer.dataSize    = BUFFER_SIZE * sizeof(short);
        xfer.configFlags = 0x12;
        xfer.channel     = ( (EN_CHAN0) ? uSAI_Channel0 : uSAI_Channel1);

        /* Start tranfering RX */
        SAI_TransferReceiveBlocking(i2s_address[0], &xfer);

        (&I2sOutHeader)->micReady = 0;

        CreateWAVHeader(I2S_BUFFER_CH0, &I2sOutHeader);

        while (((volatile I2sDescriptor *) &I2sOutHeader)->recReady == 1) {};

        for (int i=0; i<128; i+=2)
            printf("I2S_BUFFER_CH0[L-%0x:R-%0x]=0x%02x%02x - 0x%02x%02x\n",
                   i+1, i,
                   I2S_BUFFER_CH0[(i+1)*2+1],
                   I2S_BUFFER_CH0[(i+1)*2],
                   I2S_BUFFER_CH0[i*2+1],
                   I2S_BUFFER_CH0[i*2]);
    }

    printf("I2S RX in mode PDM done\n");

    return 0;
}

static void CreateWAVHeader(uint8_t *I2sOut, I2sDescriptor *I2sOutHeader) {
    uint8_t header_buffer[WAV_HEADER_SIZE];
    uint32_t idx = 0;

    // 4 bytes "RIFF"
    header_buffer[idx++] = 'R';
    header_buffer[idx++] = 'I';
    header_buffer[idx++] = 'F';
    header_buffer[idx++] = 'F';

    // 4 bytes File size - 8bytes 32kS 0x10024 - 65408S 0x1ff24
    header_buffer[idx++] = 0x24;
    header_buffer[idx++] = 0xff;
    header_buffer[idx++] = 0x01;
    header_buffer[idx++] = 0x00;

    // 4 bytes file type: "WAVE"
    header_buffer[idx++] = 'W';
    header_buffer[idx++] = 'A';
    header_buffer[idx++] = 'V';
    header_buffer[idx++] = 'E';

    // 4 bytes format chunk: "fmt " last char is trailing NULL
    header_buffer[idx++] = 'f';
    header_buffer[idx++] = 'm';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = ' ';

    // 4 bytes length of format data above: 16
    header_buffer[idx++] = 0x10;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;

    // 2 bytes type of format: 1 (PCM)
    header_buffer[idx++] = 0x01;
    header_buffer[idx++] = 0x00;

    // 2 bytes nb of channels: 1 or 2
    //header_buffer[idx++] = 0x02;
    //header_buffer[idx++] = 0x01;
    header_buffer[idx++] = (i2s_config >= 2) ? 0x02 : 0x01;
    header_buffer[idx++] = 0x00;

    // 4 bytes sample rate in Hz: 8kHz 0x1f40 - 22.05kHz 0x5622
    //header_buffer[idx++] = 0x22;
    //header_buffer[idx++] = 0x56;
    header_buffer[idx++] = 0x40;
    header_buffer[idx++] = 0x1f;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;

    // 4 bytes (Sample Rate * BitsPerSample * Channels) / 8:
    // (8000*16*1)/8=0x3e80
    // (22050*16*1)/8=0xac44
    // (22050*16*2)/8=0x15888
    //
    // 2 bytes (BitsPerSample * Channels) / 8:
    // 16*1/8=2 - 16b mono
    // 16*2/8=4 - 16b stereo
    if(i2s_config >= 2)
    {
        header_buffer[idx++] = 0x88;
        header_buffer[idx++] = 0x58;
        header_buffer[idx++] = 0x01;
        header_buffer[idx++] = 0x00;
        header_buffer[idx++] = 0x04;
        header_buffer[idx++] = 0x00;
    } else {
        header_buffer[idx++] = 0x44;
        header_buffer[idx++] = 0xac;
        header_buffer[idx++] = 0x00;
        header_buffer[idx++] = 0x00;
        header_buffer[idx++] = 0x02;
        header_buffer[idx++] = 0x00;
    }

    // 2 bytes (BitsPerSample * Channels) / 8:
    // 16*1/8=2 - 16b mono
    // 16*2/8=4 - 16b stereo
    //header_buffer[idx++] = 0x04;
    //header_buffer[idx++] = 0x02;
    //header_buffer[idx++] = 0x00;

    // 2 bytes bit per sample: 16
    header_buffer[idx++] = 0x10;
    header_buffer[idx++] = 0x00;

    // 4 bytes "data" chunk
    header_buffer[idx++] = 'd';
    header_buffer[idx++] = 'a';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = 'a';

    // 4 bytes size of data section in bytes: 65408*2 bytes
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0xff;
    header_buffer[idx++] = 0x01;
    header_buffer[idx++] = 0x00;

    for (int i=0; i<WAV_HEADER_SIZE; i++)
        I2sOut[i] = header_buffer[i];

    I2sOutHeader->l2Addr = (uint32_t) I2sOut;
    I2sOutHeader->size = (BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE;

    I2sOutHeader->recReady = 1;
}
