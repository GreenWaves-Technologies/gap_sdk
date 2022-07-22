/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"
#include "stdio.h"
#include "pmsis/platforms/gvsoc_calib.h"

#if defined(__GAP9_5__)
#include "calib_gap9_5.h"
#elif defined(__GAP9__)
#include "calib_gap9_v2.h"
#endif

#define NB_CALL 32
#define NB_TASKS 16
#define STACK_SIZE 1024

static pi_cluster_task_t task[NB_TASKS];
static pi_task_t events[NB_TASKS];
static int cluster_insn;
static pi_cluster_task_t perf_task;
static int nb_call;
static int nb_call_done;
static pi_task_t end_task;
static struct pi_device cluster_dev;
static int stacks_size;
static int bench_fc_freq;
static int bench_cl_freq;
static char bench_name[256];
static char bench_desc[256];

static char *get_bench_name(char *bench_name, char *name, int fc_freq, int cl_freq)
{
    sprintf(bench_name, "%s_fc%d_cl%d", name, fc_freq, cl_freq);
    return bench_name;
}

static char *get_bench_name2(char *bench_name, char *name0, char *name1, int fc_freq, int cl_freq)
{
    sprintf(bench_name, "%s%s_fc%d_cl%d", name0, name1, fc_freq, cl_freq);
    return bench_name;
}

static char *get_bench_desc(char *bench_desc, char *name, int fc_freq, int cl_freq)
{
    sprintf(bench_desc, "%s (FC frequency: %d, cluster frequency: %d)", name, fc_freq, cl_freq);
    return bench_desc;
}

static void bench_start()
{
#if defined(CALIB)
    printf("float bench_results[] = {\n");
#endif
}

static void bench_end()
{
#if defined(CALIB)
    printf("};\n");
#endif
}

static int bench_register(char *name, float value, float max_error)
{
    static int bench_id = 0;

#if defined(CALIB)
    printf("    %f, // %s\n", value, name);
#endif

#if defined(CHECK)
    float error = 0.0;
    float ref = bench_results[bench_id];

    error = ((float)value - (float)ref) / ref;
    if (error < 0)
    {
        error = -error;
    }

#ifdef VERBOSE
    printf("Checking (value: %f, ref: %f, error: %f)\n", value, bench_results[bench_id], error);
#endif

    if (error > max_error)
    {
        printf("  ERROR DETECTED (name: %s, expected: %f, got: %f, error: %f, expected error: %f) !!!!\n",
               name, ref, value, error, max_error);
        bench_id++;
        return -1;
    }
#endif

#if defined(BENCH)
    printf("@BENCH@%s=%f@DESC@%s@\n", name, value, name);
#endif

    bench_id++;

    return 0;
}

static void cluster_perf_start(void *arg)
{
    pi_perf_conf((int)arg);
    pi_perf_reset();
    pi_perf_start();
}

static void cluster_perf_stop(void *arg)
{
    pi_perf_stop();
    cluster_insn = pi_perf_read((int)arg);
}

static void cluster_perf_stop2(void *arg)
{
    pi_perf_stop();
    cluster_insn = pi_perf_read((int)arg);
    pi_cl_send_task_to_fc(&end_task);
}

static void cluster_entry(void *arg)
{
    pi_task_t *end_task = (pi_task_t *)arg;
    if (end_task)
    {
        pi_cl_send_task_to_fc(end_task);
    }
}

