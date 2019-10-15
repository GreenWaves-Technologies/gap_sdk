/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/eventflags.html
 */
#include "mbed.h"

EventFlags event;

void worker_f()
{
    while (true) {
        event.wait_all(0x1);

        printf("Got signal!\r\n");

        printf("Test success\n");
        #ifdef JENKINS_TEST_FLAG
        exit(0);
        #else
        return;
        #endif
    }
}

int main()
{
    Thread worker;

    worker.start(callback(worker_f));

    while (true) {
        wait(0.01); // Wait for 10ms
        event.set(0x1);
    }
}
