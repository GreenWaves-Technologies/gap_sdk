#include <stdio.h>
#include "AUDIO.h"

#define SAMPLE_FREQ 16000
#define SOC_FREQ_COEFF 25 // at soc_freq=50MHz, I2S_CLK=2MHz

/* I2S0 */
/* Clock register */
#define WORD_SIZE_CHAN0                16
#define EN_CHAN0                       1

/* Mode register */
#define LSB_FIRST_CHAN0                0
#define PDM_EN_CHAN0                   1
#define PDM_FILT_EN_CHAN0              1
#define PDM_DDR_CHAN0                  0

/* Filter register */
#define DECIMATION_CHAN0               0x40 // 250 - 8kHZ; 99 - 22.05kHz with i2s_clk=2MHz
#define SHIFT_CHAN0                    5


typedef struct i2s_descriptor_ {
    uint32_t    l2Addr;
    uint32_t    size;
    uint32_t    recReady;
    uint32_t    micReady;
    uint32_t    cStarted;
} i2s_descriptor_t;

i2s_descriptor_t i2s_out_header;

static I2S_Type *const i2s_address[] = I2S_BASE_PTRS;

static void WAV_HeaderCreate(uint8_t *i2s_out, i2s_descriptor_t *i2s_out_header)
{
    uint8_t header_buffer[WAV_HEADER_SIZE];
    uint32_t idx = 0;
    unsigned int sz = WAV_HEADER_SIZE + BUFFER_SIZE * sizeof(signed short);

    // 4 bytes "RIFF"
    header_buffer[idx++] = 'R';
    header_buffer[idx++] = 'I';
    header_buffer[idx++] = 'F';
    header_buffer[idx++] = 'F';

    // 4 bytes File size - 8bytes 32kS 0x10024 - 65408S 0x1ff24
    header_buffer[idx++] = (unsigned char) (sz & 0x000000ff);
    header_buffer[idx++] = (unsigned char)((sz & 0x0000ff00) >> 8);
    header_buffer[idx++] = (unsigned char)((sz & 0x00ff0000) >> 16);
    header_buffer[idx++] = (unsigned char)((sz & 0xff000000) >> 24);

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
    header_buffer[idx++] = 0x01;
    header_buffer[idx++] = 0x00;

    // 4 bytes sample rate in Hz: 8kHz 0x1f40 - 22.05kHz 0x5622 16kHz=0x3e80
    //header_buffer[idx++] = 0x22;
    //header_buffer[idx++] = 0x56;
    header_buffer[idx++] = 0x80;
    header_buffer[idx++] = 0x3e;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;

    // 4 bytes (Sample Rate * BitsPerSample * Channels) / 8:
    // (8000*16*1)/8=0x3e80 * 2
    // (16000*16*1)/8=32000 or 0x6F00
    // (22050*16*1)/8=0xac44
    // (22050*16*2)/8=0x15888
    /*header_buffer[idx++] = 0x88;
      header_buffer[idx++] = 0x58;
      header_buffer[idx++] = 0x01;
      header_buffer[idx++] = 0x00;*/
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x6f;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;

    // 2 bytes (BitsPerSample * Channels) / 8:
    // 16*1/8=2 - 16b mono
    // 16*2/8=4 - 16b stereo
    //header_buffer[idx++] = 0x04;
    header_buffer[idx++] = 0x02;
    header_buffer[idx++] = 0x00;

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
        i2s_out[i] = header_buffer[i];

    i2s_out_header->l2Addr = (uint32_t) i2s_out;
    i2s_out_header->size   = (BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE;

    i2s_out_header->recReady = 1;
}

void Audio_Init()
{
    /* I2S Init */
    SAI_Init(i2s_address[0], I2S0_SDI, I2S0_WS, I2S0_SCK);

    /* I2S Filter Configuration  */
    SAI_FilterConfig(i2s_address[0], uSAI_Channel0, DECIMATION_CHAN0 - 1, SHIFT_CHAN0);

    /* I2S Mode Configuration  */
    SAI_ModeConfig(i2s_address[0], uSAI_Channel0, LSB_FIRST_CHAN0, PDM_FILT_EN_CHAN0,
                   PDM_EN_CHAN0, PDM_DDR_CHAN0, uSAI_CLK0_INT_WS);

}

void Audio_Enable()
{
    /* I2S Clock Enable */
    SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, EN_CHAN0, 0x17);
}

void Audio_Disable()
{
    /* I2S Clock disable */
    SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, 0, 0x17);
}

void Audio_DeInit()
{
    Audio_Disable();

    /* Device Clock Gating */
    SAI_Deinit(i2s_address[0]);
}

void Audio_Capture()
{
    i2s_out_header.cStarted = 1;

    /* Clear buffer */
    memset(i2s_buffer, 0, (BUFFER_SIZE * sizeof(short)) + WAV_HEADER_SIZE);

    printf("Ready to record audio from microphones\n");

    /* Synchronous by python */
    while (((volatile i2s_descriptor_t *) &i2s_out_header)->micReady == 0) {};

    printf("*********************\n");
    printf("Start recording audio\n");
    printf("*********************\n");

    i2s_out_header.cStarted = 0;

    sai_transfer_t xfer;
    xfer.data        = (uint8_t*) (i2s_buffer + WAV_HEADER_SIZE);
    xfer.dataSize    = BUFFER_SIZE * sizeof(short);
    xfer.configFlags = 0x12;
    xfer.channel     = uSAI_Channel0;

    /* Start tranfering RX */
    SAI_TransferReceiveBlocking(i2s_address[0], &xfer);

    /* Synchronous by python */
    i2s_out_header.micReady = 0;
    WAV_HeaderCreate(i2s_buffer, &i2s_out_header);

    printf("I2S RX in mode PDM done\n");
}


void Audio_SynchroPython()
{
    i2s_out_header.recReady = 1;
    while(((volatile i2s_descriptor_t*) &i2s_out_header)->recReady == 1);
}
