#include "mbed.h"
#include "stdio.h"
#include "mbed_events.h"
#include <stdint.h>
#include <string.h>
using namespace std ;

/*-------------------------------------------------------------------------------*/
#define TOTAL_GPIO 21
#define EMPTY_FLAG -1

struct task {
    int pinNumber;
    int value;
};

volatile task queue[TOTAL_GPIO];
volatile int task_index = 0;

/*-----GLOBAL Variables----------------------------------------------------------*/
InterruptIn  pin0(GPIO_A16);
InterruptIn  pin1(GPIO_A3_B1);
InterruptIn  pin2(GPIO_A2_A2);
InterruptIn  pin3(GPIO_A1_B2);
InterruptIn  pin4(GPIO_A0_A3);
InterruptIn  pin5(GPIO_A17);
InterruptIn  pin6(GPIO_A18);
InterruptIn  pin7(GPIO_A19);
InterruptIn  pin8(GPIO_A20);
InterruptIn  pin9(GPIO_A21);
InterruptIn  pin10(GPIO_A14);
InterruptIn  pin11(GPIO_A5_A37);
InterruptIn  pin12(GPIO_A13);
InterruptIn  pin13(GPIO_A12);
InterruptIn  pin14(GPIO_A11);
InterruptIn  pin15(GPIO_A10);
InterruptIn  pin16(GPIO_A9);
InterruptIn  pin17(GPIO_A8);
InterruptIn  pin18(GPIO_A7);
InterruptIn  pin19(GPIO_A6);
InterruptIn  pin20(GPIO_A4_A43);
DigitalOut out(GPIO_A15);

/* LoRaSensorDataModel */
int pinValues[21]; //Read the value of the pins: Maximum number of pins = 21
int i; //for loop
int nbMaxPins=21;

/* Detect if one door is open */
int out_bits=0;

/* Initialise LoRa module variable from the library that I did */


/*-------------------------------------------------------------------------------*/

/*-----FUNCTIONS DECLARATION-----------------------------------------------------*/

/* Handler interruptions GPIO */
void allDoorsUpdate();
void handler_user_context(int pinNumber, int value);
void handler0_Interruption(void);
void handler1_Interruption(void);
void handler2_Interruption(void);
void handler3_Interruption(void);
void handler4_Interruption(void);
void handler5_Interruption(void);
void handler6_Interruption(void);
void handler7_Interruption(void);
void handler8_Interruption(void);
void handler9_Interruption(void);
void handler10_Interruption(void);
void handler11_Interruption(void);
void handler12_Interruption(void);
void handler13_Interruption(void);
void handler14_Interruption(void);
void handler15_Interruption(void);
void handler16_Interruption(void);
void handler17_Interruption(void);
void handler18_Interruption(void);
void handler19_Interruption(void);
void handler20_Interruption(void);

/*-------------------------------------------------------------------------------*/

/*-----FUNCTION DEFINITION-------------------------------------------------------*/

/* Function executed when timeout is applied */
void allDoorsUpdate() {
    handler_user_context(25,0);
}

/* Handler interruptions declaration */
void handler_user_context(int pinNumber, int value) {
    if(pinNumber!=25){
        /* update out_pins to check if at least one door is open */
        if(value==0){
            out_bits |= (1<<pinNumber); //Set bit to 1
        }else{
            out_bits &= ~(1<<pinNumber); //Set bit to 0
        }

        /* Send update of the specified pinNumber */
        printf("Pin %d	Value %d\n",pinNumber,value);
        //Prepare LoRa Message
        //Access UART and send a LoRa message : each time I send a message using TX, I receive a response from the module using RX

    }else{
        /* Send LoRa update for all sensors */
        printf("Sending update for all doors ...\n");
        /* Read values of all needed pins */
        pinValues[0]=pin0.read();
        pinValues[1]=pin1.read();
        pinValues[2]=pin2.read();
        pinValues[3]=pin3.read();
        pinValues[4]=pin4.read();
        pinValues[5]=pin5.read();
        pinValues[6]=pin6.read();
        pinValues[7]=pin7.read();
        pinValues[8]=pin8.read();
        pinValues[9]=pin9.read();
        pinValues[10]=pin10.read();
        pinValues[11]=pin11.read();
        pinValues[12]=pin12.read();
        pinValues[13]=pin13.read();
        pinValues[14]=pin14.read();
        pinValues[15]=pin15.read();
        pinValues[16]=pin16.read();
        pinValues[17]=pin17.read();
        pinValues[18]=pin18.read();
        pinValues[19]=pin19.read();
        pinValues[20]=pin20.read();

        /* update out_pins to check if at least one door is open */
        for(i=0;i<nbMaxPins;i++){
            printf("Pin %d	Value %d\n",i,pinValues[i]);
            if(pinValues[i]==0){
                out_bits |= (1<<i); //Set bit to 1
            }else{
                out_bits &= ~(1<<i); //Set bit to 0
            }
        }

        //Prepare LoRa Message
        //Access UART and send a LoRa message : each time I send a message using TX, I receive a response from the module using RX
    }

    /* Check if output pin should be put to 1 */
    if(out_bits!=0){
        out=0;
    }else{
        out=1;
    }

}

void handler0_Interruption(void) {
    queue[task_index / TOTAL_GPIO].pinNumber = 0;
    queue[task_index++ / TOTAL_GPIO].value   = pin0;
}

