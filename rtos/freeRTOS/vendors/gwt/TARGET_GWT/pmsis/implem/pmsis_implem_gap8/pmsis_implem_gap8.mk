# Cluster sources.
PMSIS_CLUSTER_SRC        = pmsis_cluster/cl_malloc.c \
                           pmsis_cluster/drivers/cluster_synchronisation/cl_to_fc_delegate.c \
                           pmsis_cluster/drivers/cluster_synchronisation/fc_to_cl_delegate.c \
                           pmsis_cluster/drivers/cluster_team/cl_team.c \
                           pmsis_cluster/drivers/delegate/hyperbus/hyperbus_cl_internal.c \
                           pmsis_cluster/drivers/delegate/uart/uart.c \
                           pmsis_cluster/drivers/dma/cl_dma_irq.c

# Drivers sources.
PMSIS_DRIVER_SRC         = pmsis_driver/fc_event/fc_event.c \
                           pmsis_driver/fll/fll.c \
                           pmsis_driver/gpio/gpio.c \
                           pmsis_driver/pad/pad.c \
                           pmsis_driver/pmu/pmu.c \
                           pmsis_driver/pwm/pwm.c \
                           pmsis_driver/pwm/pwm_internal.c \
                           pmsis_driver/rtc/rtc.c \
                           pmsis_driver/rtc/rtc_internal.c \
                           pmsis_driver/timer/timer.c \
                           pmsis_driver/udma/hyperbus/hyperbus.c \
                           pmsis_driver/udma/hyperbus/hyperbus_internal.c \
                           pmsis_driver/udma/cpi/cpi.c \
                           pmsis_driver/udma/cpi/cpi_internal.c \
                           pmsis_driver/udma/dmacpy/dmacpy.c \
                           pmsis_driver/udma/dmacpy/dmacpy_internal.c \
                           pmsis_driver/udma/i2c/i2c.c \
                           pmsis_driver/udma/i2c/i2c_internal.c \
                           pmsis_driver/udma/i2s/i2s.c \
                           pmsis_driver/udma/i2s/i2s_internal.c \
                           pmsis_driver/udma/spi/spi.c \
                           pmsis_driver/udma/spi/spi_internal.c \
                           pmsis_driver/udma/uart/uart.c \
                           pmsis_driver/udma/uart/uart_internal.c \
                           pmsis_driver/perf/perf_internal.c


#$(info #### PMSIS IMPLEM GAP8 PATH : $(PMSIS_IMPLEM_GAP8_DIR))

PMSIS_IMPLEM_CHIP_SRC    = $(PMSIS_CLUSTER_SRC) $(PMSIS_DRIVER_SRC)

# To exclude sources from build.
EXCLUDE_FROM_SRCS        =
PMSIS_IMPLEM_CHIP_SRCS  := $(filter-out $(EXCLUDE_FROM_SRCS), $(PMSIS_IMPLEM_CHIP_SRC))
PMSIS_IMPLEM_CHIP_SRCS  := $(foreach f, $(PMSIS_IMPLEM_CHIP_SRCS), $(PMSIS_IMPLEM_GAP8_DIR)/$f)

PMSIS_IMPLEM_CHIP_INC    = $(PMSIS_IMPLEM_GAP8_DIR) \
                           $(PMSIS_IMPLEM_GAP8_DIR)/cores/TARGET_RISCV_32 \
                           $(PMSIS_IMPLEM_GAP8_DIR)/pmsis_cluster \
                           $(PMSIS_IMPLEM_GAP8_DIR)/pmsis_driver \
                           $(PMSIS_IMPLEM_GAP8_DIR)/pmsis_hal \
                           $(PMSIS_IMPLEM_GAP8_DIR)/targets


#$(info #### Sources : $(PMSIS_IMPLEM_SRCS))
#$(info #### Include : $(PMSIS_IMPLEM_INC))
