# Event kernel sources.
PMSIS_EVENT_KERNEL_SRC   = event_kernel/event_kernel.c

# Malloc sources.
PMSIS_MALLOC_SRC=
PMSIS_SHARED_MALLOC_SRC=
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
PMSIS_MALLOC_SRC         = malloc/cl_l1_malloc.c \
                           malloc/fc_l1_malloc.c \
                           malloc/l2_malloc.c \
                           malloc/malloc_external.c \
                           malloc/malloc_internal.c \
                           malloc/pi_malloc.c
else ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
PMSIS_SHARED_MALLOC_SRC  = malloc/alloc.c \
                           malloc/alloc_pool.c \
						   malloc/pi_malloc.c
else
$(error "Unknown target chip family: $(TARGET_CHIP_FAMILY)")
endif

# Mem slab sources.
PMSIS_MEM_SLAB_SRC       = mem_slab/mem_slab.c

# OS sources.
PMSIS_OS_SRC             = os/device.c \
                           os/pmsis_task.c


#$(info #### PMSIS RTOS PATH : $(PMSIS_RTOS_DIR))

PMSIS_RTOS_SRC           = pi_log.c \
                           $(PMSIS_EVENT_KERNEL_SRC) \
                           $(PMSIS_MEM_SLAB_SRC) \
                           $(PMSIS_MALLOC_SRC) \
                           $(PMSIS_OS_SRC)

PMSIS_IMPLEM_MALLOC_SRCS := $(foreach f, $(PMSIS_IMPLEM_CHIP_SRCS), $(PMSIS_IMPLEM_DIR)/$f) $(foreach f, $(PMSIS_SHARED_MALLOC_SRC), $(PMSIS_SHARED_IMPLEM_DIR)/$f)

# To exclude sources from build.
EXCLUDE_FROM_SRCS        =
PMSIS_RTOS_SRCS         := $(filter-out $(EXCLUDE_FROM_SRCS), $(PMSIS_RTOS_SRC))
PMSIS_RTOS_SRCS         := $(foreach f, $(PMSIS_RTOS_SRCS), $(PMSIS_RTOS_DIR)/$f) \
						   $(foreach f, $(PMSIS_SHARED_MALLOC_SRC), $(PMSIS_SHARED_IMPLEM_DIR)/$f)


PMSIS_RTOS_INC           = $(PMSIS_RTOS_DIR) \
                           $(PMSIS_RTOS_DIR)/include \
						   $(PMSIS_SHARED_IMPLEM_DIR)/include


#$(info #### Sources : $(PMSIS_RTOS_SRCS))
#$(info #### Include : $(PMSIS_RTOS_INC))
