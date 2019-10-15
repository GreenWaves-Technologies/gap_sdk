 #ifndef __PAD_H__
 #define __PAD_H__

#include "archi_gap8Gpio.h"
 typedef enum {
 	Alt0=0,
 	Alt1=1,
 	Alt2=2,
 	Alt3=3
 } PadAlternateT;

 typedef union {
 	struct {
 		unsigned char PadEnable:1;
 		unsigned char DriveStrength:1;
 		unsigned char Padding:6;
 	} __attribute__ ((packed)) F;
 	unsigned char Raw;
 } __attribute__ ((packed)) PadConfigurationT;


 typedef enum {
 	sleepOnePadOutputDisable=SLEEP_PAD_OUTPUT_DISABLE,
 	sleepOnePadOutputEnable=SLEEP_PAD_OUTPUT_ENABLE
 } sleepPadEnableT;

 typedef enum {
 	SLEEPONEPADOUTPUTVALlOW=SLEEP_PAD_OUTPUT_VALUE_LOW,
 	SLEEPONEPADOUTPUTVALHIGH=SLEEP_PAD_OUTPUT_VALUE_HIGH
 } sleepPadValueT;

 typedef enum {
 	SLEEPPADDISABLE=DISABLE_PAD_SLEEP,
 	SLEEPPADENABLE=ENABLE_PAD_SLEEP
 } sleepPadEnT;

void gap8SetOnePadAlternate(unsigned int PadN, PadAlternateT Alt);
PadAlternateT gap8GetOnePadAlternate(unsigned int PadN);
void gap8SetAllPadAlternate(unsigned int Alt[]);
int gap8EnableOnePad(unsigned int padN, unsigned char En);
int gap8SetOnePadStrength(unsigned int padN, unsigned char St);
unsigned int gap8GetOnePadConfig(unsigned int padN);
void gap8SetAllPadConfig(unsigned int Config[]);
void gap8SetGPIODir(unsigned int Gpio, unsigned int Dir);
int gap8GiveGpioNb(unsigned int padN);
void gap8WriteGPIO(unsigned int Gpio, unsigned int V);

 #endif
