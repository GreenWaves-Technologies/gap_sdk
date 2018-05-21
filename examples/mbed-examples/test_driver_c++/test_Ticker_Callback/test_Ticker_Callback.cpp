/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/ticker.html
 */
#include "mbed.h"

// A class for flip()-ing a DigitalOut
class Flipper {
public:
    Flipper(int pin) : _pin(pin) {
    }
    void flip() {
        printf("LED 2 flip!\n");
    }
private:
    int _pin;
};

Flipper f(0);
Ticker t;

int main() {
    // the address of the object, member function, and interval
    t.attach(callback(&f, &Flipper::flip), 2);

    // spin in a main loop. flipper will interrupt it to call flip
    while(1) {
        printf("LED 1 flip!\n");
        wait(0.2);
    }
}
