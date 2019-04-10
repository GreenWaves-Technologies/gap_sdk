#include "gap_common.h"
#include "mbed_wait_api.h"

static int flag = 0;

static PWM_Type *const s_pwm_Base[] = PWM_BASE_PTRS;

static void pwm_handler( void *arg )
{
    flag = 1;
    printf("PWM PWM IRQ!\n");
}

int main()
{
    pwm_config_t config[4];

    pwm_signal_param_t param = {
        .chnum = (pwm_channel_t)0,
        .dutyCyclePercent = 50
    };

    /* Init each timer. */
    for( uint32_t i = 0; i < 4; i++ )
    {
        PWM_GetDefaultConfig(&config[i]);

        /* PWM Configuration and initialize. */
        PWM_Init(s_pwm_Base[i], &config[i]);

        // default to 20ms: standard for servos, and fine for e.g. brightness control
        PWM_SetupPwm(s_pwm_Base[i], &param, 1, 50, uPWM_REF_32K);

        /* Set an output event and enable it. */
        PWM_SetOutputEvent(s_pwm_Base[i], param.chnum, config[i].evtSel);

        /* Bind PWM IRQ handler. */
        PWM_CreateHandler(s_pwm_Base[i], ( pwm_callback_t ) pwm_handler, ( void * ) s_pwm_Base[i]);
    }

    /* Start each timer. */
    for( uint32_t i = 0; i < 4; i++ ) {
        PWM_StartTimer(s_pwm_Base[i]);
    }

    wait(1);

    for( uint32_t i = 0; i < 4; i++ ) {

        PWM_ClearOutputEvent( s_pwm_Base[i], param.chnum, config[i].evtSel );

        PWM_Deinit( s_pwm_Base[i] );
    }

    if (flag)
        printf("Test success\n");
    else
        printf("Test failed\n");
}
