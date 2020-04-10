/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// test I2S interface
// IP vocal 128 samples buffer
// 2 channels using the traffic generator in test bench configured by I2C
// PDM mode chan1 and chan0, using vip i2s

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gap_sai.h"

//#define VOCAL
//#define PERF
//#define MONITOR_VOCALIP_RES
//#define MONITOR_FC_ALIVE
//define L2STACK_DBG

#ifdef FORCE_INPUT
#include "DataTest.h"
#endif

//65536 16b samples @ 16kHz eq. to 2.01 sec of total audio record with 1 I2S
#define WAV_HEADER_SIZE 44
#define CHUNK_SIZE 128
#define PREFIX_LEN 5
#define NB_SUBUF_SAMPLES (CHUNK_SIZE*2) //128samples by mic so 256 by IS2x ;here in 16bits, *2 to convert in bytes
#define NB_SMPL_PERSEC (16000) // nb of samples per sec
#define NB_CHUNK_PERSEC (NB_SMPL_PERSEC/CHUNK_SIZE) // nb of chunks per sec
// input kws is a 1.5s length buffer (from wich a 1s buffer will be rocessed by KWS)
#define OUTPUT_BUF_SIZE (NB_SMPL_PERSEC + PREFIX_LEN * CHUNK_SIZE) //here in 16bits, *2 to convert in bytes
#define NB_CHUNK (OUTPUT_BUF_SIZE/CHUNK_SIZE)
/*
 * Test I2S:
 * 0-00 = I2S0 Mono
 * 1-01 = I2S1 Mono
 * 2-10 = I2S0 Stereo
 * 3-11 = I2S1 Stereo
 *
 */
#define I2S0_STEREO 2
#define I2S1_STEREO 3
#define EN_CHAN0                       1

/* I2S0 */
/* Clock register */
#define CK_DIV_CHAN0                   ((SOC_FREQ_COEFF)>>1)
#define WORD_SIZE_CHAN0                16

/* Mode register */
#define LSB_FIRST_CHAN0                0
#define PDM_EN_CHAN0                   1
#define PDM_FILT_EN_CHAN0              1
#define PDM_DDR_CHAN0                  1

/* Filter register */
// I2S clock div factor: assuming SOC clock is 50Mhz => I2S clock is (50Mhz/2) / (CK_DIF_FAC + 1) = 1.04 Mhz (~  64*16Khz)
#define CK_DIV_FAC 0x17

/* I2S1 */
/* Clock register */
#define CK_DIV_CHAN1                   ((SOC_FREQ_COEFF)>>1)
#define WORD_SIZE_CHAN1                16

/* Mode register */
#define LSB_FIRST_CHAN1                0
#define PDM_EN_CHAN1                   1
#define PDM_FILT_EN_CHAN1              1
#define PDM_DDR_CHAN1                  1

#ifndef GAP_SOURCE_SCK0
/* Filter register */
#define DECIMATION_CHAN1               0x40 //0x80 =>2048k => 16kHz
#define SHIFT_CHAN1                    4
#define DECIMATION_CHAN0               0x40 //0x80 => 2048Khz => 16kHz (64)
// SHIFT is 51 - 16 - 5*SHIFT_CHAN
#define SHIFT_CHAN0                    4

#else
/* Filter register */
#define DECIMATION_CHAN1               0x40 //0x40 => 1024k => 16kHz
#define SHIFT_CHAN1                    5
#define DECIMATION_CHAN0               0x40 //0x40 => 1024Khz => 16kHz (64)
// SHIFT is 51 - 16 - 5*SHIFT_CHAN
#define SHIFT_CHAN0                    5
#endif


#define OFFSET_BU 20

#define abs(a)  (((a)<0) ? (-(a)) : (a))


typedef struct {
  uint32_t    l2Addr;
  uint32_t    size;
  uint32_t    recReady;
  uint32_t    micReady;
  uint32_t    cStarted;
  uint32_t    l2Addr1;
  uint32_t    size1;
  uint32_t    l2AddrVoc;
  uint32_t    sizeVoc;
  uint32_t    startIdx;
  uint32_t    l2AddrMic1;
  uint32_t    l2AddrMic2;
  uint32_t    sizeVoc2;
} I2sDescriptor;

// buffer for IP output
typedef struct {
  uint8_t header[WAV_HEADER_SIZE];
  signed short buff[OUTPUT_BUF_SIZE];
} outputBuf_t;

// buffer for direct microphone output
typedef struct {
  uint8_t header[WAV_HEADER_SIZE];
  signed short buff[2*OUTPUT_BUF_SIZE];
} micBuf_t;

typedef enum _i2s_buf { buf0_i2s0, buf1_i2s0, buf0_i2s1, buf1_i2s1 } i2s_buf;

GAP_L2_DATA signed short audioBuf[2][NB_SUBUF_SAMPLES];
GAP_L2_DATA signed short audioBuf1[2][NB_SUBUF_SAMPLES];
GAP_L2_DATA outputBuf_t vocalOutput;

#if 0
#ifdef RECORD_MIC
GAP_L2_DATA micBuf_t micOutput1, micOutput2;
#else
#ifdef KWS
GAP_L2_DATA outputBuf_t micOutput1;
//GAP_L2_DATA short micOutput1[1];
#endif
#endif
#endif

