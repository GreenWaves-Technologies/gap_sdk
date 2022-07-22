.. _guide_freertos:

Using FreeRTOS with PMSIS API and drivers
=========================================

Introduction
------------

FreeRTOS is the default OS in GAP SDK.
While it provides the standard FreeRTOS APIs to users, it also leverages PMSIS 
(Pulp Microcontroller Software Interface Standard), both for drivers and some 
standard system tasks.
This guides explains the basics rules to use FreeRTOS and PMSIS together.

Startup
-------

At startup, in the application :code:`main`, :code:`pmsis_kickoff` will have to be called first.
This will start the scheduler, as well as a small event kernel.
Those two are hard requirements to use any PMSIS driver.

.. warning::

    vTaskSchedulerStart from FreeRTOS **must not** be called manually. Since :code:`pmsis_kickoff` already does it, this would code undefined behaviour.


Tasks
-----

TODO

.. note:: Some warning about multi task, and push async in particular.

Inter tasks synchronisation
---------------------------


All the PMSIS API uses :code:`pi_task` structures for synchronisation.
These can be used either for semaphore like construct (initialized with :code:`pi_task_block`),
or callbacks (initialized with :code:`pi_task_callback`).

These can also be use between tasks, and between a task and callbacks themselves.
xTaskNotify and xTaskSemaphore can also be used, howvever, :code:`pi_task` have
the advantage of being directly usable inside PMSIS functions.

As an example, one could dispatch from a thread and block on another via blocking.

.. code-block:: C


    pi_task_t task_write;
    pi_task_t task_read;
    void task1(void)
    {
        /*
         ....
        */
        while(something)
        {
            // wait on task, and reinit it
            pi_task_wait_on(&task_write);
            pi_task_destroy(&task_write);
            pi_task_block(&task_write);
            pi_some_device_read(some_device,some_buffer, some_size, &task_read);
        }
        /*
         ....
        */

    }

    void task2(void)
    {
        /*
         ....
        */
        while(something)
        {
            pi_some_device_write(some_device,some_buffer, some_size, &task_write)
            // wait on task, and reinit it
            pi_task_wait_on(&task_read);
            pi_task_destroy(&task_read);
            pi_task_block(&task_read);
        }
        /*
         ....
        */
    }

    void main_task(void)
    {
        /*
         ....
        */
        pi_task_block(&task_write);
        pi_task_block(&task_read);
        /*
         ....
        */
    }


.. note::

    :code:`pi_task_t` structures are by nature thread/event safe objects


Timers
------

Precise timers
~~~~~~~~~~~~~~


.. warning:: This only applies to GAP9, on GAP8, these APIs are tick based.

PMSIS API provides access to precise hw timers.
This API's resolution is in the order of 100 micro seconds.
Its two main components are:

:code:`pi_time_wait_us(uint32_t timeout_us)`: 
                                              This API allows to block the current
                                              thread for `timeout_us` micro seconds.

:code:`pi_task_push_delayed_us(pi_task_t *task, uint32_t timeout_us)`: 
                                              This API allows to execute a callback
                                              or unlock a semaphore `timeout_us`
                                              micro seconds in the future.

These APIs allow to completely replace FreeRTOS's :code:`vTaskDelay` and
:code:`xTimerXXX` APIs.
Those have the advantage of being much more precise and lightweight.

The only non direct replacement would be a periodic timer.
It is however extremely straightforward to do:

.. code-block:: C
         
    /*
     .....
    */   

    // example callback that re-pushes itself, effectively creating a periodic
    // timer
    void my_pi_periodic_timer( void *arg )
    {
        pi_task_t *task = (pi_task_t*)arg;
        // re-push the same task with same delay
        pi_task_push_delayed_us(task, 1000);
    }

    void test_sw_timer ( void  )
    {
        /*
         .....
        */
        pi_task_t task;
        // pass task itself as callback argument, for real case, using
        // a structure containing all desired information is recomended.
        pi_task_callback(&task, my_pi_periodic_timer, (void*)&task);
        // delayed push, 1ms
        pi_task_push_delayed_us(&task, 1000);
        /*
         .....
        */
    }
    /*
     .....
    */

.. note:: For full documentation on those APIs, please see API documentation.

Imprecise tick based timer
~~~~~~~~~~~~~~~~~~~~~~~~~~

FreeRTOS provides imprecise timers, which are based on tick resolution.

.. note:: By default, FreeRTOS tick rate is 10ms on GAP9, 1ms on GAP8.

.. warning:: The use of these APIs in non legacy code is NOT recomended. These APIs
          are much less precise and incure larger performance cost than their
          PMSIS counterparts.

These are used via FreeRTOS APIs, and can be one-shot or periodic.
Once a timer is done, it calls the callback given as argument.

An example of one shot timer is as follows:

