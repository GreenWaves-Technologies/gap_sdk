# Copyright (c) 2022 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# PMSIS implem CMakelist
# Add PMSIS implementation sources to compile 

###############################################################################
# Required sources
###############################################################################

set(PMSIS_IMPLEM_FLL_REQUIRED_SRC           chips/gap9/drivers/fll/fll.c)

set(PMSIS_IMPLEM_MAESTRO_REQUIRED_SRC       chips/gap9/drivers/maestro/maestro.c
                                            chips/gap9/drivers/maestro/maestro_irq.c)

set(PMSIS_IMPLEM_CLUSTER_REQUIRED_SRC       chips/gap9/drivers/cluster/cluster-cc.S
                                            chips/gap9/drivers/cluster/cluster.c
                                            chips/gap9/drivers/cluster/cluster-pe-team.S
                                            chips/gap9/drivers/cluster/cluster-pe-workgroup.S)

set(PMSIS_IMPLEM_UDMA_REQUIRED_SRC          chips/gap9/drivers/udma/udma_core.c)

set(PMSIS_IMPLEM_MALLOC_REQUIRED_SRC        malloc/alloc.c
                                            malloc/alloc_pool.c
                                            malloc/pi_malloc.c)

set(PMSIS_IMPLEM_REQUIRED_SRCS              ${PMSIS_IMPLEM_FLL_REQUIRED_SRC}
                                            ${PMSIS_IMPLEM_MAESTRO_REQUIRED_SRC}
                                            ${PMSIS_IMPLEM_MALLOC_REQUIRED_SRC}
                                            ${PMSIS_IMPLEM_CLUSTER_REQUIRED_SRC}
                                            ${PMSIS_IMPLEM_UDMA_REQUIRED_SRC})

###############################################################################
# Optional sources - Added according to KConfig
###############################################################################

if(DEFINED CONFIG_PMSIS_IMPLEM_FFC)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS  chips/gap9/drivers/ffc/ffc.c)
endif()

if(DEFINED CONFIG_PMSIS_IMPLEM_I2C)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS  chips/gap9/drivers/i2c/i2c.c
                                            chips/gap9/drivers/i2c/i2c_slave.c)
endif()

if(DEFINED CONFIG_PMSIS_IMPLEM_I3C)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS  chips/gap9/drivers/i3c/i3c.c)
endif()

if(DEFINED CONFIG_PMSIS_IMPLEM_I2S)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS  chips/gap9/drivers/i2s/i2s.c)
endif()

if(DEFINED CONFIG_PMSIS_IMPLEM_SPIM)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS  chips/gap9/drivers/spim/spim.c)
endif()


if(DEFINED CONFIG_PMSIS_IMPLEM_UDMA_DATAMOVE)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS      chips/gap9/drivers/udma/udma_datamove.c)
endif()

if(DEFINED CONFIG_PMSIS_IMPLEM_UDMA_TIMEOUT)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS      chips/gap9/drivers/udma/udma_timeout.c)
endif()

if(DEFINED CONFIG_PMSIS_IMPLEM_UDMA_TIMESTAMP)
    LIST(APPEND PMSIS_IMPLEM_OPTIONAL_SRCS      chips/gap9/drivers/udma/udma_timestamp.c)
endif()

###############################################################################
# Library creation
###############################################################################

add_library(pmsis_implem STATIC ${PMSIS_IMPLEM_REQUIRED_SRCS} ${PMSIS_IMPLEM_OPTIONAL_SRCS})
target_link_libraries(pmsis_implem PRIVATE freertos)
target_include_directories(pmsis_implem PUBLIC include)
target_include_directories(pmsis_implem PUBLIC chips/gap9/drivers)