signed short* audioBuff;

GAP_L2_DATA I2sDescriptor I2sOutHeader = { .l2Addr = 0, .size = 0, .recReady = 0,
  .micReady = 0, .cStarted = 0, .l2Addr1 = 0, .size1 = 0, .l2AddrMic1 = 0,.l2AddrMic2 = 0, .sizeVoc2 = 0 };
GAP_L2_DATA uint32_t end = 0;

static I2S_Type *const i2s_address[] = I2S_BASE_PTRS;

GAP_L2_DATA   sai_transfer_t xfer0I2S0, xfer1I2S0, xfer0I2S1, xfer1I2S1;
GAP_L2_DATA   sai_handle_t handle0I2S0, handle1I2S0, handle0I2S1, handle1I2S1;
void rxEndI2S0(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData);
void rxEndI2S1(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData);
GAP_L2_DATA   sai_transfer_callback_t callback0I2S0 = rxEndI2S0, callback1I2S0 = rxEndI2S0;
GAP_L2_DATA   sai_transfer_callback_t callback0I2S1 = rxEndI2S1, callback1I2S1 = rxEndI2S1;
GAP_L2_DATA   i2s_buf param00=buf0_i2s0, param10=buf1_i2s0, param01=buf0_i2s1, param11=buf1_i2s1;
GAP_L2_DATA uint32_t buf00=0, buf10=0, buf01=0, buf11=0;
GAP_L2_DATA uint32_t nbIT0=0, nbIT1=0, cntC=0;

#ifdef PERF
int perfTab[2][512];
unsigned int perfCnt=0;
performance_t perf;
#endif

int word1,word2;
int resCnt=0;
int THRESH_VAD = 0;
unsigned char modecal=1;
int av_energy=0;
int idxmax=-1, idxmin=-1;
unsigned char found_voice = 0;
int id0,id1;
int idxbuf=-1, start_frame=0, cntBuf=0, cntRes=0;
int res = 0, last_res=0;
int store = 0;
int maxval=0;

//
unsigned char toggle=0, stat_log;
unsigned char okgo=0;
int irq;

signed short logscore[12];

GAP_L2_DATA static unsigned char resTab[NB_CHUNK] = {[0 ... NB_CHUNK-1] = 0x0};

extern void exit(int code);




static void CreateWAVHeader(uint8_t *I2sOut, I2sDescriptor *I2sOutHeader, uint32_t channel, uint32_t);

/* vocal declaration*/
typedef signed short fract;
void init_dsp_process(fract threshold);
int dsp_process_block(fract *input_mics, fract *input_spk, fract *output, int mics, int samples);

#if L2STACK_DBG
extern uint8_t __l2Stack;
#endif

uint32_t rd_stack_pt()
{
  uint32_t st;
  asm volatile ("ori %0, sp, 0x00":"=r"(st));

  return st;
}

#ifdef L2STACK_DBG
void check_stack_overflow(char * fctName, uint32_t line)
{
  uint32_t stSize=4096;
  uint32_t max;
  max = ((uint32_t *) &__l2Stack) - stSize;
  if (rd_stack_pt() < max)
  {
    printf("stack overflow in %s at line %d\n", fctName, (unsigned int)line);
    printf("stack initial addr 0x%x last current stack pointer: 0x%x max_addr 0x%x\n", (unsigned int *) &__l2Stack, (unsigned int)rd_stack_pt(), (unsigned int)max);
  }
}

void check_stack_overflow_dbg(char * fctName, uint32_t line)
{
  uint32_t stSize=4096;
  uint32_t max = ((uint32_t *) &__l2Stack) - stSize;
  //if (rd_stack_pt() < max)
  {
    //printf("stack overflow in %s at line %d\n", fctName, line);
    printf("stack initial addr 0x%x last current stack pointer: 0x%x max_addr 0x%x\n", (unsigned int *) &__l2Stack, (unsigned int)rd_stack_pt(), max);
  }
}
#else
void check_stack_overflow(char * fctName, uint32_t line) {;}
void check_stack_overflow_dbg(char * fctName, uint32_t line) {;}
#endif


void prepI2sRx()
{
  xfer0I2S0.data        = (uint8_t*) (&audioBuf[0]);
  xfer0I2S0.dataSize    = NB_SUBUF_SAMPLES * sizeof(short);
  xfer0I2S0.configFlags = 0x12;
  xfer0I2S0.channel     = uSAI_Channel0;

  xfer1I2S0.data        = (uint8_t*) (&audioBuf[1]);
  xfer1I2S0.dataSize    = NB_SUBUF_SAMPLES * sizeof(short);
  xfer1I2S0.configFlags = 0x12;
  xfer1I2S0.channel     = uSAI_Channel0;

  xfer0I2S1.data        = (uint8_t*) (&audioBuf1[0]);
  xfer0I2S1.dataSize    = NB_SUBUF_SAMPLES * sizeof(short);
  xfer0I2S1.configFlags = 0x12;
  xfer0I2S1.channel     = uSAI_Channel1;

  xfer1I2S1.data        = (uint8_t*) (&audioBuf1[1]);
  xfer1I2S1.dataSize    = NB_SUBUF_SAMPLES * sizeof(short);
  xfer1I2S1.configFlags = 0x12;
  xfer1I2S1.channel     = uSAI_Channel1;

  SAI_TransferRxCreateHandle(i2s_address[0], &handle0I2S0, callback0I2S0, &param00);
  SAI_TransferRxCreateHandle(i2s_address[0], &handle1I2S0, callback1I2S0, &param10);
  SAI_TransferRxCreateHandle(i2s_address[0], &handle0I2S1, callback0I2S1, &param01);
  SAI_TransferRxCreateHandle(i2s_address[0], &handle1I2S1, callback1I2S1, &param11);
}

