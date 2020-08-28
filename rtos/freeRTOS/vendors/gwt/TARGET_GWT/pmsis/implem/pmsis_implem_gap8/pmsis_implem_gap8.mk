# Cluster sources.
PMSIS_CLUSTER_SRC             = cluster/malloc/cl_malloc.c \
                                cluster/drivers/cluster_synchronisation/cl_to_fc_delegate.c \
                                cluster/drivers/cluster_synchronisation/fc_to_cl_delegate.c \
                                cluster/drivers/cluster_team/cl_team.c \
                                cluster/drivers/delegate/hyperbus/hyperbus_cl_internal.c \
                                cluster/drivers/delegate/uart/uart_cl_internal.c \
                                cluster/drivers/dma/cl_dma_irq.c

PMSIS_CLUSTER_ASM_SRC         = cluster/drivers/cluster_synchronisation/cluster_core.S

# Drivers sources.
PMSIS_DRIVER_SRC              = drivers/fc_event/fc_event.c \
                                drivers/fll/fll.c \
                                drivers/gpio/gpio.c \
                                drivers/pad/pad.c \
                                drivers/pmu/pmu.c \
                                drivers/pmu/pmu_internal.c \
                                drivers/pwm/pwm.c \
                                drivers/pwm/pwm_internal.c \
                                drivers/rtc/rtc.c \
                                drivers/rtc/rtc_internal.c \
                                drivers/timer/timer.c \
                                drivers/udma/hyperbus/hyperbus.c \
                                drivers/udma/hyperbus/hyperbus_internal.c \
                                drivers/udma/cpi/cpi.c \
                                drivers/udma/cpi/cpi_internal.c \
                                drivers/udma/dmacpy/dmacpy.c \
                                drivers/udma/dmacpy/dmacpy_internal.c \
                                drivers/udma/i2c/i2c.c \
                                drivers/udma/i2c/i2c_internal.c \
                                drivers/udma/i2s/i2s.c \
                                drivers/udma/i2s/i2s_internal.c \
                                drivers/udma/spi/spi.c \
                                drivers/udma/spi/spi_internal.c \
                                drivers/udma/uart/uart.c \
                                drivers/udma/uart/uart_internal.c \
                                drivers/perf/perf_internal.c

PMSIS_DRIVER_ASM_SRC          =


#$(info #### PMSIS IMPLEM GAP8 PATH : $(PMSIS_IMPLEM_GAP8_DIR))

PMSIS_IMPLEM_CHIP_SRC         = $(PMSIS_CLUSTER_SRC) $(PMSIS_DRIVER_SRC)

PMSIS_IMPLEM_CHIP_ASM_SRC     = $(PMSIS_CLUSTER_ASM_SRC) $(PMSIS_DRIVER_ASM_SRC)

# To exclude sources from build.
EXCLUDE_FROM_SRCS             =
PMSIS_IMPLEM_CHIP_SRCS       := $(filter-out $(EXCLUDE_FROM_SRCS), $(PMSIS_IMPLEM_CHIP_SRC))
PMSIS_IMPLEM_CHIP_SRCS       := $(foreach f, $(PMSIS_IMPLEM_CHIP_SRCS), $(PMSIS_IMPLEM_GAP8_DIR)/$f)
PMSIS_IMPLEM_CHIP_ASM_SRCS    = $(foreach f, $(PMSIS_IMPLEM_CHIP_ASM_SRC), $(PMSIS_IMPLEM_GAP8_DIR)/$f)

PMSIS_IMPLEM_CHIP_INC    = $(PMSIS_IMPLEM_GAP8_DIR) \
                           $(PMSIS_IMPLEM_GAP8_DIR)/include \
                           $(PMSIS_IMPLEM_GAP8_DIR)/include/cores/TARGET_RISCV_32 \
                           $(PMSIS_IMPLEM_GAP8_DIR)/include/pmsis/implem \
                           $(PMSIS_IMPLEM_GAP8_DIR)/include/pmsis/targets



#$(info #### Sources : $(PMSIS_IMPLEM_SRCS))
#$(info #### Include : $(PMSIS_IMPLEM_INC))
