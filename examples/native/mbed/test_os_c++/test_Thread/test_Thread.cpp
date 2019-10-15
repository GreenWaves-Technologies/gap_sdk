/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/v5.6/reference/thread.html
 * Modified without LED
*/
#include "mbed.h"

Thread thread;

void led2_thread() {
    while (true) {
        printf("Test thread !\n");
        // Wait for 0.01s
        wait(0.01);
    }
}

int main() {
    thread.start(led2_thread);

    while (true) {
        printf("Main thread !\n");
        // Wait for 0.005s
        wait(0.005);
    }
}
