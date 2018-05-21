/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/memorypool.html
 *             https://os.mbed.com/docs/latest/reference/queue.html
*/
#include "mbed.h"

typedef struct {
    uint32_t    voltage;   /* AD result of measured voltage */
    uint32_t    current;   /* AD result of measured current */
    uint32_t counter;   /* A counter value               */
} message_t;

MemoryPool<message_t, 16> mpool;
Queue<message_t, 16> queue;
Thread thread;

/* Send Thread */
void send_thread (void) {
    uint32_t i = 0;
    while (true) {
        i++; // fake data update
        message_t *message = mpool.alloc();
        message->voltage = (i * 1) * 33;
        message->current = (i * 1) * 11;
        message->counter = i;
        queue.put(message);
        // Wait for 10ms
        wait(0.01);
    }
}

int main (void) {
    thread.start(callback(send_thread));

    while (true) {
        osEvent evt = queue.get();
        if (evt.status == osEventMessage) {
            message_t *message = (message_t*)evt.value.p;
            printf("\nVoltage: %u V\n"   , message->voltage);
            printf("Current: %u A\n"     , message->current);
            printf("Number of cycles: %u\n", message->counter);

            mpool.free(message);

            printf("Test success\n");
            #ifdef JENKINS_TEST_FLAG
            exit(0);
            #else
            return 0;
            #endif
        }
    }
}