static int bench_task(char *name, char *score_name, void (*init_callback)(), void (*exec_callback)(), float error_margin)
{
    int cycles = 0;
    int instr = 0;
    int active_cycles = 0;
    int cl_instr = 0;
    int cl_active_cycles = 0;
    int errors = 0;

    init_callback();

    // Loop 3 times to benchmark with hot cache. Results of last iteration will be used
    for (int k = 0; k < 3; k++)
    {
        // Loop twice to collect instructions and active cycles since we have only 1 counter
        for (int l = 0; l < 2; l++)
        {
            if (l == 0)
            {
                pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_INSTR));
                pi_cluster_send_tasklet(&cluster_dev, pi_cluster_tasklet(&perf_task, cluster_perf_start, (void *)(1 << PI_PERF_INSTR)));
            }
            else
            {
                pi_perf_conf((1 << PI_PERF_ACTIVE_CYCLES));
                pi_cluster_send_tasklet(&cluster_dev, pi_cluster_tasklet(&perf_task, cluster_perf_start, (void *)(1 << PI_PERF_ACTIVE_CYCLES)));
            }

            pi_perf_reset();
            pi_perf_start();

            exec_callback();

            pi_perf_stop();

            pi_task_block(&end_task);

            if (l == 0)
            {
                pi_cluster_send_tasklet(&cluster_dev, pi_cluster_tasklet(&perf_task, cluster_perf_stop2, (void *)PI_PERF_INSTR));
                pi_task_wait_on(&end_task);
                cycles = pi_perf_read(PI_PERF_CYCLES);
                instr = pi_perf_read(PI_PERF_INSTR);
                cl_instr = cluster_insn;
            }
            else
            {
                pi_cluster_send_tasklet(&cluster_dev, pi_cluster_tasklet(&perf_task, cluster_perf_stop2, (void *)PI_PERF_ACTIVE_CYCLES));
                pi_task_wait_on(&end_task);
                active_cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
                cl_active_cycles = cluster_insn;
            }
        }
    }

#ifdef VERBOSE
#if 0
    printf("%-34s, FC instructions      , %d\n", name, instr / NB_CALL);
    printf("                                  , FC cycles            , %d\n", cycles / NB_CALL);
    printf("                                  , FC active cycles     , %d\n", active_cycles / NB_CALL);
    printf("                                  , Cluster instructions , %d\n", cl_instr / NB_CALL);
    printf("                                  , Cluster active cycles , %d\n", cl_active_cycles / NB_CALL);
#else
    printf("| %-34s | %21d | %21d | %21d | %21d | %21d |\n", name, active_cycles / NB_CALL, cycles / NB_CALL, instr / NB_CALL, cl_active_cycles / NB_CALL, cl_instr / NB_CALL);
#endif
#endif

    errors += bench_register(get_bench_name2(bench_name, score_name, "fc_cycles", bench_fc_freq, bench_cl_freq), (float)cycles / NB_CALL, error_margin);
    errors += bench_register(get_bench_name2(bench_name, score_name, "fc_active_cycles", bench_fc_freq, bench_cl_freq), (float)active_cycles / NB_CALL, error_margin);
    errors += bench_register(get_bench_name2(bench_name, score_name, "fc_instr", bench_fc_freq, bench_cl_freq), (float)instr / NB_CALL, error_margin);
    errors += bench_register(get_bench_name2(bench_name, score_name, "cl_instr", bench_fc_freq, bench_cl_freq), (float)cl_instr / NB_CALL, error_margin);
    errors += bench_register(get_bench_name2(bench_name, score_name, "cl_active_cycles", bench_fc_freq, bench_cl_freq), (float)cl_active_cycles / NB_CALL, error_margin);

    return errors;
}


/*
 * Synchronous task send
 *
 * The FC send tasks one by one and wait for task completion before going-on with next one.
 * This is using the synchronous task send.
 */
static void bench_send_task_init()
{
    pi_cluster_task(&task[0], cluster_entry, NULL);
}

static void bench_send_task_exec()
{
    for (int i = 0; i < NB_CALL; i++)
    {
        pi_cluster_send_task(&cluster_dev, &task[0]);
    }
}

static int bench_send_task(float error_margin)
{
    return bench_task("Cluster task send", "cluster.task.send.", bench_send_task_init, bench_send_task_exec, error_margin);
}



/*
 * Asynchronous task send
 *
 * The FC send multiple tasks at the same time and wait for them before going-on with the next 
 * set of tasks. 
 * This is using the asynchronous task send and the task_wait_on so it is still very close to
 * a synchronous call. Compared to the sync task send bench, it just benchmarks having multiple
 * tasks in the queue but still include the cost of pi_task_wait_on.
 */