void handler1_Interruption(void) {
    queue[task_index / TOTAL_GPIO].pinNumber = 1;
    queue[task_index++ / TOTAL_GPIO].value   = pin1;
}

void handler2_Interruption(void) {
    queue[task_index / TOTAL_GPIO].pinNumber = 2;
    queue[task_index++ / TOTAL_GPIO].value   = pin2;
}

void handler3_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 3;
    queue[task_index++ % TOTAL_GPIO].value   = pin3;
}

void handler4_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 4;
    queue[task_index++ % TOTAL_GPIO].value   = pin4;
}

void handler5_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 5;
    queue[task_index++ % TOTAL_GPIO].value   = pin5;
}

void handler6_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 6;
    queue[task_index++ % TOTAL_GPIO].value   = pin6;
}

void handler7_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 7;
    queue[task_index++ % TOTAL_GPIO].value   = pin7;
}

void handler8_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 8;
    queue[task_index++ % TOTAL_GPIO].value   = pin8;
}

void handler9_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 9;
    queue[task_index++ % TOTAL_GPIO].value   = pin9;
}

void handler10_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 10;
    queue[task_index++ % TOTAL_GPIO].value   = pin10;
}

void handler11_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 11;
    queue[task_index++ % TOTAL_GPIO].value   = pin11;
}

void handler12_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 12;
    queue[task_index++ % TOTAL_GPIO].value   = pin12;
}

void handler13_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 13;
    queue[task_index++ % TOTAL_GPIO].value   = pin13;
}

void handler14_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 14;
    queue[task_index++ % TOTAL_GPIO].value   = pin14;
}

void handler15_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 15;
    queue[task_index++ % TOTAL_GPIO].value   = pin15;
}

void handler16_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 16;
    queue[task_index++ % TOTAL_GPIO].value   = pin16;
}

void handler17_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 17;
    queue[task_index++ % TOTAL_GPIO].value   = pin17;
}

void handler18_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 18;
    queue[task_index++ % TOTAL_GPIO].value   = pin18;
}

void handler19_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 19;
    queue[task_index++ % TOTAL_GPIO].value   = pin19;
}

void handler20_Interruption(void) {
    queue[task_index % TOTAL_GPIO].pinNumber = 20;
    queue[task_index++ % TOTAL_GPIO].value   = pin20;
}


/*-----MAIN FUNCTION-------------------------------------------------------------*/
int main() {
    printf("Application started ...\n");

    /* Access Radio module using UART to do the following :
       1) Send a LoRa message using tx
       2) Wait 10 seconds to receive some bytes on RX UART (start a thread that do this operation)
       3) Stop listening (kill the thread that is reading) and check if I received some bytes or not (to do that I use a semaphore to protect a global variable that is accesse by two threads)
    */

    /* On boot start a LoRa update for all the doors */
    allDoorsUpdate();

    /* Led turned off by default */
    out=1;

    /* Declare interruptions for each pin on rise and fall */
    pin0.rise(handler0_Interruption);
    pin0.fall(handler0_Interruption);
    pin1.rise(handler1_Interruption);
    pin1.fall(handler1_Interruption);
    pin2.rise(handler2_Interruption);
    pin2.fall(handler2_Interruption);
    pin3.rise(handler3_Interruption);
    pin3.fall(handler3_Interruption);
    pin4.rise(handler4_Interruption);
    pin4.fall(handler4_Interruption);
    pin5.rise(handler5_Interruption);
    pin5.fall(handler5_Interruption);
    pin6.rise(handler6_Interruption);
    pin6.fall(handler6_Interruption);
    pin7.rise(handler7_Interruption);
    pin7.fall(handler7_Interruption);
    pin8.rise(handler8_Interruption);
    pin8.fall(handler8_Interruption);
    pin9.rise(handler9_Interruption);
    pin9.fall(handler9_Interruption);
    pin10.rise(handler10_Interruption);
    pin10.fall(handler10_Interruption);
    pin11.rise(handler11_Interruption);
    pin11.fall(handler11_Interruption);
    pin12.rise(handler12_Interruption);
    pin12.fall(handler12_Interruption);
    pin13.rise(handler13_Interruption);
    pin13.fall(handler13_Interruption);
    pin14.rise(handler14_Interruption);
    pin14.fall(handler14_Interruption);
    pin15.rise(handler15_Interruption);
    pin15.fall(handler15_Interruption);
    pin16.rise(handler16_Interruption);
    pin16.fall(handler16_Interruption);
    pin17.rise(handler17_Interruption);
    pin17.fall(handler17_Interruption);
    pin18.rise(handler18_Interruption);
    pin18.fall(handler18_Interruption);
    pin19.rise(handler19_Interruption);
    pin19.fall(handler19_Interruption);
    pin20.rise(handler20_Interruption);
    pin20.fall(handler20_Interruption);

    /* Initial task queue to empty */
    for (i = 0; i < TOTAL_GPIO; i++)
    {
        queue[i].pinNumber = EMPTY_FLAG;
        queue[i].value     = 0;
    }

    int cnt = 0;

    while(1) {
        // sleep();
        while (queue[cnt % TOTAL_GPIO].pinNumber != EMPTY_FLAG)
        {
            handler_user_context(queue[cnt % TOTAL_GPIO].pinNumber,
                                 queue[cnt % TOTAL_GPIO].value);

            /* Clean task */
            queue[cnt % TOTAL_GPIO].pinNumber = EMPTY_FLAG;
            queue[i].value     = 0;

            cnt++;
        }
    }

    return 0;
}