.. code-block:: C
         
    /*
     .....
    */   
    // a timer array
    TimerHandle_t xTimers[ TIMERS ];

    // example timer callback - just print and release creating task
    void vTimerCallbackOneShot( TimerHandle_t xTimer )
    {
        char *taskname = pcTaskGetName( NULL );
        printf("%s : Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
        pi_task_t *task_block = (pi_task_t*)pvTimerGetTimerID(xTimer);
        pi_task_release(task_block);
    }

    void test_sw_timer ( void  )
    {
        /*
         .....
        */
        // create a freertos timer, with a blocking task as argument
        xTimers[0] = xTimerCreate( "Timer0", pdMS_TO_TICKS( 10 ), pdFALSE,
        (void*)&task_block, vTimerCallback0 );
        if( xTimers[0] == NULL )
        {
            exit(0);
        }
        // start the timer
        xTimerStart( xTimers[0], portMAX_DELAY );
        printf("%s created and started one shot timer. Waiting.\n", taskname);
        /*
         .....
        */
    }
    /*
     .....
    */ 

The callback will be called once, and only once.
After which, the timer will stop on its own and need to be rearmed for further usage.


An example of a periodidc timer is as follows:

.. code-block:: C
         
    /*
     .....
    */   
    // a timer array
    TimerHandle_t xTimers[ TIMERS ];

    static uint32_t call_nb = 0;
    // example timer callback - just print and release after 10 cycles
    void vTimerCallbackPeriodic( TimerHandle_t xTimer )
    {
        call_nb++;
        char *taskname = pcTaskGetName( NULL );
        printf("%s : Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
        pi_task_t *task_block_periodic = (pi_task_t*)pvTimerGetTimerID(xTimer);
        if(call_nb >= 10)
        {
            pi_task_release(task_block_periodic);
            // stop the timer from trigering again
            xTimerStop( xTimers[1], portMAX_DELAY );
        }
    }

    void test_sw_timer ( void  )
    {
        /*
         .....
        */
        pi_task_wait_on(&task_block);
        xTimers[1] = xTimerCreate( "Timer1", pdMS_TO_TICKS( 10 ), pdTRUE,
        (void*)&task_block_periodic, vTimerCallback0 );
        if( xTimers[1] == NULL )
        {
            exit(0);
        }
        // start the timer
        xTimerStart( xTimers[1], portMAX_DELAY );
        /*
         .....
        */
    }
    /*
     .....
    */ 

The periodic callback will be called 10 times, until call_nb reaches 10.
At that point, the timer will be stoped.



Saving dynamic power
--------------------

Any time nothing happens on the core, it will be clock gated.
Thus saving power.
To reach that state, the condition is to have no thread to schedule.
As a consequence it is extremely important not to make threads artificially busy.
This forbids most :code:`while(true){wait_ms()}` constructs.
Instead, code needs to wait on a semaphore or a pi_task initialized via 
:code:`pi_task_block()`.


example:

.. code-block:: C
    
    /* PMSIS includes */
    #include "pmsis.h"

    #define TIMERS ( 1 )
    TimerHandle_t xTimers[ TIMERS ];

    void vTimerCallback0( TimerHandle_t xTimer )
    {
        char *taskname = pcTaskGetName( NULL );
        printf("%s : Callback at TICK = %d.\n", taskname, xTaskGetTickCount());
        pi_task_t *task_block = (pi_task_t*)pvTimerGetTimerID(xTimer);
        pi_task_release(task_block);
    }

    void test_sw_timer ( void  )
    {
        char *taskname = pcTaskGetName( NULL );
        uint32_t ulValue = 0, wait_val = 0;

        pi_task_t task_block;
        // Initialize a blocking task, equivalent to a semaphore
        pi_task_block(&task_block);
        printf("%s creating Timers.\n", taskname);
        // create a freertos timer, with said blocking task as argument
        xTimers[0] = xTimerCreate( "Timer0", pdMS_TO_TICKS( 10 ), pdFALSE,
        (void*)&task_block, vTimerCallback0 );
        if( xTimers[0] == NULL )
        {
            exit(0);
        }

        // start the timer
        xTimerStart( xTimers[0], portMAX_DELAY );

        printf("%s created and started timers. Waiting.\n", taskname);

        pi_task_wait_on(&task_block);

        printf("%s deleting timers and suspending.\n", taskname);
        xTimerStop( xTimers[0], portMAX_DELAY );
        xTimerDelete( xTimers[0], portMAX_DELAY );

        pmsis_exit(0);
    }

    /* Program Entry. */
    int main(void)
    {
        printf("\n\n\t *** SW Timer Test ***\n\n");
        return pmsis_kickoff((void *) test_sw_timer);
    }

In this example, one task wait on an event, here coming from a one shot timer.
During all the wait the task is blocked, and therefore never scheduled.
Since no other tasks are present, FreeRTOS will execute its idle task, which 
will clock gate the core until the next IRQ which could change the system state.
As such, 10ms of dynamic power are potentially saved.
