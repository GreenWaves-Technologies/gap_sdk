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


cmake_minimum_required(VERSION 3.16)


set(LIB_GAP_LIB 0 PARENT_SCOPE)


###############################################################################
# Optional gap lib sources
###############################################################################

if (DEFINED CONFIG_GAP_LIB_G722_ENC_DEC)
    message(STATUS "[${TARGET_NAME} Options GAP LIB] g722 encoder decoder enabled")
    LIST(APPEND GAP_LIB_SRCS g722_encoder_decoder/src/g722_encode.c g722_encoder_decoder/src/g722_decode.c)

    if (DEFINED CONFIG_GAP_LIB_G722_ENC_DEC_RUN_ON_FC)
        message(STATUS "[${TARGET_NAME} Options GAP LIB g722 encoder decoder] Run on FC")
        LIST(APPEND GAP_LIB_OPTIONS "-DRUNNING_ON_FC")
    endif()
endif()

if (DEFINED CONFIG_GAP_LIB_IMG_IO)
    message(STATUS "[${TARGET_NAME} Options GAP LIB] imgIO enabled")
    LIST(APPEND GAP_LIB_SRCS img_io/ImgIO.c)

    if (DEFINED CONFIG_GAP_LIB_IMG_IO_EMUL)
        message(STATUS "[${TARGET_NAME} Options GAP LIB imgIO] EMUL enabled")
        LIST(APPEND GAP_LIB_OPTIONS "-D__EMUL__")
    endif()
endif()

if (DEFINED CONFIG_GAP_LIB_JPEG)
    message(STATUS "[${TARGET_NAME} Options GAP LIB] jpeg enabled")
    LIST(APPEND GAP_LIB_SRCS jpeg/cluster.c jpeg/dct.c jpeg/jpeg_constants.c jpeg/jpeg_encoder.c)

    if (DEFINED CONFIG_QUALITY_100)
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 100")
        LIST(APPEND GAP_LIB_OPTIONS "-DJPEG_Q_100")
    elseif (DEFINED CONFIG_QUALITY_95)
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 95")
        LIST(APPEND GAP_LIB_OPTIONS "-DJPEG_Q_95")
    elseif (DEFINED CONFIG_QUALITY_90)
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 90")
        LIST(APPEND GAP_LIB_OPTIONS "-DJPEG_Q_90")
    elseif (DEFINED CONFIG_QUALITY_20)
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 20")
        LIST(APPEND GAP_LIB_OPTIONS "-DJPEG_Q_20")
    elseif (DEFINED CONFIG_QUALITY_10)
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 10")
        LIST(APPEND GAP_LIB_OPTIONS "-DJPEG_Q_10")
    elseif (DEFINED CONFIG_QUALITY_0)
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 0")
        LIST(APPEND GAP_LIB_OPTIONS "-DJPEG_Q_0")
    else()
        # Default quality set to 50 in the code, without any flag.
        message(STATUS "[${TARGET_NAME} Options GAP LIB jpeg] Quality set to 50")
    endif()
endif()

if (DEFINED CONFIG_GAP_LIB_TESTBENCH)
    message(STATUS "[${TARGET_NAME} Options GAP LIB] testbench enabled")
    LIST(APPEND GAP_LIB_SRCS testbench/testbench.c testbench/testlib_hyper.c testbench/testlib_i2c.c testbench/testlib_i2s.c testbench/testlib_uart.c testbench/testlib.c)
    set(GAP_LIB_TESTBENCH 1 PARENT_SCOPE)
else()
    set(GAP_LIB_TESTBENCH 0 PARENT_SCOPE)
endif()

if (DEFINED CONFIG_GAP_LIB_WAV_IO)
    message(STATUS "[${TARGET_NAME} Options GAP LIB] wavIO enabled")
    LIST(APPEND GAP_LIB_SRCS wav_io/wavIO.c)

    if (DEFINED CONFIG_GAP_LIB_WAV_IO_EMUL)
        message(STATUS "[${TARGET_NAME} Options GAP LIB wavIO] EMUL enabled")
        LIST(APPEND GAP_LIB_OPTIONS "-D__EMUL__")
    endif()
endif()


###############################################################################
# Library Creation
###############################################################################

if (GAP_LIB_SRCS)
    add_library(lib_gap_lib STATIC ${GAP_LIB_SRCS})

    if (GAP_LIB_OPTIONS)
        target_compile_options(lib_gap_lib PUBLIC ${GAP_LIB_OPTIONS})
    endif()

    # Tells that gap_lib is available
    set(LIB_GAP_LIB 1 PARENT_SCOPE)

    set(GAP_LIB_INC_DIRS g722_encoder_decoder/include ./include jpeg testbench)
    target_include_directories(lib_gap_lib PUBLIC ${GAP_LIB_INC_DIRS})

    target_link_libraries(lib_gap_lib PRIVATE freertos)
endif()