void init_I2S() {

      for(uint32_t i=0; i<NB_SUBUF_SAMPLES;i++)
	{
	  audioBuf[0][i] = 0;
	  audioBuf[1][i] = 0;
	  audioBuf1[0][i] = 0;
	  audioBuf1[1][i] = 0;
	}
      
      /* I2S1 Init */
      SAI_Init(i2s_address[0], I2S1_SDI_B14, I2S1_WS, I2S0_SCK); //gapuino I2S0_SCK used for both I2S
      
      /* I2S1 Filter Configuration  */
      SAI_FilterConfig(i2s_address[0], uSAI_Channel1, DECIMATION_CHAN1 - 1, SHIFT_CHAN1);
      
      /* I2S Mode Configuration  */
#ifdef GAP_SOURCE_SCK0
      SAI_ModeConfig(i2s_address[0], uSAI_Channel1, LSB_FIRST_CHAN1, PDM_FILT_EN_CHAN1, PDM_EN_CHAN1, PDM_DDR_CHAN1, uSAI_CLK0_INT_WS); //gapuino I2S0_SCK used for both I2S
#else
      SAI_ModeConfig(i2s_address[0], uSAI_Channel1, LSB_FIRST_CHAN1, PDM_FILT_EN_CHAN1, PDM_EN_CHAN1, PDM_DDR_CHAN1, 2); //gapuino I2S0_SCK used for both I2S
#endif
      /* I2S0 Init */
      SAI_Init(i2s_address[0], I2S0_SDI, I2S0_WS, I2S0_SCK);
      
      /* I2S0 Filter Configuration  */
      SAI_FilterConfig(i2s_address[0], uSAI_Channel0, DECIMATION_CHAN0 - 1, SHIFT_CHAN0);
 
      /* I2S Mode Configuration  */
#ifdef GAP_SOURCE_SCK0
      SAI_ModeConfig(i2s_address[0], uSAI_Channel0, LSB_FIRST_CHAN0, PDM_FILT_EN_CHAN0, PDM_EN_CHAN0, PDM_DDR_CHAN0, uSAI_CLK0_INT_WS);
#else
      SAI_ModeConfig(i2s_address[0], uSAI_Channel0, LSB_FIRST_CHAN0, PDM_FILT_EN_CHAN0, PDM_EN_CHAN0, PDM_DDR_CHAN0, 2);
#endif

      
      /* I2S Clock Configuration  void SAI_ClockConfig(I2S_Type *base, char ch_id, uint8_t bits, uint8_t en, uint16_t div) : I2S0 is master */
#ifdef GAP_SOURCE_SCK0 
      SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, 1, CK_DIV_FAC);
#else
      SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, 0, CK_DIV_FAC);
#endif

      //synchronyze with python
#ifdef PYTHON_SYNC  
      printf("c: Ready to record audio from microphones (synced wih python)\n");
      I2sOutHeader.cStarted = 1;
      
      while(((volatile I2sDescriptor *) &I2sOutHeader)->micReady == 0) {};
#endif
      
      
      // set to highet threshold: always active
      init_dsp_process(3000);
      
      prepI2sRx();
    

      // ===> commented out : this screws up the wake up on interrupt process <== WHY?
      SAI_TransferReceiveNonBlocking(i2s_address[0], &handle0I2S0, &xfer0I2S0);
      SAI_TransferReceiveNonBlocking(i2s_address[0], &handle0I2S1, &xfer0I2S1);
      SAI_TransferReceiveNonBlocking(i2s_address[0], &handle1I2S0, &xfer1I2S0);
      SAI_TransferReceiveNonBlocking(i2s_address[0], &handle1I2S1, &xfer1I2S1);
            

      
      for (int i=0;i<NB_SUBUF_SAMPLES;i++) {audioBuf[0][i]=0;audioBuf[1][i]=0;audioBuf1[0][i]=0;audioBuf1[1][i]=0;}


}


void createWavVocIP()
{
  uint32_t I2S0_MONO = 1; //output of VocalIP is in mono
  printf("in createWavVocIP\n");
  CreateWAVHeader((uint8_t *)vocalOutput.header, &I2sOutHeader, 0, I2S0_MONO);
  //CreateWAVHeader((uint8_t *)micOutput1.header, &I2sOutHeader, 0, I2S0_MONO);

  I2sOutHeader.recReady = 1;
  while (((volatile I2sDescriptor *) &I2sOutHeader)->recReady == 1) {};
}

