.. _guide_cluster_management:

Using the cluster
=================

.. warning::

    This guide is **only for GAP9**.


Execution model
---------------

When the chip has just started-up and entered the main of the application, the whole cluster,
including the cores, is off.

Once the cluster is powered-on using the cluster driver, all the cores start executing some
start-up code to enter a loop where they are waiting for tasks sent by the fabric controller.
The cluster cores have 2 different roles, which make them enter a different loop:

- The cluster controller core. It is core 8 on Gap9. Its role is to serve as the main controller
  of the cluster and thus, receives tasks directly from the fabric controller, and is then in charge
  of organizing the execution of these tasks over the whole cluster. After cluster power-up, it
  enters a loop where it is waiting for tasks sent by the fabric controller.
- Worker cores. They are cores 0 to 7 on gap9. Their role is to execute computation scheduled by the
  cluster controller core. For that, after cluster power-up, they enter a loop where they are waiting
  for work from it, like fork sections.

Cluster stacks
--------------

Since the cluster is off when the chip is starting-up, the cores of the cluster do not need any
stack at startup.

Once the cluster is powered-up and tasks are sent to it, the cluster cores must be allocated stacks.
Default stacks can be allocated to them, or user can either allocate them or just give the desired
stack size.

Depending on the role of the core, it will get a different stack, at a different moment:

- The cluster controller core must get a stack when the cluster is powered-up through the driver.
  This is because the cluster controller is the only core of the cluster which will keep its stack
  whatever the cluster task is executed, even with multiple tasks running.
- The worker cores will get a different stack for each cluster task, and so they will get it only
  when a cluster task is received.

Cluster controller stack
........................

The size of the cluster controller stack can be specified in cluster configuration:

.. code-block:: c

    struct pi_cluster_conf conf;
    pi_cluster_conf_init(&conf);
    conf.cc_stack_size = 0x2000;
    pi_open_from_conf(&cluster_dev, (void *)&conf);
    pi_cluster_open(&cluster_dev);

If no stack size is specified, a default size is chosen by the driver. The driver will allocate the
stack during the driver open.


Workers stacks
..............

The size of each worker stack can be specified when a cluster task is setup:

.. code-block:: c

    pi_cluster_task_t task;
    pi_cluster_task(&task, &entry, NULL);
    pi_cluster_task_stacks(&task, NULL, 0x2000);
    pi_cluster_send_task(&cluster_dev, &task);

The specified size is the size for the stack of one core. The driver will allocate one stack for
each core.
If no size is specified, the driver will take a default one. The driver will try try to reuse the
same stack to avoid allocating it everytime a task is sent. Thus, it will allocate it the first
time a task is sent and will reallocate it if a task with a different stack size is sent.

To avoid that, the application can allocate the stacks itself and give them to the task:

.. code-block:: c

    pi_cluster_task_t task;
    pi_cluster_task(&task, &entry, NULL);
    pi_cluster_task_stacks(&task, my_stacks, 0x2000);
    pi_cluster_send_task(&cluster_dev, &task);

Be careful that in this case, the allocated area must contain a stack for each core of the task,
while the specified size if for the stack of one core.

.. note::

    When they are allocated by the user, the stacks can be reused from one task to another since
    the tasks are executed sequentially on the cluster.

Temporary memory
----------------

Since cluster tasks are executed sequentially, they don't always need exclusive memory areas.
For example, the stacks of a task can easily be reused from one task to another. It's also true
for some data buffers which are used only for temporary data during the task execution.

To ease managing such use-cases, a per-task allocator is provided, called the scratch allocator.
This is a linear allocator, so buffers must be freed in reverse order they are allocated. Also, each
task is having its own allocator. Such memory can be freely use during the execution of the task,
but it will lose its content when the task is done, since it can be used by another task. For permanent
data which should be kept untouched, the classic allocator can be used.

To use it, the application must first declare which part of the cluster memory is reserved for 
scratch allocators when the cluster is opened:

.. code-block:: c

    struct pi_cluster_conf conf;
    pi_cluster_conf_init(&conf);
    conf.scratch_size = 0x8000;

The specified size is removed from the cluster memory classic allocator, and is available to
each cluster task. 

Then memory from the scratch allocator can be allocated and freed using pi_cl_l1_scratch_alloc
and pi_cl_l1_scratch_free.

Temporary stacks
----------------

The scratch allocator is particularly interesting in order to have different sizes for stacks and
temporary buffers between different tasks, depending on the specific needs of each task.

For that, stacks can be allocated within the scratch area, using the scratch allocator:

.. code-block:: c

    pi_cluster_task_t task;
    pi_cluster_task(&task, &entry, NULL);
    void *stacks = pi_cl_l1_scratch_alloc(&cluster_dev, &task, STACK_SIZE*NB_WORKERS);
    pi_cluster_task_stacks(&task, stacks, STACK_SIZE);

They do not need to be freed, they can be allocated only once and given several times to the same
task.

Cluster multi-tasking
---------------------

By default, the cluster will execute the tasks in order until completion. This can cause some
real-time issues if one task is too long, as it will prevent other small tasks from executing
in time.

The cluster driver does not support task preemption, but supports a kind of cooperative
scheduling where long tasks can regularly check if more important tasks need to execute.
However, this is not done with context switching, to avoid allocating several tasks, and requires
the current task to return from its entry point to let another task execute. Later on, the
interrupted task will be resumed by entering again its entry point, and this will be up to the
interrupted task to resume from where it stopped by keeping some kind of internal state.

For that, cluster task can first be assigned a priority:

.. code-block:: c

    pi_cluster_task_t task;
    pi_cluster_task(&task, &entry, NULL);
    pi_cluster_task_priority(&task, 1);

The default priority of a task is 0 and the driver only supports 2 priorities.

Tasks with a priority other than 0 must be enqueued with pi_cluster_enqueue_task or
pi_cluster_enqueue_task_async instead of pi_cluster_send_task or pi_cluster_send_task_async.
Be careful that pi_cluster_enqueue_task and pi_cluster_enqueue_task_async do not support
automatic stack allocation.

Then a low-priority task is supposed to call pi_cl_task_yield from time to time to check if it must
leave its entry point:

.. code-block:: c

    void cluster_entry(void *args)
    {
        my_args_t *my_args = (my_args_t *)args;
        for (; my_args->index < 16; my_args->index++)
        {
            // Do some computation

            // Check is we should leave to let another task execute
            if (pi_cl_task_yield())
            {
                return;
            }
        }
    }

Once there is no more high-priority tasks to execute, the low-priority task will be resumed. It is
important that it continues from the iteration where it stopped.
