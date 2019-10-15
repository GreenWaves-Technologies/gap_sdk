#ifndef RADIOPARAMS_H
#define RADIOPARAMS_H
#ifdef __cplusplus
extern "C"{
#endif 
/* Frequency */
static const unsigned char FREQ_868100000[3]={0x67,0x06,0xD9};
static const unsigned char FREQ_868300000[3]={0x34,0x13,0xD9};
static const unsigned char FREQ_868500000[3]={0x00,0x20,0xD9};
static const unsigned char FREQ_868700000[3]={0xCD,0x2C,0xD9};
static const unsigned char FREQ_868900000[3]={0x9A,0x39,0xD9};
static const unsigned char FREQ_865000000[3]={0x00,0x40,0xD8};
static const unsigned char FREQ_867500000[3]={0x00,0xE0,0xD8};

/* Bandwidth */
static const unsigned char BANDWIDTH_125[1]={0x00};
static const unsigned char BANDWIDTH_250[1]={0x01};
static const unsigned char BANDWIDTH_500[1]={0x02};

/* Spreading Factor */
static const unsigned char SF_7[1]={0x07};
static const unsigned char SF_8[1]={0x08};
static const unsigned char SF_9[1]={0x09};
static const unsigned char SF_10[1]={0x0A};
static const unsigned char SF_11[1]={0x0B};
static const unsigned char SF_12[1]={0x0C};

/* Code Rate */
static const unsigned char CR_4_5[1]={0x00};
static const unsigned char CR_4_6[1]={0x02};
static const unsigned char CR_4_7[1]={0x03};
static const unsigned char CR_4_8[1]={0x04};

/* Transmission Power */
static const unsigned char PWR_5[1]={0x05};
static const unsigned char PWR_6[1]={0x06};
static const unsigned char PWR_7[1]={0x07};
static const unsigned char PWR_8[1]={0x08};
static const unsigned char PWR_9[1]={0x09};
static const unsigned char PWR_10[1]={0x0A};
static const unsigned char PWR_11[1]={0x0B};
static const unsigned char PWR_12[1]={0x0C};
static const unsigned char PWR_13[1]={0x0D};
static const unsigned char PWR_14[1]={0x0E};
static const unsigned char PWR_15[1]={0x0F};
static const unsigned char PWR_16[1]={0x10};
static const unsigned char PWR_17[1]={0x11};
static const unsigned char PWR_18[1]={0x12};
static const unsigned char PWR_19[1]={0x13};
static const unsigned char PWR_20[1]={0x14};

/* Radio Mode */
static const unsigned char RADIOMODE_STANDARD[1]={0x00};
static const unsigned char RADIOMODE_SNIFFER[1]={0x02};
static const unsigned char RADIOMODE_ECHO[1]={0x01};

/* RX Mode */
static const unsigned char RX_OFF[1]={0x00};
static const unsigned char RX_ALWAYS_ON[1]={0x01};
static const unsigned char RX_ON_FOR_LIMITED_TIME[1]={0x02};
#ifdef __cplusplus
}
#endif
#endif

