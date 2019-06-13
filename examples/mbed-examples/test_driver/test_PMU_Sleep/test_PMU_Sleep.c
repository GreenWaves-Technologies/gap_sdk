#include<stdio.h>
#include "cmsis.h"
#include "gap_common.h"

/* Wake up mothod choose - 1 for RTC, other for GPIO */
#define PMU_WAKUP_BY_RTC      1

/* Wake up in several second*/
#define COUNT_SECONDS         5

#define    VALUE_INIT         1
#define    VALUE_SET          2

static int my_var = VALUE_INIT;

#if (PMU_WAKUP_BY_RTC == 1)

void pmu_wakeup_by_rtc() {
    printf("Wake up by RTC Timer in %d second !\n", COUNT_SECONDS);

    int repeat_en = 0;
    rtc_config_t config;
    RTC_GetDefaultConfig(&config);

    RTC_Init(RTC_APB, &config);

    /* Set Timer */
    RTC_SetTimer(RTC_APB, COUNT_SECONDS);

    /* Start Timer */
    RTC_StartTimer(RTC_APB, repeat_en);
}

#else

/* Array of PORT and GPIO peripheral base address. */
static GPIO_Type *const gpio_addrs[] = GPIO_BASE_PTRS;
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

void pmu_wakeup_by_gpio() {
    printf("Wake up by GPIO button !\n");

    /* BUTTON config. */
    PinName button = GPIO_A2_A2;

    /* Parsing GPIO pin to get real number for port, gpio and pin*/
    uint32_t port_number = GET_GPIO_PORT   (button);
    uint32_t gpio_number = GET_GPIO_NUM    (button);
    uint32_t pin_number  = GET_GPIO_PIN_NUM(button);


    /* Init PORT - MUX as GPIO/Alter1. */
    port_pin_config_t port_config = { .pullSelect    = uPORT_PullUpEnable,
                                      .driveStrength = uPORT_LowDriveStrength,
                                      .mux           = uPORT_MuxAlt1};

    PORT_SetPinConfig  ( port_addrs[port_number], pin_number,  &port_config);

    /* Init GPIO - INPUT. */
    gpio_pin_config_t gpio_config = { .pinDirection  = uGPIO_DigitalInput,
                                      .outputLogic   = uGPIO_LOW };

    GPIO_PinInit       ( gpio_addrs[port_number], gpio_number, &gpio_config );


    /* Bind GPIO IRQ handler */
    GPIO_IRQHandlerBind(gpio_addrs[port_number], 0);

    /* GPIO IRQ enable */
    SOC_EU_SetFCMask(GPIO_EVENT);

    /* GPIO trigger IRQ by falling edge */
    GPIO_SetPinInterruptConfig(gpio_addrs[port_number], gpio_number, uGPIO_InterruptFallingEdge);

    /* PMU GPIO configure*/
    PMU_EnableGPIOWakeUp(gpio_number, uGPIO_WakeUpFallingEdge);
}
#endif

int main()
{
    printf("BOOT mode = %d\n", PMU_WakeupState());

    if (PMU_WakeupState() == uPMU_COLD_BOOT)
    {
        printf("Entering test from cold boot my_var = %d\n", my_var);

        my_var = VALUE_SET;

        printf("Set my_var = %d\n", my_var);

        #if (PMU_WAKUP_BY_RTC == 1)
        pmu_wakeup_by_rtc();
        #else
        pmu_wakeup_by_gpio();
        #endif

        /* For changing to deep sleep state, just change < uPMU_SWITCH_RETENTIVE_SLEEP > to < uPMU_SWITCH_DEEP_SLEEP > */
        PMU_StateSwitch(uPMU_SWITCH_RETENTIVE_SLEEP, uPMU_SWITCH_FAST);
    }
    else if (PMU_WakeupState() == uPMU_RETENTIVE_SLEEP_BOOT)
    {
        PMU_DisableGPIOWakeUp();
        printf("Wakeup from retentive sleep my_var=%d\n", my_var);

        if (my_var == VALUE_SET) {
            printf("Test success\n");
        }

        #if (PMU_WAKUP_BY_RTC == 1)
        pmu_wakeup_by_rtc();
        #else
        pmu_wakeup_by_gpio();
        #endif

        /* For changing to deep sleep state, just change < uPMU_SWITCH_RETENTIVE_SLEEP > to < uPMU_SWITCH_DEEP_SLEEP > */
        PMU_StateSwitch(uPMU_SWITCH_RETENTIVE_SLEEP, uPMU_SWITCH_FAST);
    }
    else if (PMU_WakeupState() == uPMU_DEEP_SLEEP_BOOT)
    {
        PMU_DisableGPIOWakeUp();
        printf("Wakeup from deep sleep my_var=%d\n", my_var);

        if (my_var == VALUE_INIT) {
            printf("Test success\n");
        }

        #if (PMU_WAKUP_BY_RTC == 1)
        pmu_wakeup_by_rtc();
        #else
        pmu_wakeup_by_gpio();
        #endif

        /* For changing to deep sleep state, just change < uPMU_SWITCH_RETENTIVE_SLEEP > to < uPMU_SWITCH_DEEP_SLEEP > */
        PMU_StateSwitch(uPMU_SWITCH_DEEP_SLEEP, uPMU_SWITCH_FAST);
    }

    printf("Test failed\n");
    return -1;
}
