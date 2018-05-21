/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/rtostimer.html
*/
#include "mbed.h"

void blink(void const *n) {
    printf("LED_%u blinking!\n", (int)n);
}

int main(void) {
    RtosTimer led_1_timer(blink, osTimerPeriodic, (void *)0);
    RtosTimer led_2_timer(blink, osTimerPeriodic, (void *)1);
    RtosTimer led_3_timer(blink, osTimerPeriodic, (void *)2);
    RtosTimer led_4_timer(blink, osTimerPeriodic, (void *)3);

    led_1_timer.start(20); // call blink every 20ms
    led_2_timer.start(10); // call blink every 10ms
    led_3_timer.start(5);  // call blink every 5ms
    led_4_timer.start(3);  // call blink every 3ms

    Thread::wait(osWaitForever);
}