#ifdef RECORD_MIC
void createWavFromBuffer()
{
  printf("dump microphone output\n");
  CreateWAVHeader((uint8_t *)micOutput1.header, &I2sOutHeader, 1, I2S0_STEREO);
  CreateWAVHeader((uint8_t *)micOutput2.header, &I2sOutHeader, 2, I2S1_STEREO);
  I2sOutHeader.recReady = 1;
  while (((volatile I2sDescriptor *) &I2sOutHeader)->recReady == 1) {};
  printf("microphone output dumped\n");
}
#endif

void rxEndI2S0(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData)
{
  uint32_t nBuf = *((uint32_t *) userData);//the buffer on which to do next Rx
  nbIT0++;

//  check_stack_overflow(__FUNCTION__, __LINE__);
  switch(nBuf)
  {
    case buf0_i2s0:
      SAI_TransferReceiveNonBlocking(i2s_address[0], &handle0I2S0, &xfer0I2S0);
      buf00 = 1;
      break;
    case buf1_i2s0:
      SAI_TransferReceiveNonBlocking(i2s_address[0], &handle1I2S0, &xfer1I2S0);
      buf10 = 1;
      break;
  }
  return;
}

void rxEndI2S1(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData)
{
  uint32_t nBuf = *((uint32_t *) userData);//the buffer on which to do next Rx
  nbIT1++;

    switch(nBuf)
    {
      case buf0_i2s1:
        SAI_TransferReceiveNonBlocking(i2s_address[0], &handle0I2S1, &xfer0I2S1);
        buf01 = 1;
        break;
      case buf1_i2s1:
        SAI_TransferReceiveNonBlocking(i2s_address[0], &handle1I2S1, &xfer1I2S1);
        buf11 = 1;
        break;
    }
    return;
}

static void CreateWAVHeader(uint8_t *I2sOut, I2sDescriptor *I2sOutHeader, uint32_t channel, uint32_t i2s_config) {
    unsigned int idx = 0;
    unsigned char * header_buffer = I2sOut;
    unsigned int sz = WAV_HEADER_SIZE + (OUTPUT_BUF_SIZE-PREFIX_LEN*CHUNK_SIZE) * sizeof(signed short);

    // 4 bytes "RIFF"
    header_buffer[idx++] = 'R';
    header_buffer[idx++] = 'I';
    header_buffer[idx++] = 'F';
    header_buffer[idx++] = 'F';

    // 4 bytes File size
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
    header_buffer[idx++] = (i2s_config >= 2) ? 0x02 : 0x01;
    header_buffer[idx++] = 0x00;

    // 4 bytes sample rate in Hz:
    //8kHz 0x1f40 - 22.05kHz 0x5622 - 16kHz=0x3e80
    header_buffer[idx++] = 0x80;
    header_buffer[idx++] = 0x3e;
    header_buffer[idx++] = 0x00;
    header_buffer[idx++] = 0x00;
    // set sampling to 32k
    //header_buffer[idx++] = 0x00;
    //header_buffer[idx++] = 0x7c;
    //header_buffer[idx++] = 0x00;
    //header_buffer[idx++] = 0x00;

    // 4 bytes (Sample Rate * BitsPerSample * Channels) / 8:
    // (8000*16*1)/8=0x3e80 * 2
    // (16000*16*1)/8=32000 or 0x6F00
    // (22050*16*1)/8=0xac44
    // (22050*16*2)/8=0x15888
    if (i2s_config >= 2)
    {
      header_buffer[idx++] = 0x10; //16bits stereo
      header_buffer[idx++] = 0xb1;
      header_buffer[idx++] = 0x2;
      header_buffer[idx++] = 0x00;
    } else {
      header_buffer[idx++] = 0x88; //16bits mono
      header_buffer[idx++] = 0x58;
      header_buffer[idx++] = 0x1;
      header_buffer[idx++] = 0x00;
    }
    // 2 bytes (BitsPerSample * Channels) / 8:
    // 16*1/8=2 - 16b mono
    // 16*2/8=4 - 16b stereo
    header_buffer[idx++] = (i2s_config >= 2) ? 0x04 : 0x02;
    header_buffer[idx++] = 0x00;

    // 2 bytes bit per sample: 16
    header_buffer[idx++] = 0x10;
    header_buffer[idx++] = 0x00;

    // 4 bytes "data" chunk
    header_buffer[idx++] = 'd';
    header_buffer[idx++] = 'a';
    header_buffer[idx++] = 't';
    header_buffer[idx++] = 'a';

    // 4 bytes size of data section in bytes:
    sz = OUTPUT_BUF_SIZE * sizeof(signed short);
    header_buffer[idx++] = (unsigned char) (sz & 0x000000ff);
    header_buffer[idx++] = (unsigned char)((sz & 0x0000ff00) >> 8);
    header_buffer[idx++] = (unsigned char)((sz & 0x00ff0000) >> 16);
    header_buffer[idx++] = (unsigned char)((sz & 0xff000000) >> 24);

    if (channel==1)
    {
      I2sOutHeader->l2AddrMic1 = (uint32_t) I2sOut;
      I2sOutHeader->sizeVoc2 = (2*OUTPUT_BUF_SIZE * sizeof(short)) + WAV_HEADER_SIZE;
    }
    if (channel==2)
    {
      I2sOutHeader->l2AddrMic2 = (uint32_t) I2sOut;
    }
    if (channel==0)
    {
      I2sOutHeader->l2AddrVoc = (uint32_t) I2sOut;
      I2sOutHeader->sizeVoc = ((OUTPUT_BUF_SIZE-(PREFIX_LEN*CHUNK_SIZE)) * sizeof(short)) + WAV_HEADER_SIZE;
      I2sOutHeader->startIdx = start_frame*CHUNK_SIZE;
    }

}