static void bench_send_task_async_init()
{
    for (int i = 0; i < NB_TASKS; i++)
    {
        pi_cluster_task(&task[i], cluster_entry, NULL);
    }
}

static void bench_send_task_async_exec()
{
    for (int i = 0; i < NB_CALL; i += NB_TASKS)
    {
        for (int j = 0; j < NB_TASKS; j++)
        {
            pi_cluster_send_task_async(&cluster_dev, &task[j], pi_task_block(&events[j]));
        }
        for (int j = 0; j < NB_TASKS; j++)
        {
            pi_task_wait_on(&events[j]);
        }
    }
}

static int bench_send_task_async(float error_margin)
{
    return bench_task("Async cluster task send", "cluster.task.send_async.", bench_send_task_async_init, bench_send_task_async_exec, error_margin);
}


/*
 * Callback task send
 *
 * The FC send multiple tasks at the same time and use the task callbacks to go-on with the other
 * tasks.
 */
static void cluster_callback(void *arg)
{
    nb_call_done--;

    if (nb_call)
    {
        int id = (int)arg;
        nb_call--;
        pi_cluster_send_task_async(&cluster_dev, &task[id], pi_task_callback(&events[id], cluster_callback, (void *)id));
    }
    else if (nb_call_done == 0)
    {
        pi_task_push(&end_task);
    }
}

static void bench_send_task_callback_init()
{
    for (int i = 0; i < NB_TASKS; i++)
    {
        pi_cluster_task(&task[i], cluster_entry, NULL);
    }
}

static void bench_send_task_callback_exec()
{
    nb_call = NB_CALL;
    nb_call_done = NB_CALL;
    pi_task_block(&end_task);

    nb_call -= NB_TASKS;

    for (int j = 0; j < NB_TASKS; j++)
    {
        pi_cluster_send_task_async(&cluster_dev, &task[j], pi_task_callback(&events[j], cluster_callback, (void *)j));
    }

    pi_task_wait_on(&end_task);
}

static int bench_send_task_callback(float error_margin)
{
    return bench_task("Callback cluster task send", "cluster.task.send_callback.", bench_send_task_callback_init, bench_send_task_callback_exec, error_margin);
}


/*
 * IRQ callback task send
 *
 * The FC send multiple tasks at the same time and use the task IRQ callbacks to go-on with the other
 * tasks.
 */
static void cluster_callback_irq(void *arg)
{
    nb_call_done--;

    if (nb_call)
    {
        int id = (int)arg;
        nb_call--;
        pi_cluster_send_task_async(&cluster_dev, &task[id], pi_task_irq_callback(&events[id], cluster_callback_irq, (void *)id));
    }
    else if (nb_call_done == 0)
    {
        pi_task_push(&end_task);
    }
}


static void bench_send_task_irq_callback_init()
{
    for (int i = 0; i < NB_TASKS; i++)
    {
        pi_cluster_task(&task[i], cluster_entry, NULL);
    }
}

static void bench_send_task_irq_callback_exec()
{
    nb_call = NB_CALL;
    nb_call_done = NB_CALL;
    pi_task_block(&end_task);

    nb_call -= NB_TASKS;

    for (int j = 0; j < NB_TASKS; j++)
    {
        pi_cluster_send_task_async(&cluster_dev, &task[j], pi_task_irq_callback(&events[j], cluster_callback_irq, (void *)j));
    }

    pi_task_wait_on(&end_task);
}

static int bench_send_task_irq_callback(float error_margin)
{
    return bench_task("IRQ callback cluster task send", "cluster.task.send_irq.", bench_send_task_irq_callback_init, bench_send_task_irq_callback_exec, error_margin);
}


/*
 * Synchronous task enqueue
 *
 * The FC enqueue tasks one by one and wait for task completion before going-on with next one.
 * This is using the synchronous task enqueue.
 */
static void bench_enqueue_task_init()
{
    pi_cluster_task(&task[0], cluster_entry, NULL);
    void *stacks = pi_cl_l1_scratch_alloc(&cluster_dev, &task[0], stacks_size);
    pi_cluster_task_stacks(&task[0], stacks, STACK_SIZE);
}

