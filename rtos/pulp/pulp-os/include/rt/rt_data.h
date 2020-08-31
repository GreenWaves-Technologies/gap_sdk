/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __RT_RT_DATA_H__
#define __RT_RT_DATA_H__

/// @cond IMPLEM

#include "archi/pulp_defs.h"

#if PULP_CHIP == CHIP_GAP
#include "rt/chips/gap/config.h"
#elif PULP_CHIP == CHIP_VEGA
#include "rt/chips/vega/config.h"
#elif PULP_CHIP == CHIP_GAP9
#include "rt/chips/gap9/config.h"
#endif

#define INLINE static inline
#define PMSIS_USE_EXTERNAL_TYPES
#define PMSIS_NO_INLINE_INCLUDE



#include "archi/pulp.h"

#ifndef LANGUAGE_ASSEMBLY

#define   likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)

typedef struct pi_task rt_event_t;

#endif

/// @endcond




#ifndef LANGUAGE_ASSEMBLY

typedef void (*rt_error_callback_t)(void *arg, rt_event_t *event, int error, void *object);

#endif




/// @cond IMPLEM

// This event is used by FC to notify few events to the cluster:
//  - A remote cluster to FC event was handled and the cluster can then try to
//    post again another event in case it was working
//  - There was a remote request handled so the cluster can check iif the one is
//    waiting for has finished
#define RT_CLUSTER_CALL_EVT 1

// This event is used for user notifications to clusters
// This will probably soon be dynamically allocated in order to use
// several events so this should not be used directly as a constant
// but through the notification API.
#define RT_CLUSTER_TRIGGER_EVT 2


#define RT_FC_ENQUEUE_EVENT 1
#define RT_FC_SYNC          2
#define RT_CL_SYNC_EVENT 1
#define PLP_RT_NOTIF_EVENT 3    // TODO this is a temp def, should be removed

// This event is used by the external debug bridge to enqueue tasks to pulp
#define RT_BRIDGE_ENQUEUE_EVENT 4

#define RT_DMA_EVENT 5
#define RT_USER_EVENT 6

#if defined(EU_VERSION) && (EU_VERSION == 1)
#define RT_FORK_EVT 0
#endif

#ifndef LANGUAGE_ASSEMBLY

#include <stddef.h>
#include "archi/pulp.h"

#define RT_L1_GLOBAL_DATA __attribute__((section(".data_l1")))

#define RT_L1_BSS __attribute__((section(".bss_l1")))

#define RT_L1_ALIAS_DATA __attribute__((section(".data_alias_l1")))

#if defined(ARCHI_HAS_L1_ALIAS)
#if !defined(ARCHI_NO_L1_TINY) && !defined(__LLVM__)
#define RT_L1_TINY_DATA __attribute__ ((tiny)) __attribute__((section(".data_tiny_l1")))
#else
#define RT_L1_TINY_DATA __attribute__((section(".data_tiny_l1")))
#endif
#else
#define RT_L1_TINY_DATA RT_L1_GLOBAL_DATA
#endif

#define RT_L1_DATA RT_L1_GLOBAL_DATA
#define L1_DATA RT_L1_DATA
#define PI_L1 RT_L1_DATA
#define PI_CL_L1 RT_L1_DATA

#if (defined(ARCHI_HAS_FC_TCDM) || defined(ARCHI_HAS_L2_ALIAS)) && !defined(__LLVM__) && !defined(RV_ISA_RV32)
#define RT_FC_TINY_DATA __attribute__((section(".data_tiny_fc"))) __attribute__ ((tiny))
#else
#define RT_FC_TINY_DATA __attribute__((section(".data_tiny_fc")))
#endif

#define RT_FC_GLOBAL_DATA __attribute__((section(".data_fc")))

#define RT_FC_SHARED_DATA __attribute__((section(".data_fc")))

#define RT_L2_DATA __attribute__((section(".l2_data")))
#define L2_DATA RT_L2_DATA
#define PI_L2 RT_L2_DATA

#define RT_L2_RET_DATA __attribute__((section(".l2_data")))

#define RT_FC_DATA RT_FC_GLOBAL_DATA
#define PI_FC_L1 RT_FC_DATA