#ifdef PERF
void perf_print()
{
  printf("\tperf(perfCnt=%d):\n", perfCnt);
  uint32_t it0=nbIT0, it1=nbIT1, cntLp=cntC;
  printf("\tnbIT %d %d cntC %d\n", (unsigned int)it0, (unsigned int)it1, (unsigned int)cntLp);//this line creates execution issue
  unsigned int acc=0, acc1=0;
  for(uint32_t idx=0; idx<256;idx++)
  {
    acc += perfTab[0][idx];
    acc1 += perfTab[1][idx];
    printf("\t%d %d, ", perfTab[0][idx], perfTab[1][idx]);
  }
  printf("\n");
  printf("\tTotal perf cnt PCER_CYCLE_Pos=%d PCER_INSTR_Pos=%d\n", acc, acc1);
}
#endif

void res_vocIP_print()
{
  char found=0;
  printf("\tres from VAD: (thresh = %d) idxfirst=%d idxlast=%d (%d) (idxbu %d)\n",THRESH_VAD,idxmin,idxmax,idxmax-idxmin, idxbuf);
  if (1) for(int32_t i=0; i<NB_CHUNK; i++)
  {
    printf("%x ", resTab[i]);
    if ((i>(idxmin))  && !found) {printf("^^");found=1;}
    if (!((i+1)%10)) printf("\n");
  }
  printf("\n");
  printf("%x\n",(int) (vocalOutput.header));
  if (0) for(uint32_t i=0;i < OUTPUT_BUF_SIZE ; i++) {
    printf(" %d,", vocalOutput.buff[i]);
    if (!((i+1)%32)) printf("\n");

  }

}


int vad(signed short *input, unsigned char calmode) {


  unsigned int i;
  // check if voice activity
  int energy=0,min=0xffff,max=-0xffff;

  for(i = 0; i < CHUNK_SIZE ; i++) {
    energy +=(input[i]<0)?(-input[i]):input[i];
    //    if (input[i]>max) max=input[i];
    //if (input[i]<min) min=input[i];
  }
  //printf("energy %d min %d max %d\n",energy,min,max);
  if (calmode)
    return energy;
  else {
    if (energy>=THRESH_VAD ) return 1;
    else return 0;
  }
}


int loginput(int sel) {

  signed short * pt;
  static int  av_nbone=0,max_nbone=0;
  int i, energy;




  energy=vad(&(audioBuf[sel][0]),modecal);
  av_energy+=energy;

  // log VAD after calibration
  if (resCnt>(2*NB_CHUNK_PERSEC) && !found_voice) {
    //resTab[resCnt/8] |= (energy<<(resCnt&0x7));
    //printf("%d %d\n",av_nbone,max_nbone);
    if (energy) av_nbone++; else av_nbone--;

    if (idxmin==-1 && energy) {
      idxmin=resCnt;
      idxbuf=cntBuf;
      //if (cntBuf!=PREFIX_LEN-1) cntBuf=PREFIX_LEN+cntBuf+1; else cntBuf = PREFIX_LEN;
      cntBuf=PREFIX_LEN+cntBuf;
    }

    if (av_nbone<=0) {av_nbone=0;max_nbone=0;}

    if (av_nbone>max_nbone) {max_nbone=av_nbone;idxmax=resCnt;}


  }

  resCnt++;
  //printf("%d %d %d\n",idxmin,found_voice,resCnt);
  if (!found_voice)   {
    //printf("%d %d %d\n",idxmin,cntBuf,resCnt);
    pt = vocalOutput.buff + cntBuf*CHUNK_SIZE;
    if( idxmin!=-1 && cntBuf==125+idxbuf-PREFIX_LEN)
      {
	//printf("***ACQUISITION COMPLETE: START KWS** %d\n",cntBuf);
	found_voice=1;
	// first sample in frame
	//if (idxbuf!=PREFIX_LEN-1) start_frame = idxbuf; else start_frame = 0;
	start_frame = idxbuf;
	// move  prefix to get a sequential buffer
	//if (idxbuf!=PREFIX_LEN-1)
	//  for (i=0; i < CHUNK_SIZE*(idxbuf+1); i++)
	//    pt[i + PREFIX_LEN*CHUNK_SIZE] = pt[i];
	for (i=0; i < CHUNK_SIZE*idxbuf; i++)
	  pt[i + PREFIX_LEN*CHUNK_SIZE] = pt[i];
	return 1;
      }

    // when voice detected log the signal
    // select one input and load
    for(i = 0; i < CHUNK_SIZE; i++)
      {
	//printf("%d \n",audioBuf[sel][i]);
	pt[i] = audioBuf[sel][2*i]+audioBuf1[sel][2*i]+audioBuf[sel][2*i+1]+audioBuf1[sel][2*i+1];
      }
    cntBuf++;
    // circular buffer for the prefix
    if (idxmin==-1 && cntBuf>=PREFIX_LEN) cntBuf=0;
  }
  // end of calibration, set the detection threshold
  if (resCnt==2*NB_CHUNK_PERSEC && modecal==1) {
    printf("av energy %d\n",av_energy>>8);
    THRESH_VAD=(av_energy>>7);
    modecal=0;
    return 2;
  }
  //printf(">> %d vad %d sel %d\n",cntBuf,resTab[cntBuf], sel );
  return 0;

}


