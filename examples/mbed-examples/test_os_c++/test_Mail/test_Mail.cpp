/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/mail.html
 */
#include "mbed.h"

/* Mail */
typedef struct {
    unsigned int voltage; /* AD result of measured voltage */
    unsigned int current; /* AD result of measured current */
    unsigned int counter; /* A counter value               */
} mail_t;

Mail<mail_t, 16> mail_box;
Thread thread;

void send_thread (void) {
    uint32_t i = 0;
    while (true) {
        i++; // fake data update
        mail_t *mail = mail_box.alloc();
        mail->voltage = (i * 1) * 33;
        mail->current = (i * 1) * 11;
        mail->counter = i;
        mail_box.put(mail);
        // Wait for 10ms
        wait(0.01);
    }
}

int main (void) {
    thread.start(callback(send_thread));

    while (true) {
        osEvent evt = mail_box.get();
        if (evt.status == osEventMail) {
            mail_t *mail = (mail_t*)evt.value.p;
            printf("\nVoltage: %u V\n"   , mail->voltage);
            printf("Current: %u A\n"     , mail->current);
            printf("Number of cycles: %u\n", mail->counter);

            mail_box.free(mail);

            printf("Test success\n");
            #ifdef JENKINS_TEST_FLAG
            exit(0);
            #else
            return 0;
            #endif
        }
    }
}