#ifdef __RT_CLUSTER_START
#define RT_LOCAL_DATA RT_L1_GLOBAL_DATA
#else
#define RT_LOCAL_DATA RT_FC_SHARED_DATA
#endif

#if defined(ARCHI_HAS_CLUSTER) && defined(__RT_CLUSTER_START)
#define RT_LOCAL_BSS RT_L1_BSS
#else
#define RT_LOCAL_BSS RT_FC_SHARED_DATA
#endif


#define RT_BOOT_CODE
#define RT_FC_BOOT_CODE



struct rt_thread_s;
struct rt_event_sched_s;
struct pi_task;
struct rt_thread_s;

struct pi_cluster_task_implem
{
  int pending;
  int core_mask;
};

#include "rt/data/rt_data_bridge.h"

typedef struct rt_alloc_block_s {
  int                      size;
  struct rt_alloc_block_s *next;
} rt_alloc_chunk_t;

typedef struct rt_alloc_block_extern_s {
  int                      size;
  struct rt_alloc_block_extern_s *next;
  unsigned int             addr;
} rt_alloc_chunk_extern_t;

typedef struct {
  rt_alloc_chunk_t *first_free;
#ifdef ARCHI_MEMORY_POWER
  uint32_t track_pwd;
  uint32_t *pwd_count;
  uint32_t *ret_count;
  uint32_t bank_size_log2;
  uint32_t first_bank_addr;
#endif
} rt_alloc_t;

typedef struct {
  rt_alloc_chunk_extern_t *first_free;
} rt_extern_alloc_t;


typedef enum {
  RT_THREAD_STATE_READY,
  RT_THREAD_STATE_OTHER,
  RT_THREAD_STATE_WAITING
} rt_thread_state_t;

typedef struct rt_thread_queue_s {
  struct rt_thread_s *first;
  struct rt_thread_s *last;
} rt_thread_queue_t;



struct rt_event_sched_s;

typedef struct rt_event_sched_s {
  struct pi_task *first;
  struct pi_task *last;
  struct pi_task *first_free;
  rt_error_callback_t error_cb;
  void *error_arg;
} rt_event_sched_t;


struct rt_periph_channel_s;

#define RT_PERIPH_COPY_PERIPH_DATA_SIZE 16

typedef struct rt_periph_copy_s {
  unsigned int addr;
  unsigned int size;
  unsigned int cfg;
  unsigned int ctrl;
  unsigned int enqueue_callback;
#if defined(UDMA_VERSION) && UDMA_VERSION == 1
  unsigned int end_callback;
#endif
  struct rt_periph_copy_s *next;
  struct pi_task *event;
  unsigned int repeat;
  unsigned int repeat_size;
  union {
    struct {
      unsigned int rx_addr;
      unsigned int rx_size;
    } dual;
    struct {
      unsigned int hyper_addr;
      unsigned int pending_hyper_addr;
      unsigned int pending_addr;
      unsigned int pending_size;
      char mbr;
      char channel;
      char async;
      char memcpy;
      short stride;
      short length;
      int size_2d;
    } hyper;
    struct {
      unsigned int val[8];
    } raw;
#if defined(UDMA_VERSION) && UDMA_VERSION == 1
    struct {
      unsigned char *data;
      int size;
      uint32_t base;
      short div;
      short xfer_pending;
    } i2c;
#endif
  } u;
#if PULP_CHIP_FAMILY == CHIP_GAP || !defined(ARCHI_HAS_FC)
  char *periph_data;
#else
  char periph_data[RT_PERIPH_COPY_PERIPH_DATA_SIZE];
#endif
} rt_periph_copy_t;


struct pi_mem_slab {
        uint32_t num_blocks;
        size_t block_size;
        char *buffer;
        char *free_list;
        uint32_t num_used;
};

struct pi_task_implem
{
  struct pi_task *next;
  struct rt_thread_s *thread;
  int pending;
  int keep;
  void (*saved_callback)(void *);
  void *saved_arg;
  int saved_pending;

  union {
    rt_periph_copy_t copy;
    struct {
      unsigned int data[8];
    };
    struct {
      unsigned int time;
    };
    rt_bridge_req_t bridge_req;
  };
} __attribute__((packed));

