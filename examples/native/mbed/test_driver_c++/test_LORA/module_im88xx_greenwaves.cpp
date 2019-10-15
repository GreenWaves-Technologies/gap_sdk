#include "mbed.h"
#include <stdint.h>
#include <string.h>
#include "module_im88xx_greenwaves.h"
#include "radio_params.h"
#include "crc.h"
using namespace std;

#define SLIP_END 0xC0
#define SLIP_ESC 0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD

/* Global variables */
RawSerial pc(USBTX, USBRX, 115200);
Semaphore messageReception(2); // Protect receivedLoRa when accessed by two threads
unsigned char RX_BUFFER[256]={0,};
int receivedLoRa; //boolean value to know if a lora message is received
int loraBytesReceived; //Number of bytes of the lora message received

/* Constructor */
module_im88xx::module_im88xx(void){
	receivedLoRa=0;
	loraBytesReceived=0;
}

/* Treat special caracters (c0 and db) in the message received */
int module_im88xx::checkMessageReceived(unsigned char *tab,int nb){
	unsigned char decodedMessage[256]="";
	int index_decoded_message=0;
	for(int i=0;i<nb;i++){
		if(tab[i]==SLIP_ESC){
			if(tab[i+1]==SLIP_ESC_END){
				decodedMessage[index_decoded_message]=SLIP_END;
				i++;
			}else{
				if(tab[i+1]==SLIP_ESC_ESC){
					decodedMessage[index_decoded_message]=SLIP_ESC;
					i++;
				}
			}
		}else{
			decodedMessage[index_decoded_message]=tab[i];
		}
		index_decoded_message++;
	}
	for(int i=0;i<index_decoded_message;i++){
		tab[i]=decodedMessage[i];
	}
	return index_decoded_message;
}

/* Read one iM88xx message */
int module_im88xx::readIm88xxMessage ()
{
	int nbReceived=0;
	char read_buf[1];
	int received = 0;
	while(!received){
		if((read_buf[0]=pc.getc())==0xc0){
			RX_BUFFER[nbReceived]=read_buf[0];
			nbReceived++;
			while((read_buf[0]=pc.getc())!=0xc0){
				RX_BUFFER[nbReceived]=read_buf[0];
				nbReceived++;
			}
			RX_BUFFER[nbReceived]=read_buf[0];
			nbReceived++;
			received=1;
		}
	}
	if (nbReceived) {
        	for(int i=0;i<nbReceived;i++){
        	        printf("%02x ", RX_BUFFER[i]);
		}
		printf("\n");
        }
	return nbReceived;
}


/* Write to serial port */
int module_im88xx::writeMessagePerChar(unsigned char *buf,int nb){
	for(int i=0;i<nb;i++){
		pc.putc(buf[i]);
	}
	return nb;
}

/* Send Get Radio Config */
bool module_im88xx::sendGetRadioConfig(){
	unsigned char getRadioConfigRequest[]={0xc0,0x01,0x13,0x85,0x34,0xc0};
	writeMessagePerChar(getRadioConfigRequest,6);
	int received = readIm88xxMessage();
	if(received>0){
		int messageSize = checkMessageReceived(RX_BUFFER,received);
		if(RX_BUFFER[1]==0x01 && RX_BUFFER[2]==0x14){
			size_t payloadSize = received-6;
			unsigned char payload[256]={0,};
			for(int i=3;i<received-3;i++){
				payload[i-3]=RX_BUFFER[i];
			}
			// Decode the payload
			int status = payload[0];
			if(status==0x00){
				//Operation Successfull
				current_RADIOMODE[0]=payload[1];
				current_GROUPADDRESS[0]=payload[2];
				current_DEVICEADDRESS[0]=payload[5]; current_DEVICEADDRESS[1]=payload[4];
				current_FREQ[0]=payload[9]; current_FREQ[1]=payload[10]; current_FREQ[2]=payload[11];
				current_BANDWIDTH[0]=payload[12];
				current_SF[0]=payload[13];
				current_CR[0]=payload[14];
				current_PWR[0]=payload[15];
				wait(0.5);
				return true;
			}else{
				wait(0.5);
				return false;
			}
		}else{
			wait(0.5);
			return false;
		}
	}
	wait(0.5);
	return false;
}

/* function to call in a thread when reading a lora message */
void module_im88xx::readLoRaMessage ()
{
	// char read_buf[1];
	// while(!receivedLoRa){
	// 	if((read_buf[0]=pc.getc())==0xc0){
	// 		messageReception.wait();
	// 		RX_BUFFER[loraBytesReceived]=read_buf[0];
	// 		loraBytesReceived++;
	// 		while((read_buf[0]=pc.getc())!=0xc0){
	// 			RX_BUFFER[loraBytesReceived]=read_buf[0];
	// 			loraBytesReceived++;
	// 		}
	// 		RX_BUFFER[loraBytesReceived]=read_buf[0];
	// 		loraBytesReceived++;
	// 		receivedLoRa=1;
	// 		messageReception.release();
	// 	}
	// }
}

char read_buf[1];

void module_im88xx::rx_irq(int event)
{
    if(!receivedLoRa){
        if(loraBytesReceived) {
            if(read_buf[0] != 0xc0){
                RX_BUFFER[loraBytesReceived]=read_buf[0];
                loraBytesReceived++;
            } else {
                RX_BUFFER[loraBytesReceived]=read_buf[0];
                loraBytesReceived++;
                receivedLoRa=1;
            }
        } else {
            if(read_buf[0]==0xc0){
                RX_BUFFER[loraBytesReceived]=read_buf[0];
                loraBytesReceived++;
            }

        }
    }
    pc.read((uint8_t*)read_buf, 1, callback(this, &module_im88xx::rx_irq), SERIAL_EVENT_RX_ALL, 0);
}

void module_im88xx::payloadReceivedStart()
{
    pc.read((uint8_t*)read_buf, 1, callback(this, &module_im88xx::rx_irq), SERIAL_EVENT_RX_ALL, 0);
}

int module_im88xx::payloadReceivedLoRaMessage(unsigned char payload [], int timeoutInSeconds){
        wait(timeoutInSeconds);
        wait(0.5);

	messageReception.wait();
	if(receivedLoRa){
		int messageSize = checkMessageReceived(RX_BUFFER,loraBytesReceived);
		int payloadSize = messageSize-20;

		/* Reinitialise boolean of received lora message */
		receivedLoRa=0;
		loraBytesReceived=0;

		if(RX_BUFFER[1]==0x03 && RX_BUFFER[2]==0x04 && RX_BUFFER[3]==0x01){
			for(int i=10;i<(messageSize-10);i++){
				payload[i-10]=RX_BUFFER[i];
			}
			wait(0.5);
			messageReception.release();
			return payloadSize;
		}else{
			wait(0.5);
			messageReception.release();
			return 0;
		}
	}else{
		/* Reinitialise boolean of received lora message */
		receivedLoRa=0;
		loraBytesReceived=0;
		wait(0.5);
		messageReception.release();
		return 0;
	}
}
