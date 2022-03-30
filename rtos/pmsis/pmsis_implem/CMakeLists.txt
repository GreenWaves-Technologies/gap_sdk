# Driver sources
LIST(APPEND PMSIS_SRC
    chips/gap9/drivers/i2s/i2s.c
    chips/gap9/drivers/spim/spim.c
    chips/gap9/drivers/ffc/ffc.c
    chips/gap9/drivers/udma/udma_core.c
    chips/gap9/drivers/udma/udma_datamove.c
    chips/gap9/drivers/udma/udma_timeout.c
    chips/gap9/drivers/udma/udma_timestamp.c
    chips/gap9/drivers/i2c/i2c.c
    chips/gap9/drivers/i2c/i2c_slave.c
    chips/gap9/drivers/fll/fll.c
    )

add_library(pmsis_implem STATIC ${PMSIS_SRC})
target_link_libraries(pmsis_implem PRIVATE freertos)
target_include_directories(pmsis_implem PUBLIC include)
