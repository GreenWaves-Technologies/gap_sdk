#PULP Microcontroller Software Interface Standard (PMSIS)

# TODO:
## Step 1: API for FC to cluster communication

    Basically contains:
    1) Cluster start/stop interface
    2) Dispatch
    3) wait (barrier) interface
    4) Perf counters

## Implied changes:

### Pretty much simple renaming:
    1) start/stop interface: CLUSTER_Start/stop or rt_cluster_mount
    2) wait for current cluster task to finish (on FC): CLUSTER_Wait / need to add on pulp side (check & sleep)
    3) barrier: CLUSTER_SynchBarrier / rt_team_barrier ---> potentially need to add core mask config to mbed?
    4) Perf counters --> definitely add rt_perf_event_e like enumerator in final API (mbed side is completely lacking in that area)

### More important changes:
    All in CLUSTER_SendTask (or future equivalent) and pretty much all on mbed/freertos side
    1) Add dynamic stack size to send task
    2) Add task pipelining for "full" asynchronous accesses
    3) Add callback (only partially implemented)
    
    On both sides:
    1) Unified callback pushback to FC (callback event to be executed by FC at the end of cluster task)
    ----> require some scheduler api unification
    
### Not interesting for now:
    1) Add pulp notifications to mbed (and of course use pulp/PMSIS API)

### Complementary Steps:
    1) Wrap mallocs (L1/FC/XX malloc for mbed, and rt_alloc_mem for pulp)
    2) Create OS specific backends for mbed/pulp/freertos, mbed and freertos will share the same backend for drivers
    3) Create factories and get/set for types we need to abstract (everything related to the scheduler in particular)
    4) Eventualy, promote as many things as possible to pmsis (scheduler...) to remove some abstractions

## Step 2: Event scheduler/yield API

## Implied changes:
### Create a specific event driven task for mbded/freertos
    1) Use pulp events model: in particular for devices callback (i.e. Cluster talks to uDMA or Hyperram/flash)
    2) Place with user chosen priority in standard sysTick driven scheduler
    3) One central task for every callback events: defined at call time (on FC side) but pushed by "slave" (cluster if FC-Cluster communication)
    4) Create a second task, executing with user privileges if need to call user callbacks. User events pushed by sys event handler.
### Abstract this event creation
    1) Basically, make every PMSIS function have an "event" argument
    2) Add basic event scheduler handles to PMSIS: overcommit events, push events, pop events (for the scheduler itself)
### Abstract underlying scheduler, aka Create low level PMSIS frontend/backend handles
    1) Create PMSIS_Yield + PMSIS_sync primitives (yield,mutex,semaphore) to pop/push events or yield when non available
    2) Create PMSIS taks create handles so that event driven task may be created at startup
### Finally, make the event task itself a part of PMSIS
    1) Used as a standard func in PULP
    2) But used as a task in time slice based OSes.

## Step 3: Cluster to FC communication
### Use current model, unless better ideas:
    1) Each driver has a small subdriver for cluster, included as a PMSIS backend
    2) Special events from our CLUSTER, with the required struct created by cluster side driver
    3) Finally executed by event thread on FC.