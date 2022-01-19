# Driver sources
LIST(APPEND PMSIS_SRC
    chips/gap9/drivers/i2s/i2s.c
    chips/gap9/drivers/udma/udma_core.c
    )

add_library(pmsis_implem STATIC ${PMSIS_SRC})
target_link_libraries(pmsis_implem PRIVATE freertos)
target_include_directories(pmsis_implem PUBLIC include)
