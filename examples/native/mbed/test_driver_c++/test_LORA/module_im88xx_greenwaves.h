#ifndef MODULEIM88XX_H
#define MODULEIM88XX_H
#ifdef __cplusplus
extern "C"{
#endif 
#include "mbed.h"
#include <stdint.h>
#include <string.h>

class module_im88xx{
	public:

	/* iM88xx functions */
	bool sendPingRequest();
	bool sendGetRadioConfig();

	/* Serial communication */
	int readIm88xxMessage();
	int writeMessagePerChar(unsigned char *buf,int nb);
	void rx_irq(int event);
	void payloadReceivedStart();
	int payloadReceivedLoRaMessage(unsigned char lorapayload[], int timeoutInSeconds);
	
	/* Constructor */
	module_im88xx();

	int checkMessageReceived(unsigned char *tab,int nb);

	private:

	/* Current Radio Configuration */
	unsigned char current_FREQ[3]={0,0,0};
	unsigned char current_BANDWIDTH[1]={0};
	unsigned char current_SF[1]={0};
	unsigned char current_CR[1]={0};
	unsigned char current_PWR[1]={0};
	unsigned char current_RADIOMODE[1]={0};
	unsigned char current_RXMODE[1]={0};
	unsigned char current_DEVICEADDRESS[2]={0,0};
	unsigned char current_GROUPADDRESS[1]={0x02};
	unsigned char devEUI[8]={0,0,0,0,0,0,0,0};
	
	/* Private functions */
	static void readLoRaMessage();
};
#ifdef __cplusplus
}
#endif
#endif