#define CLUSTER_TASK_IMPLEM struct pi_cluster_task_implem implem
#define PI_TASK_IMPLEM struct pi_task_implem implem

typedef struct pi_task{
    struct pi_task *next;
    // Warning, might be accessed inline in asm, and thus can not be moved
    uintptr_t arg[4];
    int8_t done;
    int id;

    PI_TASK_IMPLEM;

} pi_task_t;

#include "pmsis/pmsis_types.h"

#ifndef IMPLEM_MUTEX_OBJECT_TYPE
#define IMPLEM_MUTEX_OBJECT_TYPE \
    void* mutex_object;
#endif

struct pi_mutex
{
  IMPLEM_MUTEX_OBJECT_TYPE
  __pmsis_mutex_func take;
  __pmsis_mutex_func give;
};

#ifndef IMPLEM_SEM_OBJECT_TYPE
#define IMPLEM_SEM_OBJECT_TYPE \
    void* sem_object;
#endif

struct pi_sem
{
  IMPLEM_SEM_OBJECT_TYPE
  __pi_sem_func take;
  __pi_sem_func give;
};


#include "pmsis/cluster/cl_pmsis_types.h"

typedef struct pi_task rt_event_t;


typedef struct rt_thread_s {
  union {
    struct {
        int ra;
        int s0;
        int s1;
        int s2;
        int s3;
        int s4;
        int s5;
        int s6;
        int s7;
        int s8;
        int s9;
        int s10;
        int s11;
        int sp;
    } regs;
    char jmpbuf;
  } u;
  struct rt_thread_s *next;
  struct rt_thread_s *waiting;
  int finished;
  void *status;
  rt_event_t event;
  int state;
  int error;
} rt_thread_t;

typedef struct rt_periph_channel_s {
  rt_periph_copy_t *first;
  rt_periph_copy_t *last;
  rt_periph_copy_t *firstToEnqueue;
  unsigned int base;
  unsigned int data[3];
  void *callback;
} rt_periph_channel_t;


#if defined(UDMA_VERSION) && UDMA_VERSION >= 3
typedef struct {
  rt_periph_copy_t *pendings[2];
  rt_periph_copy_t *first_waiting;
  rt_periph_copy_t *last_waiting;
} rt_periph_spim_t;
#endif



typedef struct {
  int nb_pe;
  void (*entry)(void *);
  void *arg;
  void *stacks;
  int master_stack_size;
  int slave_stack_size;
  rt_event_t *event;
} __rt_cluster_call_t;

typedef struct {
} rt_cluster_call_t;

typedef struct
{
  struct pi_cluster_task *first_call_fc_for_cl;
  struct pi_cluster_task *first_call_fc;
  struct pi_cluster_task *last_call_fc;
} rt_cluster_call_pool_t;

typedef struct cluster_data_t {
  int mount_count;
  rt_event_t *events;
  void *stacks;
  int stacks_size;
  unsigned int trig_addr;
  rt_cluster_call_pool_t *pool;
  int powered_up;
  int state;
  int cid;
  rt_event_t *mount_event;
} rt_fc_cluster_data_t;

typedef struct {
  char *name;
  int channel;
  int itf;
  void *desc;
  union {
    struct {
      int size;
    } hyperram;
  } u;
} rt_dev_t;

typedef struct {
  rt_extern_alloc_t *alloc;
  int channel;
} rt_hyperram_t;

typedef struct {
  rt_periph_copy_t periph_copy;
} rt_flash_copy_t;

struct rt_flash_s;
typedef struct rt_flash_conf_s rt_flash_conf_t;

// BEWARE, assembly offsets in all flash structures must be updated below if this structure is modified
typedef struct rt_flash_dev_s {
  struct rt_flash_s *(*open)(rt_dev_t *dev, rt_flash_conf_t *conf, rt_event_t *event);
  void (*close)(struct rt_flash_s *flash, rt_event_t *event);
  void (*read)(struct rt_flash_s *dev, void *addr, void *data, size_t size, rt_event_t *event);
  void (*program)(struct rt_flash_s *dev, void *data, void *addr, size_t size, rt_event_t *event);
  void (*erase_chip)(struct rt_flash_s *dev, rt_event_t *event);
  void (*erase_sector)(struct rt_flash_s *dev, void *data, rt_event_t *event);
  void (*erase)(struct rt_flash_s *dev, void *addr, int size, rt_event_t *event);

} rt_flash_dev_t;

