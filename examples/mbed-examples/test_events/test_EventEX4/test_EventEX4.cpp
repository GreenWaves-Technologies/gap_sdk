/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/v5.6/reference/event.html
*/
#include "mbed.h"
#include "mbed_events.h"

int main() {

    // Creates an event
    EventQueue queue;

    queue.call_in(5, printf, "this will work\n");

    // The event id uniquely represents the event in the queue
    int id = queue.call_in(10, printf, "will this work?\n");

    // If there was not enough memory necessary to allocate the event,
    // an id of 0 is returned from the call functions
    if (!id) {
        error("oh no!");
    }

    // Events can be cancelled as long as they have not been dispatched. If the
    // event has already expired, cancel has no side-effects.
    queue.cancel(id);

    queue.dispatch(20);

    printf("Test success\n");

    #ifdef JENKINS_TEST_FLAG
    exit(0);
    #else
    return 0;
    #endif
}
