/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/v5.6/reference/semaphore.html
*/
#include "mbed.h"

Semaphore two_slots(2);
Thread t2;
Thread t3;

void test_thread(void const *name) {
    while (true) {
        two_slots.wait();
        printf("%s\n", (const char*)name);
        // Wait for 5ms
        wait(0.005);
        two_slots.release();
    }
}

int main (void) {
    t2.start(callback(test_thread, (void *)"Th 2"));
    t3.start(callback(test_thread, (void *)"Th 3"));

    test_thread((void *)"Th 1");
}