// BEWARE, assembly offsets in all flash structures must be updated below if this structure is modified
typedef struct rt_flash_s {
  rt_flash_dev_t desc;
} rt_flash_t;

typedef struct rt_hyperflash_s {
  rt_flash_t header;
  int channel;
  struct pi_device device;
} rt_hyperflash_t;

// BEWARE, assembly offsets must be updated below if this structure is modified
typedef struct {
  rt_flash_t header;
  int periph_id;
  unsigned int periph_base;
  rt_periph_copy_t *first_pending_copy;
  rt_periph_copy_t *last_pending_copy;
} rt_mram_t;

typedef struct rt_uart_s {
  int open_count;
  int channel;
  unsigned int baudrate;
  int active;
} rt_uart_t;

typedef struct rt_dma_copy_s {
  int id;
} rt_dma_copy_t;

typedef struct {
  unsigned int cluster_mask;
} rt_iclock_t;

typedef struct {
  unsigned int tas;
  unsigned int addr;
  unsigned int cluster_mask;
} rt_iclock_cl_t;

#if defined(CSR_PCER_NB_EVENTS)

#define RT_PERF_NB_EVENTS (CSR_PCER_NB_EVENTS + 1)

#else

#define RT_PERF_NB_EVENTS 0

#endif

typedef struct {
  unsigned int events;
  unsigned int values[RT_PERF_NB_EVENTS];
} rt_perf_t;


typedef struct rt_i2s_s rt_i2s_t;
typedef struct rt_i2s_conf_s rt_i2s_conf_t;

typedef struct rt_i2s_dev_s {
  rt_i2s_t *(*open)(rt_dev_t *dev, rt_i2s_conf_t *conf, rt_event_t*event);
  void (*close)(rt_i2s_t *dev, rt_event_t*event);
  void (*capture)(rt_i2s_t *dev_name, void *buffer, size_t size, rt_event_t *event);
  void (*start)(rt_i2s_t *handle);
  void (*stop)(rt_i2s_t *handle);
  void (*channel_capture)(rt_i2s_t *dev_name, int channel, void *buffer, size_t size, rt_event_t *event);
} rt_i2s_dev_t;

typedef struct rt_i2s_s {
  rt_i2s_dev_t desc;
  int i2s_freq;
  struct rt_i2s_s *next;
  short decimation_log2;
  unsigned char udma_channel;
  unsigned char i2s_id;
  unsigned char periph_id;
  unsigned char clk;
  char pdm;
  char dual;
  char running;
  char width;
  char channels;
} rt_i2s_t;

typedef struct rt_fs_l2_s {
  uint32_t fs_offset;
  uint32_t reserved0;
  uint32_t fs_size;
  uint32_t reserved1;
} rt_fs_l2_t;


#define FS_READ_THRESHOLD            16
#define FS_READ_THRESHOLD_BLOCK      128
#define FS_READ_THRESHOLD_BLOCK_FULL (FS_READ_THRESHOLD_BLOCK + 8)

typedef struct {

} rt_mutex_t;

typedef struct {
  unsigned int addr;
  unsigned int size;
  unsigned int path_size;
  char name[];
} rt_fs_desc_t;

typedef struct rt_fc_lock_req_s rt_fc_lock_req_t;

typedef struct {
  int locked;
  rt_fc_lock_req_t *waiting;
  rt_fc_lock_req_t *last_waiting;
  rt_thread_t *fc_wait;
#if !defined(ARCHI_HAS_FC)
  uint32_t lock;
#endif
} rt_fc_lock_t;

typedef struct rt_fc_lock_req_s {
  rt_fc_lock_t *lock;
  rt_fc_lock_req_t *next;
  rt_event_t event;
  char done;
  char cid;
  char req_lock;
} rt_fc_lock_req_t;