#define USE_AUDIO

#include "gap_cluster.h"
#include "gap_dmamchan.h"

#include "MFCC_Processing.h"

// Kernel function
#include "KernelLibStdTypes.h"

// produced by read_data_test.c
#ifndef USE_AUDIO
#ifdef DOMFCC
#include "DataTest.h"
#else
#include "DataFEAT.h"
#endif
#endif


#ifdef RT_HAS_HWCE
#include "CnnKernels_hwce.h"
#define POWERCNN1_2   (6670.f)
#define POWERCNN1     (5600.f)
#else
#include "CnnKernels.h"
#define POWERCNN1_2   (28000.f)
#define POWERCNN1     (16800.f)
#endif
#include "CnnKernels_tileddense.h"

#ifdef KWS

#ifdef RT_HAS_HWCE
#include "coeff_hwce.h"
#else

#ifdef FULL_PREC
#include "coeff_ref.h"
#endif

#ifdef W_HALFCHAR
#include "coeff.h"
#endif

#ifdef W_CHAR
#include "coeff.h"
#endif
#endif

#endif

#ifdef ALEXA

#ifdef RT_HAS_HWCE
#include "coeff_hwce_alexa.h"
#else

#ifdef W_HALFCHAR
#include "coeff_alexa.h"
#endif
#endif
#endif

#define CORE_NUMBER      8

uint32_t cluster_perf;

/*******************************************************************************
 * Frame Variables
 ******************************************************************************/
//GAP_L2_DATA v2s  W_Frame[N_FFT+4];
GAP_L1_FC_DATA v2s  *W_Frame;
//GAP_L2_DATA  short int Frame[FRAME];
GAP_L1_FC_DATA  short int *Frame;
short int FEAT_LIST[N_FRAME * NUMCEP];
short int *pfeat_list;

/*******************************************************************************
 * IMAGE Variables
 ******************************************************************************/
#define C0_NFEAT 32
#define C1_NFEAT 32

#ifdef KWS
#define CLast_NFEAT 12
char *word_list[CLast_NFEAT] = {"silence", "none", "yes", "no", "up", "down", "left", "right", "on", "off", "stop",  "go"};
#endif

#ifdef ALEXA
#define CLast_NFEAT 3
char *word_list[CLast_NFEAT] = {"silence", "none", "alexa"};
#endif

#define IMAGE_SIZE 98
#define BUF0_SIZE  20000
#define BUF1_SIZE  13000
#define L1_Memory_SIZE 52000

//GAP_L2_DATA short int *l2_big0;
//GAP_L2_DATA short int *l2_big1;
GAP_L2_DATA short int l2_big0[BUF0_SIZE];
GAP_L2_DATA short int l2_big1[BUF1_SIZE];

void CNN_Process() {

#if 0
  // allocate data buffers
    l2_big1 = malloc(BUF1_SIZE * sizeof(short int));
    //printf("@l2_big1: %p\n", l2_big1);
    if(l2_big1 == NULL)
        printf("error of malloc l2_big1\n");
    l2_big0 = malloc(BUF0_SIZE * sizeof(short int));
    if(l2_big0 == NULL)
        printf("error of malloc l2_big0\n");
#endif
    
    #ifdef RT_HAS_HWCE
    Conv8x20MaxPool2x2_HWCE_0(pfeat_list,L2_W_0,l2_big0,8,L2_B_0,AllKernels + 0);
    Conv6x10_HWCE_1(l2_big0,L2_W_1,l2_big1,8,L2_B_1,AllKernels + 1);

    #else
    ConvLayer1(pfeat_list, L2_W_0,l2_big0,8,L2_B_0,8,20,AllKernels + 0);

    #ifdef W_HALFCHAR
    ConvLayer3(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1);
    #endif
    #ifdef FULL_PREC
    ConvLayer2(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1);
    #endif
    #ifdef W_CHAR
    ConvLayer3(l2_big0,L2_W_1,l2_big1,8,L2_B_1,4,10,AllKernels + 1);
    #endif
    #endif

    //free(l2_big0);
    //l2_big0 = malloc(CLast_NFEAT*sizeof(short int));

    // in,filter,normfilter,bias,normbias,out,outsize
    #ifdef W_HALFCHAR
    Dense_halfchar(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,CLast_NFEAT,AllKernels + 2);
    #endif
    #ifdef FULL_PREC
    Dense2(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,CLast_NFEAT,AllKernels + 2);
    #endif
    #ifdef W_CHAR
    Dense2(l2_big1,L2_W_2,10,L2_B_2,0,l2_big0,CLast_NFEAT,AllKernels + 2);
    #endif
}

