#include "mbed.h"
#include "test_env.h"
#include "rtos.h"

#if defined(MBED_RTOS_SINGLE_THREAD)
  #error [NOT_SUPPORTED] test not supported
#endif

#define SIGNALS_TO_EMIT     100
#define SIGNAL_HANDLE_DELEY 25
#define SIGNAL_SET_VALUE    0x01

/*
 * The stack size is defined in cmsis_os.h mainly dependent on the underlying toolchain and
 * the C standard library. For GCC, ARM_STD and IAR it is defined with a size of 2048 bytes
 * and for ARM_MICRO 512. Because of reduce RAM size some targets need a reduced stacksize.
 */
#if (defined(TARGET_EFM32HG_STK3400)) && !defined(TOOLCHAIN_ARM_MICRO)
    #define STACK_SIZE 512
#elif (defined(TARGET_EFM32LG_STK3600) || defined(TARGET_EFM32WG_STK3800) || defined(TARGET_EFM32PG_STK3401)) && !defined(TOOLCHAIN_ARM_MICRO)
    #define STACK_SIZE 768
#elif (defined(TARGET_EFM32GG_STK3700)) && !defined(TOOLCHAIN_ARM_MICRO)
    #define STACK_SIZE 1536
#elif defined(TARGET_MCU_NRF51822)
    #define STACK_SIZE 768
#elif (defined(TARGET_STM32F070RB) || defined(TARGET_STM32F072RB))
    #define STACK_SIZE DEFAULT_STACK_SIZE/2
#else
    #define STACK_SIZE DEFAULT_STACK_SIZE
#endif

DigitalOut led(LED1);
volatile int signal_counter = 0;

void led_thread(void const *argument) {
    while (true) {
        // Signal flags that are reported as event are automatically cleared.
        ThisThread::flags_wait_all(SIGNAL_SET_VALUE);
        led = !led;
        signal_counter++;
    }
}

int main (void) {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(Signals messaging);
    MBED_HOSTTEST_START("RTOS_4");

    Thread thread(led_thread, NULL, osPriorityNormal, STACK_SIZE);
    bool result = true;

    while (true) {
        ThisThread::sleep_for(2 * SIGNAL_HANDLE_DELEY);
        thread.signal_set(SIGNAL_SET_VALUE);
        if (signal_counter == SIGNALS_TO_EMIT) {
            printf("Handled %d signals\r\n", signal_counter);
            break;
        }
    }
    MBED_HOSTTEST_RESULT(result);
    return 0;
}
