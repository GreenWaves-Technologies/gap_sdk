#include "mbed.h"

InterruptIn  pin0(GPIO_A0_A3); // change this to the button on your board
InterruptIn  pin1(GPIO_A1_B2); // change this to the button on your board
InterruptIn  pin2(GPIO_A2_A2); // change this to the button on your board
InterruptIn  pin3(GPIO_A3_B1); // change this to the button on your board

void handler0() {
    printf("pin0\n");
}

void handler1() {
    printf("pin1\n");
}
void handler2() {
    printf("pin2\n");
}
void handler3() {
    printf("pin3\n");
}

int main()
{

    pin0.rise(&handler0);
    pin1.rise(&handler1);
    pin2.rise(&handler2);
    pin3.rise(&handler3);
    while(1) {           // wait around, interrupts will interrupt this!
        wait(0.25);
    }

}