typedef struct rt_uart_req_s {
  rt_uart_t *uart;
  void *buffer;
  size_t size;
  rt_event_t event;
  char done;
  char cid;
} rt_uart_req_t ;

typedef struct rt_io_wait_req_s {
  rt_event_t event;
  char done;
  char cid;
} rt_io_wait_req_t ;

struct pi_cl_alloc_req_s {
  void *result;
  int flags;
  int size;
  rt_event_t event;
  char done;
  char cid;
};

struct pi_cl_free_req_s {
  void *result;
  int flags;
  int size;
  void *chunk;
  rt_event_t event;
  char done;
  char cid;
};

typedef struct pi_cl_alloc_req_s rt_alloc_req_t;
typedef struct pi_cl_free_req_s rt_free_req_t;


typedef struct pi_cl_hyper_req_s rt_hyperram_req_t ;

typedef struct pi_cl_hyperram_alloc_req_s rt_hyperram_alloc_req_t ;

typedef struct pi_cl_hyperram_free_req_s rt_hyperram_free_req_t ;

typedef struct {
  rt_flash_t *dev;
  void *addr;
  void *flash_addr;
  size_t size;
  rt_event_t event;
  int done;
  unsigned char cid;
  unsigned char is_write;
  unsigned char direct;
} rt_flash_req_t;

typedef struct rt_file_s rt_file_t;

typedef struct {
  rt_file_t *file;
  void *buffer;
  size_t size;
  rt_event_t event;
  int done;
  int result;
  unsigned char cid;
  unsigned char direct;
  unsigned int offset;
} rt_fs_req_t;

typedef struct {
  unsigned int trig_addr;
  int event_id;
} rt_notif_t;

typedef struct {
  char *name;
  unsigned int *config;
} rt_padframe_profile_t;

typedef struct
{
  rt_event_t *event;
  rt_event_t *user_event;
  unsigned int current_time;
  unsigned int period;
  int flags;
} rt_timer_t;




typedef struct rt_task_s rt_task_t;

typedef struct rt_task_s {
  void (*entry)(rt_task_t *task, int id);
  uint32_t args[4];
  rt_task_t *next;
  void *stacks;
  void *cluster;
  rt_event_t *event;
  uint16_t stack_size;
  unsigned char nb_cores;
  unsigned char cid;
  unsigned char nb_cores_to_pop;
  unsigned char nb_cores_to_end;
  unsigned char pending;
} rt_task_t;

typedef struct
{
  rt_task_t *__rt_task_first_fc_for_cl;
  rt_task_t *__rt_task_first_fc;
  rt_task_t *__rt_task_last_fc;
  int nb_cores_done;
} rt_task_cluster_loc_t;

typedef struct
{
  void *stacks;
  unsigned int stack_size;
  int cid;
  rt_event_t *end_event;
  rt_task_cluster_loc_t *loc;
  int nb_cores;
  int free_stacks;
} rt_task_cluster_t;

extern rt_padframe_profile_t __rt_padframe_profiles[];

#ifdef ARCHI_UDMA_HAS_HYPER
extern RT_FC_TINY_DATA void *__rt_hyper_udma_handle;
#endif

#include "rt/data/rt_data_spim.h"
#include "rt/data/rt_data_camera.h"
#include "rt/data/rt_data_i2c.h"

#endif

#define RT_EVENT_T_NEXT       0

#define RT_SCHED_T_FIRST      0
#define RT_SCHED_T_LAST       4
#define RT_SCHED_T_FIRST_FREE 8

#define RT_PERIPH_COPY_CTRL_TYPE_BIT    0
#define RT_PERIPH_COPY_CTRL_TYPE_WIDTH  4
#define RT_PERIPH_COPY_CTRL_CUSTOM_BIT 16
#define RT_PERIPH_COPY_SPIM_STEP1  1
#define RT_PERIPH_COPY_SPIM_STEP2  2
#define RT_PERIPH_COPY_I2C_STEP1   3
#define RT_PERIPH_COPY_I2C_STEP2   4
#define RT_PERIPH_COPY_DUAL        5
#define RT_PERIPH_COPY_HYPER       6
#define RT_PERIPH_COPY_FC_TCDM     7
#define RT_PERIPH_COPY_SPIFLASH    8
#define RT_PERIPH_COPY_SPECIAL_ENQUEUE_THRESHOLD   RT_PERIPH_COPY_DUAL


