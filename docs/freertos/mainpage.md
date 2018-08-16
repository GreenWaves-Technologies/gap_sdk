# Introduction
![FreeRTOS]( images/freertos.png "FreeRTOS")

[//]: # (<img style="float: left;" src="images/freertos.png"  width="300" height="150">)

FreeRTOS is an RTOS designed for small and resource constrained micro-controllers, issued under MIT license. It was developed and maintained, since 2003, by Richard Barry and his Real Time Engineers Ltd. It is now under the aegis of [Amazon Web Services](https://aws.amazon.com/blogs/opensource/announcing-freertos-kernel-v10/) and became Amazon FreeRTOS since 2017. FreeRTOS still remains available under MIT license.
Light and easily configurable, it supports multi-tasking, various synchronization mechanism and communication between tasks, and various others features. You may refer to the official website to learn more about [FreeRTOS and its features](https://www.freertos.org/FreeRTOS_Features.html).

## FreeRTOS Structure

~~~
freeRTOS                              
├── demos                     <- This folder contains a "Hello World" demo.
├── lib                       <- The kernel source code and the port files are here.
│   ├── FreeRTOS              <- FreeRTOS source code.
│   │   └── portable          <- This folder can be edited to port the kernel
|   |       |                    on a particular MCU and other special features.
│   │       ├── GCC           <- Compiler used for the port
│   │       │   └── GAP8      <- Actual port files
│   │       └── MemMang       <- Memory management(malloc and free algorithms)
│   ├── third_party
|   |   └── mcu_vendor/gwt    <- Gapuino drivers and features are in this folder.
│   └── include               <- Headers and macros used by the kernel
└── LICENSE
~~~

FreeRTOS is only a kernel for embedded systems. To make use of GAP8 and GAPUINO, a stack has been added between the kernel and the hardware as follows.

![FreeRTOS Structure]( images/freertos_stack_cropped.png "FreeRTOS Stacks")

[//]: # (<img style="float: left;" src="images/freertos_stack_cropped.png">)

For better portability and maintenance, this stack is the same as used in Mbed for low level drivers. This stack contains files to boot, handle interruptions, use features and drivers.

## Port details

The port essentially concerns 3 files :
~~~
~/gap_sdk/freeRTOS $ ls lib/FreeRTOS/portable/GCC/GAP8/
port_asm.S          <- Contains some functions written in RISC-V assembly.
port.c              <- Contains functions needed for the port.
portmacro.h         <- Some macros used for the port.
~~~

# Hello World test

In *demos* folder, there is a simple there is a simple "Hello World" example for FreeRTOS.
Before trying this example, the shell environment shall be configured. To do so, start a terminal and navigate to *gap_sdk* folder and then execute `source` command :
~~~
~/gap_sdk $ source sourceme.sh
~~~

Then navigate to the 'HelloWorld' demo and execute the following command :
~~~
~/gap_sdk/ $ cd freeRTOS/demos/gwt/gap8/common/application_code/
~/gap_sdk/freeRTOS/demos/gwt/gap8/common/application_code/ $ make clean all run
~~~

You should get this as the output
~~~


	 *** Cluster Hello World Test ***


Scheduler starts !
Hello World !
TestHelloWorld on Core 0 of cluster 32 leaving...
Detected end of application, exiting with status: 0
~~~

## Hello World Explanation:

~~~c
/****************************************************************************/

/* Program Entry. */
int main( void )
{
    printf("\n\n\t *** Cluster Hello World Test ***\n\n");

    #if configSUPPORT_DYNAMIC_ALLOCATION == 1

    /* Init memory regions to alloc memory. */
    vPortDefineHeapRegions( xHeapRegions );

    BaseType_t xTask;
    TaskHandle_t xHandleDynamic = NULL;

    xTask = xTaskCreate(
        vTestHelloWorld,
        "TestHelloWorld",
        configMINIMAL_STACK_SIZE * 1,
        NULL,
        tskIDLE_PRIORITY + 1,
        &xHandleDynamic
        );
    if( xTask != pdPASS )
    {
        printf("TestHelloWorld is NULL !\n");
        exit(0);
    }
    #endif //configSUPPORT_DYNAMIC_ALLOCATION

    tasks[0] = xHandleDynamic;

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    printf("\nScheduler starts !\n");
    vTaskStartScheduler();

    /* Exit FreeRTOS */
    return 0;
}
/*-----------------------------------------------------------*/

void vTestHelloWorld( void *parameters )
{
    ( void ) parameters;
    char *taskname = pcTaskGetName( NULL );
    uint32_t coreid = __core_ID(), clusterid = __cluster_ID();

    printf("Hello World !\n");
    printf("%s on Core %d of cluster %d leaving...\n", taskname, coreid, clusterid);
    vTaskSuspend( NULL );
}
/*-----------------------------------------------------------*/
~~~

This test has 2 functions : `main()` and `vTestHelloWorld()`.
The `main()` function is used to create threads, kernel objects(semaphore, mutex,...), and start the scheduler. Once the scheduler has started, there is no return to the `main()` function. Therefore, if you need to share variables or objects between tasks, do not instantiate inside the `main()` function. `vTestHelloWorld()` is the actual function printing "Hello World".

First thing to note is the preprocessor directive. Here the portion of code between the `#if` and `#endif` will be compiled only if the kernel is configured to allocate memory dynamically.

This portion of code initializes heap memory regions, to allocate memory dynamically further(this part will be explained later), and creates a task with the following parameters :

| Parameters | Value |
|:---:|:---:|
| `Function to execute` | vTestHelloWorld    |
| `Task name`           |   "TestHelloWorld" |
| `Stack size`          | configMINIMAL_STACK_SIZE |
| `Parameters to pass to the thread` | NULL |
| `Task's priority` | tskIDLE_PRIORITY + 1 |
| `Task handler` | &xHandleDynamic |

* Each task is attributed a function, a routine, to execute. Parameters can be transmitted from the task creator to the task, 4 arg, and must be casted inside the task. Once the execution of the function is completed, the task is removed from the scheduler by the kernel.
* The stack size must suit the application. Here the macro `configMINIMAL_STACK_SIZE` is used to indicate the minimal stack size for the Idle Task(task with the lowest priority, 0).
Stack overflow can be checked during runtime by the kernel if `configCHECK_FOR_STACK_OVERFLOW` is set in *FreeRTOSConfig.h*. If `configCHECK_FOR_STACK_OVERFLOW` is set, a hook function must be defined by the user.
The released FreeRTOS is set to check overflows and the associated hook function is defined in *FreeRTOS_util.c*. The program is terminated in case of stack overflow. You are free to edit it to suit your application.
* The task priority can be set between the Idle Task priority and `configMAX_PRIORITIES` - 1.
* A `Task handler` is always linked to a task. It can be used to stop, resume or kill a task by another one.

More details on task creation are available [here](https://www.freertos.org/a00019.html) and [FreeRTOS API Reference](https://www.freertos.org/a00106.html).

If the task creation is successful, the scheduler can then be called. From here on, the `main()` function is not reachable.

In `vTestHelloWorld()`, the task prints "Hello World" with its name, core ID and cluster ID. When it is done, the task suspends its execution( `vTaskSuspend( NULL )` ) and the Idle task takes place on the core and cleans memory allocated for the deleted task. When a task is deleted using `vTaskDelete( taskHandle )`, the Idle task must not be starved of processing time to free allocated memory. The *Idle Task hook* should not use any blocking API.

In the released FreeRTOS, *Idle Task hook* is used to get details of tasks or delete suspended tasks. Once again, you may edit *FreeRTOS_util.c* to your convenience.

# FreeRTOS Scheduler

Tasks in FreeRTOS are classified in 4 states :
* **Running** : Task is currently using the core.
* **Ready** : Task is in ready list, ready to take place on the core.
* **Blocked** : Task is waiting for an event. Task can be in this state if the task has called `vTaskDelay()` function, or if a non-null timeout has been defined when the task was waiting for a semaphore, mutex or queue and is not yet available. If the timeout is reached and the object is not available, the task enters in **Ready** state.
* **Suspended** : Same as the **Blocked** state, but without timeout. `vTaskSuspend()` and `xTaskResume()` are the API calls to get in and out of this state.

The states transition diagram is given below.
![Task State](https://www.freertos.org/tskstate.gif "FreeRTOS task states transition diagram").

FreeRTOS scheduler is preemptive, with Round-Robin policy. User can modify its behaviour with `configUSE_PREEMPTION` and `configUSE_TIME_SLICING`, *FreeRTOSConfig.h*.

* `configUSE_PREEMPTION` = 0 :

  * Preemptive mode disabled. Tasks take place on core when
    * the running task enters **Blocked** or **Suspended** state
    * the running task explicitly leaves the core when calling `taskYIELD()`

* `configUSE_PREEMPTION` = 1 :

  * `configUSE_TIME_SLICING` = 0 :

    Preemptive mode enabled without time slicing, i.e processing time(quantum) will not be equal between tasks. Tasks are preempted from the core if

    * a higher priority task enters **Ready** state
    * the running task enters **Blocked** or **Suspended** state
    * the running task explicitly leaves the core when calling `taskYIELD()`

  * `configUSE_TIME_SLICING` = 1 :

    Preemptive mode enabled with time slicing, i.e equal processing time between tasks of equal priority. Tasks are preempted from the core if

    * one or more tasks of equal priority are in **Ready** state.
    * a higher priority task enters **Ready** state
    * the running task enters **Blocked** or **Suspended** state
    * the running task explicitly leaves the core when calling `taskYIELD()`

FreeRTOS is released with preemptive mode and time slicing enabled.

# Memory allocation

## Memory locations

On GAP8, there are 3 locations for data storage :

| Name | Address | Size | Type |
|:---:|:---:|:---:|:---:|
| L2_RAM | 0x1C000000 | 512kB | Primary memory used for both data and instructions storage |
| FC_TCDM | 0x1B000000 | 16kB | Memory used by Fabric Controller for fast access |
| L1_TCDM | 0x10000000 | 64kB | Shared memory between Feature Cluster cores |

## Malloc implementations

FreeRTOS offers 5 algorithms to allocate memory.

`configSUPPORT_DYNAMIC_ALLOCATION` must be set in order to use the following malloc implementations.
* **heap_1** : Simplest implementation of `pvPortMalloc( size )`, malloc function. The requirements to use this implementation : memory size, defined with `configTOTAL_HEAP_SIZE`, and an array for the heap if `configAPPLICATION_ALLOCATED_HEAP` is set.
This implementation allows memory allocation but does not allow to free the allocated memory.

* **heap_2** : Same requirements as of heap_1. This implementation allows allocated memory to be freed, but does not combine adjacent free blocks into a single larger one, so the memory is likey to be fragmented.

* **heap_3** : This implementation is simply a wrapper of the compiler's malloc and free functions. No requirements are needed.

* **heap_4** : Same requirements as of heap_1. This implementation is similar to heap_2, but this one combines adjacent free blocks into a larger one.

* **heap_5** : This implementation requires memory regions for memory allocation. A memory region is defined by the start address of the memory region and its size. Several memory regions can be defined to allocate memory. Memory region initialization must occur before any memory allocation. It is done by a call to `vPortDefineHeapRegions( xHeapRegions )` function.
	In `main()`, this call should be the first instruction, before task creation.


For this release, the heap_5 implementation has been used. L2_RAM is used to allocate memory.

If this implementation does not suit your application, you may switch for another implementation or use compiler's implementation. In that case, you will have to edit the Makefile rules(*freeRTOS_rules.mk*) to add/remove the correct heap implementation in order to compile the project.

## GAP8 Memory allocations

In GAP8, it is possible to allocate memory in each of the 3 memory locations.

| Location | Function | Arg | Type |
|:---:|:---:|:---:|:---:|
| L2_RAM | void *pvPortMalloc() | size_t size | Allocate a chunk of *size* bytes in L2 and returns a pointer |
| L2_RAM | void vPortFree() | void *chunk | Free an allocated memory chunk |
| FC_TCDM | void * FC_Malloc() | int size | Allocate a chunk of *size* bytes in FC memory and returns a pointer |
| FC_TCDM | void FC_MallocFree() | void *chunk, int size | Free an allocated memory chunk. |
| L1_TCDM | void *L1_Malloc() | int size | Allocate a chunk of *size* bytes in shared L1 memory and returns a pointer |
| L1_TCDM | void L1_MallocFree() | void *chunk, int size | Free an allocated memory chunk. |

Few notes :
* Before any call to `FC_Malloc()`(resp. `L1_Malloc()`), it is necessary to call `FC_MallocInit()`(resp. `L1_MallocInit()`).
* Freeing allocated memory chunks in FC_TCDM or L1_TCDM requires a pointer to the allocated chunk and its size. This is to easily combine allocated chunks with adjacent free blocks.

## About stacks

| Type | Size | Location |
|:---:|:---:|:---:|
| Main Stack | 4KB | FC_TCDM |
| Idle Task Stack | 512B | L2_RAM |
| FC Task Stack | User choice | L2_RAM |
| Cluster Task Stack | 1KB | L1_TCDM |

**Main  Stack** refers to the stack allocated for the `main()` function only. In FreeRTOS, when tasks are created and the scheduler called, there is no coming back to this function, this stack also can not be accessed. If variables/objects are to be passed to tasks, consider using global variables or dynamically allocate them in L2.

For each created task, a memory chunk is allocated in L2_RAM, with th size depending on task's function. This is the **FC Task Stack**. When a part of the task is executed on the Cluster, then for each core enabled, 1KB memory is allocated in L1_TCDM(**Cluster Task Stack**).


# Kernel configuration

In this release, kernel is configured as follows :
* **configUSE_PREEMPTION** : Enabled. You can disable preemptive mode by adding this to the Makefile :
~~~
NO_PREEMPTION	= true
~~~
* **configTICK_RATE_HZ** : Set to 1000. Meaning the SysTick interrupt occurs every 1ms.
* **configUSE_IDLE_HOOK** : Enabled. An associated hook function is defined to delete and exit program.
* **configMINIMAL_STACK_SIZE** : Set to 128. The size here is given in number of words, the real stack size is 128\*4=512B. This macro is used to set the Idle Task's stack. If you do not use Idle Task hook function, the size can be decreased.
* **configUSE_TIMERS** : Disabled to reduce binary size. Setting this macro will enable the use of Software Timers API, but the user should also define  **configTIMER_TASK_PRIORITY**, **configTIMER_QUEUE_LENGTH**, **configTIMER_TASK_STACK_DEPTH**.

All the other configurations are in *FreeRTOSConfig.h*, you can edit it to suit your application.

Hook functions and memory regions are defined in *FreeRTOS_util.c* and *FreeRTOS_util.h*.

More details and information are available in [Amazon Reference Manual](https://docs.aws.amazon.com/freertos-kernel/latest/ref/reference130.html) and in [FreeRTOS Kernel Configuration](https://www.freertos.org/a00110.html).

# FreeRTOS API Reference

Documentation and API Reference can be found on [FreeRTOS API Reference](https://www.freertos.org/a00106.html), [Amazon Reference Manual](https://docs.aws.amazon.com/freertos-kernel/latest/ref/welcome.html) or [Amazon Developer Guide](https://docs.aws.amazon.com/freertos-kernel/latest/dg/about.html).
