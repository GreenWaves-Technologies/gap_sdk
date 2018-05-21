/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/v5.6/reference/event.html
*/
#include "mbed.h"
#include "mbed_events.h"

int main() {
    // creates a queue with the default size
    EventQueue queue;

    // events are simple callbacks
    queue.call(printf, "called immediately\n");
    queue.call_in(20, printf, "called in 20 milliseconds\n");
    queue.call_every(10, printf, "called every 10 milliseconds\n");

    // events are executed by the dispatch method, time expire 100ms
    queue.dispatch(50);

    printf("Test success\n");
    #ifdef JENKINS_TEST_FLAG
    exit(0);
    #else
    return 0;
    #endif
}