#define RT_PERIPH_COPY_T_ADDR              0
#define RT_PERIPH_COPY_T_SIZE              4
#define RT_PERIPH_COPY_T_CFG               8
#define RT_PERIPH_COPY_T_CTRL              12
#define RT_PERIPH_COPY_T_ENQUEUE_CALLBACK  16
#define RT_PERIPH_COPY_T_NEXT              20
#define RT_PERIPH_COPY_T_EVENT             24
#define RT_PERIPH_COPY_T_REPEAT            28
#define RT_PERIPH_COPY_T_REPEAT_SIZE       32
#define RT_PERIPH_COPY_T_HYPER_ADDR        36
#define RT_PERIPH_COPY_T_SPIM_USER_SIZE    36
#define RT_PERIPH_COPY_T_RAW_VAL0          36
#define RT_PERIPH_COPY_T_RAW_VAL1          40
#define RT_PERIPH_COPY_T_RAW_VAL2          44
#define RT_PERIPH_COPY_T_RAW_VAL3          48
#define RT_PERIPH_COPY_T_RAW_VAL4          52
#define RT_PERIPH_COPY_T_RAW_VAL5          56
#define RT_PERIPH_COPY_T_RAW_VAL6          60
#define RT_PERIPH_COPY_T_RAW_VAL7          64
#define RT_PERIPH_COPY_T_PERIPH_DATA       68


#define RT_PERIPH_CHANNEL_T_SIZEOF_LOG2      (5)
#define RT_PERIPH_CHANNEL_T_SIZEOF           (8*4)
#define RT_PERIPH_CHANNEL_T_FIRST             0
#define RT_PERIPH_CHANNEL_T_LAST              4
#define RT_PERIPH_CHANNEL_T_FIRST_TO_ENQUEUE  8
#define RT_PERIPH_CHANNEL_T_BASE              12
#define RT_PERIPH_CHANNEL_T_DATA0             16
#define RT_PERIPH_CHANNEL_T_DATA1             20
#define RT_PERIPH_CHANNEL_T_DATA2             24
#define RT_PERIPH_CHANNEL_T_CALLBACK          28

#if defined(UDMA_VERSION) && UDMA_VERSION >= 3
#define RT_PERIPH_SPIM_T_PENDING0      0
#define RT_PERIPH_SPIM_T_PENDING1      4
#define RT_PERIPH_SPIM_T_FIRST_WAITING 8
#define RT_PERIPH_SPIM_T_LAST_WAITING  12
#endif


#define RT_MRAM_T_PERIPH_ID          28
#define RT_MRAM_T_PERIPH_BASE        32
#define RT_MRAM_T_FIRST_PENDING_COPY 36
#define RT_MRAM_T_LAST_PENDING_COPY  40


#define RT_CLUSTER_CALL_T_SIZEOF       (7*4)
#define RT_CLUSTER_CALL_T_NB_PE        0
#define RT_CLUSTER_CALL_T_ENTRY        4
#define RT_CLUSTER_CALL_T_ARG          8
#define RT_CLUSTER_CALL_T_STACKS       12
#define RT_CLUSTER_CALL_T_M_STACK_SIZE 16
#define RT_CLUSTER_CALL_T_S_STACK_SIZE 20
#define RT_CLUSTER_CALL_T_EVENT        24

#define RT_FC_CLUSTER_DATA_T_SIZEOF       (10*4)
#define RT_FC_CLUSTER_DATA_T_MOUNT_COUNT  0
#define RT_FC_CLUSTER_DATA_T_EVENTS       4
#define RT_FC_CLUSTER_DATA_T_CALL_STACKS       8
#define RT_FC_CLUSTER_DATA_T_CALL_STACKS_SIZE  12
#define RT_FC_CLUSTER_DATA_T_TRIG_ADDR         16
#define RT_FC_CLUSTER_DATA_T_CLUSTER_POOL      20

