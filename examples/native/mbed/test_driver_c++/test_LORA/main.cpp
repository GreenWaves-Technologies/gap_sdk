#include "mbed.h"
#include <stdint.h>
#include <string.h>
#include "module_im88xx_greenwaves.h"
#include "radio_params.h"
using namespace std;

DigitalOut out(GPIO_A15);

int main()
{
	out=1;
	wait(1);
	out=0;
    	module_im88xx mod;

	if(!mod.sendGetRadioConfig()){
		out=1;
	}else{
		printf("Get Radio Config Success ...\n");
	}

        mod.payloadReceivedStart();

	while (1){
		printf("Receiving LoRa Message from a lora device ... \n");
		unsigned char recMessage[256];
		int payloadSize = mod.payloadReceivedLoRaMessage(recMessage, 10); //timeout = 10 seconds
		if(payloadSize){
			printf("Received message = ");
			for(int i=0;i<payloadSize;i++)
				printf("%02x ",recMessage[i]);
			printf("\n");
		}else{
			printf("Payload size = 0\n");
		}

		printf("End receiving LoRa Message from demo ... \n");
	}

	printf("END\n");

    return 0;

}