static void bench_enqueue_task_exec()
{
    for (int i = 0; i < NB_CALL; i++)
    {
        pi_cluster_enqueue_task(&cluster_dev, &task[0]);
    }
}

static int bench_enqueue_task(float error_margin)
{
    return bench_task("Cluster task enqueue", "cluster.task.enqueue.", bench_enqueue_task_init, bench_enqueue_task_exec, error_margin);
}



/*
 * Asynchronous task enqueue
 *
 * The FC enqueues multiple tasks at the same time and wait for them before going-on with the next 
 * set of tasks. 
 * This is using the asynchronous task enqueue and the task_wait_on so it is still very close to
 * a synchronous call. Compared to the sync task enqueue bench, it just benchmarks having multiple
 * tasks in the queue but still include the cost of pi_task_wait_on.
 */
static void bench_enqueue_task_async_init()
{
    for (int i = 0; i < NB_TASKS; i++)
    {
        pi_cluster_task(&task[i], cluster_entry, NULL);
        void *stacks = pi_cl_l1_scratch_alloc(&cluster_dev, &task[i], stacks_size);
        pi_cluster_task_stacks(&task[i], stacks, STACK_SIZE);
    }
}

static void bench_enqueue_task_async_exec()
{
    for (int i = 0; i < NB_CALL; i += NB_TASKS)
    {
        for (int j = 0; j < NB_TASKS; j++)
        {
            pi_cluster_enqueue_task_async(&cluster_dev, &task[j], pi_task_block(&events[j]));
        }
        for (int j = 0; j < NB_TASKS; j++)
        {
            pi_task_wait_on(&events[j]);
        }
    }
}

static int bench_enqueue_task_async(float error_margin)
{
    return bench_task("Async cluster task enqueue", "cluster.task.enqueue_async.", bench_enqueue_task_async_init, bench_enqueue_task_async_exec, error_margin);
}




/*
 * Callback task enqueue
 *
 * The FC enqueues multiple tasks at the same time and use the task callbacks to go-on with the other
 * tasks.
 */
static void cluster_callback_enqueue(void *arg)
{
    nb_call_done--;

    if (nb_call)
    {
        int id = (int)arg;
        nb_call--;
        pi_cluster_enqueue_task_async(&cluster_dev, &task[id], pi_task_callback(&events[id], cluster_callback_enqueue, (void *)id));
    }
    else if (nb_call_done == 0)
    {
        pi_task_push(&end_task);
    }
}


static void bench_enqueue_task_callback_init()
{
    for (int i = 0; i < NB_TASKS; i++)
    {
        pi_cluster_task(&task[i], cluster_entry, NULL);
    }
}

static void bench_enqueue_task_callback_exec()
{
    nb_call = NB_CALL;
    nb_call_done = NB_CALL;
    pi_task_block(&end_task);

    nb_call -= NB_TASKS;

    for (int j = 0; j < NB_TASKS; j++)
    {
        pi_cluster_enqueue_task_async(&cluster_dev, &task[j], pi_task_callback(&events[j], cluster_callback_enqueue, (void *)j));
    }

    pi_task_wait_on(&end_task);
}

static int bench_enqueue_task_callback(float error_margin)
{
    return bench_task("Callback cluster task enqueue", "cluster.task.enqueue_callback.", bench_enqueue_task_callback_init, bench_enqueue_task_callback_exec, error_margin);
}



/*
 * IRQ callback task enqueue
 *
 * The FC enqueues multiple tasks at the same time and use the task IRQ callbacks to go-on with the other
 * tasks.
 */

static void cluster_callback_irq_enqueue(void *arg)
{
    nb_call_done--;

    if (nb_call)
    {
        int id = (int)arg;
        nb_call--;
        pi_cluster_enqueue_task_async(&cluster_dev, &task[id], pi_task_irq_callback(&events[id], cluster_callback_irq_enqueue, (void *)id));
    }
    else if (nb_call_done == 0)
    {
        pi_task_push(&end_task);
    }
}