#define RT_TASK_T_ENTRY       (0*4)
#define RT_TASK_T_ARGS0       (1*4)
#define RT_TASK_T_ARGS1       (2*4)
#define RT_TASK_T_ARGS2       (3*4)
#define RT_TASK_T_ARGS3       (4*4)
#define RT_TASK_T_NEXT        (5*4)
#define RT_TASK_T_STACKS      (6*4)
#define RT_TASK_T_CLUSTER     (7*4)
#define RT_TASK_T_EVENT       (8*4)
#define RT_TASK_T_STACK_SIZE  (9*4)
#define RT_TASK_T_NB_CORES    (9*4+2)
#define RT_TASK_T_CID         (9*4+3)
#define RT_TASK_T_NB_CORES_TO_POP (9*4+4)
#define RT_TASK_T_NB_CORES_TO_END (9*4+5)
#define RT_TASK_T_PENDING     (9*4+6)

#define RT_CLUSTER_TASK_ENTRY                 (0*4)
#define RT_CLUSTER_TASK_ARG                   (1*4)
#define RT_CLUSTER_TASK_STACKS                (2*4)
#define RT_CLUSTER_TASK_STACK_SIZE            (3*4)
#define RT_CLUSTER_TASK_SLAVE_STACK_SIZE      (4*4)
#define RT_CLUSTER_TASK_NB_CORES              (5*4)
#define RT_CLUSTER_TASK_COMPLETION_CALLBACK   (6*4)
#define RT_CLUSTER_TASK_STACK_ALLOCATED       (7*4)
#define RT_CLUSTER_TASK_NEXT                  (8*4)
#define RT_CLUSTER_TASK_PENDING               (9*4)
#define RT_CLUSTER_TASK_CORE_MASK             (10*4)


#define RT_CLUSTER_CALL_POOL_T_FIRST_CALL_FC_FOR_CL    (0*4)
#define RT_CLUSTER_CALL_POOL_T_FIRST_CALL_FC           (1*4)
#define RT_CLUSTER_CALL_POOL_T_FIRST_LAST_FC           (2*4)

#define PI_CALLBACK_T_NEXT       (0*4)
#define PI_CALLBACK_T_ENTRY      (1*4)
#define PI_CALLBACK_T_ARG        (2*4)

#define PI_TASK_T_NEXT_NEW       (0*4)
#define PI_TASK_T_ARG_0          (1*4)
#define PI_TASK_T_ARG_1          (2*4)
#define PI_TASK_T_ARG_2          (3*4)
#define PI_TASK_T_ARG_3          (4*4)
#define PI_TASK_T_DONE           (5*4)
#define PI_TASK_T_ID             (6*4)
#define PI_TASK_T_NEXT           (7*4)
#define PI_TASK_T_THREAD         (8*4)
#define PI_TASK_T_PENDING        (9*4)
#define PI_TASK_T_KEEP           (10*4)
#define PI_TASK_T_SAVED_CALLBACK (11*4)
#define PI_TASK_T_SAVED_ARG      (12*4)
#define PI_TASK_T_SAVED_PENDING  (13*4)
#define PI_TASK_T_DATA_0         (14*4)
#define PI_TASK_T_DATA_1         (15*4)
#define PI_TASK_T_DATA_2         (16*4)
#define PI_TASK_T_DATA_3         (17*4)
#define PI_TASK_T_DATA_4         (18*4)
#define PI_TASK_T_DATA_5         (19*4)
#define PI_TASK_T_DATA_6         (20*4)
#define PI_TASK_T_DATA_7         (21*4)

#define CL_DMA_CMD_T_ID          (0*4)
#define CL_DMA_CMD_T_CMD         (1*4)
#define CL_DMA_CMD_T_SIZE        (2*4)
#define CL_DMA_CMD_T_STRIDE      (3*4)
#define CL_DMA_CMD_T_LENGTH      (4*4)
#define CL_DMA_CMD_T_LOC_ADDR    (5*4)
#define CL_DMA_CMD_T_EXT_ADDR    (6*4)
#define CL_DMA_CMD_T_NEXT        (7*4)

/// @endcond


#endif