void Master_Entry(void *arg) {
    #ifdef RT_HAS_HWCE
    /* Make HWCE event active */
    //printf("CNN launched on the HWCE\n");
    EU_EVT_MaskSet(1 << EU_HWCE_EVENT);
    #endif

    CNN_Process();
}

void runkws(char trial) {

#ifdef DOMFCC
#ifdef USE_AUDIO
        short int *InSignal = (short int *) (audioBuff + start_frame*CHUNK_SIZE);
#else
        short int *InSignal = DataIn;
#endif


#ifdef PERF
	//gap8_resethwtimer();

	PERFORMANCE_Start(&perf, PCER_CYCLE_Msk | PCER_INSTR_Msk);
#endif

	Frame = FC_Malloc(FRAME*sizeof(short int));
	W_Frame = FC_Malloc((N_FFT+4)*sizeof(v2s));

	//printf("start %d\n",start_frame);
        MFCC_Processing(InSignal, W_Frame, Frame, FEAT_LIST);
        pfeat_list = (short int*) FEAT_LIST;
#else
        pfeat_list = (short int*) DataIn;
#endif

#ifdef PERF
	PERFORMANCE_Stop(&perf);
	printf("MFCC cycles==>%d\n",PERFORMANCE_Get(&perf, PCER_CYCLE_Pos));
#endif

	FC_MallocFree(W_Frame);
	FC_MallocFree(Frame);

	

        /* FC send a task to Cluster */
        CLUSTER_SendTask(0, Master_Entry, 0, 0);

        /* Wait cluster finish task */
        CLUSTER_Wait(0);
        {
            //  ******************************** Softmax on FC ****************************
            int i, j, sum=0;
            char *s;
            uint8_t idx_max=0;

            printf("\n");
            int max=0x80000000;

            for(i=0; i < CLast_NFEAT; i++) {
	      logscore[i]+=l2_big0[i] ;
#ifdef DUMP_SCORE
	      printf(" feat %d: %d  \n", i, l2_big0[i]);
#endif
	      sum += l2_big0[i];

                #ifndef TESTNONE
                if (l2_big0[i]>max && i!=1) {max=l2_big0[i];idx_max=i;}
                #else
                if (l2_big0[i]>max) {max=l2_big0[i];idx_max=i;}
                #endif
            }
	    if (trial) word1=idx_max; else word2=idx_max;
            printf("found max %d\n", idx_max+1);
#ifdef DUMP_SCORE
            printf("found word %s\n", word_list[idx_max]);
#endif
        }

	//free(l2_big0);
	//free(l2_big1);


}

void Audio_Enable()
{
    /* I2S Clock Enable */
    SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, EN_CHAN0, CK_DIV_FAC);
}

void Audio_Disable()
{
    /* I2S Clock disable */
    SAI_ClockConfig(i2s_address[0], uSAI_Channel0, WORD_SIZE_CHAN0, 0, CK_DIV_FAC);
}

#define LENGTH_AV 16
#define SHL 1
int av[4][LENGTH_AV] ;
int idx_av=0;
int av_00=0,av_01=0,av_02=0,av_03=0;


