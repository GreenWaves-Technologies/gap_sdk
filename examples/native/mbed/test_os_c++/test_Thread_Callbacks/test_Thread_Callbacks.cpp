/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/v5.6/reference/thread.html
 * Modified without LED
*/
#include "mbed.h"

Thread thread;
volatile bool running = true;

// Blink function toggles the led in a long running loop
void blink(int *arg) {
    while (running) {
        printf("blink(%d) !\n", *arg);
        // Wait for 5ms
        wait(0.005);
    }
}

// Spawns a thread to run blink for 5 seconds
int main() {
    int arg = 255;

    thread.start(callback(blink, &arg));
    // Wait for 20ms
    wait(0.02);
    running = false;
    thread.join();
}