static void bench_enqueue_task_irq_callback_init()
{
    for (int i = 0; i < NB_TASKS; i++)
    {
        pi_cluster_task(&task[i], cluster_entry, NULL);
    }

}

static void bench_enqueue_task_irq_callback_exec()
{
    nb_call = NB_CALL;
    nb_call_done = NB_CALL;
    pi_task_block(&end_task);

    nb_call -= NB_TASKS;

    for (int j = 0; j < NB_TASKS; j++)
    {
        pi_cluster_enqueue_task_async(&cluster_dev, &task[j], pi_task_irq_callback(&events[j], cluster_callback_irq_enqueue, (void *)j));
    }

    pi_task_wait_on(&end_task);
}

static int bench_enqueue_task_irq_callback(float error_margin)
{
    return bench_task("IRQ callback cluster task enqueue", "cluster.task.enqueue_irq.", bench_enqueue_task_irq_callback_init, bench_enqueue_task_irq_callback_exec, error_margin);
}

/*
 * Tasklet send
 *
 * The FC send multiple tasklets at the same time and wait for them before going-on with the next 
 * set of tasklets. 
 */
static void bench_send_tasklet_init()
{
}

static void bench_send_tasklet_exec()
{
    for (int i = 0; i < NB_CALL; i += NB_TASKS)
    {
        pi_task_block(&end_task);

        for (int j = 0; j < NB_TASKS; j++)
        {
            pi_cluster_send_tasklet(&cluster_dev, pi_cluster_tasklet(&task[j], cluster_entry, j == NB_TASKS - 1 ? &end_task : NULL));
        }
        pi_task_wait_on(&end_task);
    }
}

static int bench_send_tasklet(float error_margin)
{
    return bench_task("Cluster tasklet send", "cluster.task.tasklet.", bench_send_tasklet_init, bench_send_tasklet_exec, error_margin);
}



/*
 * Runner
 */
static int launch_benchs(int fc_freq, int cl_freq, float error_margin)
{
    int errors = 0;

    bench_fc_freq = fc_freq;
    bench_cl_freq = cl_freq;

    pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq);
    pi_freq_set(PI_FREQ_DOMAIN_CL, cl_freq);

#ifdef VERBOSE
    printf("| ---------------------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- |\n");
    printf("| %-34s | %-21s | %-21s | %-21s | %-21s | %-21s |\n", "Name", "FC active cycles", "FC cycles", "FC instructions", "Cluster active cycles", "Cluster instructions");
    printf("| ---------------------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- |\n");
#endif

    errors += bench_send_task(error_margin);
    errors += bench_send_task_async(error_margin);
    errors += bench_send_task_callback(error_margin);
    errors += bench_send_task_irq_callback(error_margin);
    errors += bench_enqueue_task(error_margin);
    errors += bench_enqueue_task_async(error_margin);
    errors += bench_enqueue_task_callback(error_margin);
    errors += bench_enqueue_task_irq_callback(error_margin);
    errors += bench_send_tasklet(error_margin);

#ifdef VERBOSE
    printf("| ---------------------------------- | --------------------- | --------------------- | --------------------- | --------------------- | --------------------- |\n");
#endif

    return errors;
}

int test_entry()
{
    struct pi_cluster_conf conf;
    int errors = 0;

    pi_cluster_conf_init(&conf);

    conf.id = 0;

    pi_open_from_conf(&cluster_dev, &conf);
    pi_cluster_open(&cluster_dev);

    bench_start();

    errors += launch_benchs(50, 50, CALIB_CLUSTER_TASK_NOMINAL);

#if defined(CALIB) || defined(CHECK)
    errors += launch_benchs(50, 300, CALIB_CLUSTER_TASK_CORNER);
    errors += launch_benchs(300, 50, CALIB_CLUSTER_TASK_CORNER);
    errors += launch_benchs(300, 300, CALIB_CLUSTER_TASK_NOMINAL);
#endif

    bench_end();

    pi_cluster_close(&cluster_dev);

    if (errors)
    {
        printf("Failed with %d errors\n", errors);
    }

    return errors;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}