int processI2sRx(int sel)
{
  int av_0=0,av_1=0,av_2=0,av_3=0;
  unsigned i, j=0;
  signed short * pt;
  fract input[4*128];
  //static unsigned int resTab[NB_CHUNK] = {[0 ... NB_CHUNK-1] = 0x55};

  
    for(i = 0; i < 128; i++)
    {
      input[4*i + 0] = audioBuf[sel][j]<<SHL;
      input[4*i + 2] = audioBuf1[sel][j++]<<SHL ;
      input[4*i + 1] = audioBuf[sel][j]<<SHL;
      input[4*i + 3] = audioBuf1[sel][j++]<<SHL ;
    }

    for(i = 0; i < 128; i++)
    {
      av_0 += input[4*i + 0] ;
      av_2 += input[4*i + 2] ;
      av_1 += input[4*i + 1] ;
      av_3 += input[4*i + 3] ;
    }


    av_0 >>= 7;
    av_1 >>= 7;
    av_2 >>= 7;
    av_3 >>= 7;

    av_00 -= av[0][idx_av];
    av_00 += av_0;
    av[0][idx_av] = av_0;

    av_01 -= av[1][idx_av];
    av_01 += av_1;
    av[1][idx_av] = av_1;

    av_02 -= av[2][idx_av];
    av_02 += av_2;
    av[2][idx_av] = av_2;

    av_03 -= av[3][idx_av];
    av_03 += av_3;
    av[3][idx_av] = av_3;


    idx_av++;
    if (idx_av==LENGTH_AV) idx_av=0;

    // offset correction
    if (1) for(i = 0; i < 128; i++)
    {
      input[4*i + 0] -= (av_00>>4) ;
      input[4*i + 2] -= (av_02>>4) ;
      input[4*i + 1] -= (av_01>>4) ;
      input[4*i + 3] -= (av_03>>4) ;
    }


    last_res=res;

#ifndef FORCE_INPUT
    res = dsp_process_block(input, NULL, vocalOutput.buff+OFFSET_BU*CHUNK_SIZE+cntBuf*CHUNK_SIZE, 4, CHUNK_SIZE);

    if (!last_res&&res) store=1;
#else
    return 1;
#endif

#ifndef RECORD_MIC
#if 0
#ifdef KWS
    // load the unprocessed mic input into buffer
    if (store)
      for (i=0;i<CHUNK_SIZE;i++) {
	micOutput1.buff[OFFSET_BU*CHUNK_SIZE+cntBuf*CHUNK_SIZE + i  ] = (input[4*i + 0] + input[4*i + 1] + input[4*i + 2] + input[4*i + 3])>>2;
	if (abs(input[4*i + 0])>maxval) maxval =  abs(input[4*i + 0]);
      }
    //resTab[cntRes] = res;
#endif
#endif
#endif
    resTab[cntRes/8] |= (res<<(cntRes&0x7));

    cntRes++;
    if (cntRes>=8*NB_CHUNK) cntRes=0;


#ifdef RECORD_MIC
    if (store) {
      for (i=0;i<CHUNK_SIZE;i++) {
	micOutput1.buff[2*cntBuf*CHUNK_SIZE + 2*i  ] = input[4*i + 0];
	micOutput1.buff[2*cntBuf*CHUNK_SIZE + 2*i+1] = input[4*i + 1];
	micOutput2.buff[2*cntBuf*CHUNK_SIZE + 2*i  ] = input[4*i + 2];
	micOutput2.buff[2*cntBuf*CHUNK_SIZE + 2*i+1] = input[4*i + 3];
      }
    }
#endif

    if (!found_voice)   {
      //printf("%d %d %d\n",idxmin,cntBuf,resCnt);
      //pt = vocalOutput.buff + cntBuf*CHUNK_SIZE;
      if( store && cntBuf==(125-OFFSET_BU))
	{
	  //printf("%d %d %d %d\n",av_00>>4,av_01>>4,av_02>>4,av_03>>4);
	  found_voice=1;
	  // first sample in frame
	  start_frame = 0;

	  return 1;
	}

      if (store) cntBuf++;
    }

    return 0;
}

int capture_and_process() {

  while(1) {

    
#ifndef FORCE_INPUT 
    if ((buf00 && buf01) || (buf10 && buf11))
#else
      stat_log=1;
#endif
      {
      //printf("!\n");

      irq = __disable_irq();
      if (buf00) buf00 = buf01 = 0;
      if (buf10) buf10 = buf11 = 0;

      stat_log=processI2sRx(toggle);

      __restore_irq(irq);

      if (toggle==0) toggle=1; else toggle=0;

      //printf("stat_log %d\n",stat_log);

      if (stat_log==1) {
	if (1) {

	  irq = __disable_irq();
	  
	  // first run with the IP output
	  audioBuff = vocalOutput.buff;
	  
#ifdef FORCE_INPUT
	  audioBuff = DataIn;
#endif
	  //printf("run kws\n");
	  runkws(0);

#if 0	  
#ifdef KWS
	  // second run with direct mic output
	  audioBuff = micOutput1.buff;
	  runkws(1);
#endif
#endif	  
	  	
	  
	  int max=0;
	  for (int i=0;i<CLast_NFEAT;i++) {


#ifndef TESTNONE
	    if (logscore[i]>max && i!=1) {max=logscore[i];idxmax=i;}
#else
	    if (logscore[i]>max) {max=logscore[i];idxmax=i;}
#endif
	    logscore[i]=0;
	  }

#ifdef KWS
	  if (0) {
	  //if (word1==11) idxmax=11;
	  if (word2==3) idxmax = 3;
	  if (word2==4) idxmax = 4;
	  //if (word1==6) idxmax=6;
	  if (word1==5) idxmax=5;
	  }
#endif
       	  printf("found word %s\n", word_list[idxmax]);

#ifdef FORCE_INPUT
	  return 0;
#endif
	  
	}
	
	last_res=0;
	res=0;
	store=0;
	found_voice=0;
	cntBuf=0;
	idxmin=-1;
	idxbuf=-1;
	cntRes=0;
	maxval=0;
	toggle=0;
	__restore_irq(irq);
      	printf(">>>\n");
      }
    }
  }
}

int main()
{
  /* Cluster Start - Power on */
  CLUSTER_Start(0, CORE_NUMBER,0);

  // Allocate a buffer in the shared L1 memory
  L1_Memory = L1_Malloc(L1_Memory_SIZE);

  if(L1_Memory == NULL) {
    printf("L1 Memory allocation error\n");
    return -1;
  }


#if L2STACK_DBG
  printf("stack initial addr 0x%x last current stack pointer: 0x%x\n", (unsigned int *) &__l2Stack, (unsigned int)rd_stack_pt());
  check_stack_overflow(__FUNCTION__, __LINE__);
#endif

#ifndef FORCE_INPUT
  printf("init_I2S\n");
  init_I2S();
  printf(">>>\n");
#endif  

  capture_and_process();
  
  return 0;
}

