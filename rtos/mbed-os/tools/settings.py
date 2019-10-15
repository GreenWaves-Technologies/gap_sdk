"""
mbed SDK
Copyright (c) 2016 ARM Limited

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""
###
# Modifications copyright (C) 2018 GreenWaves Technologies
#
# - Add GCC toolchain PATH support for RISC-V GCC and target for GAP
###
from __future__ import print_function
from os import getenv
from os.path import join, abspath, dirname, exists
import logging

ROOT = abspath(join(dirname(__file__), ".."))


##############################################################################
# Toolchains and Build System Settings
##############################################################################
BUILD_DIR = abspath(join(ROOT, "BUILD"))

# Default directory for dual-core and v8 targets
# delivering secure binaries across builds
DELIVERY_DIR = abspath(join(ROOT, "DELIVERY"))

# ARM Compiler 5
ARM_PATH = ""

# ARM Compiler 6
ARMC6_PATH = ""

# GCC ARM
GCC_ARM_PATH = ""

# GCC_RISCV
GCC_RISCV_PATH = ""

# IAR
IAR_PATH = ""

# Goanna static analyser. Please overload it in mbed_settings.py
GOANNA_PATH = ""


# cppcheck path (command) and output message format
CPPCHECK_CMD = ["cppcheck", "--enable=all"]
CPPCHECK_MSG_FORMAT = ["--template=[{severity}] {file}@{line}: {id}:{message}"]

BUILD_OPTIONS = []

# mbed.org username
MBED_ORG_USER = ""

# Print compiler warnings and errors as link format
PRINT_COMPILER_OUTPUT_AS_LINK = False

# Compare against a fixed build of the project for space consumption
COMPARE_FIXED = False

# Print warnings/errors in color
COLOR = False

CLI_COLOR_MAP = {
    "Warning": "yellow",
    "Error"  : "red"
}

##############################################################################
# User Settings (file)
##############################################################################
try:
    # Allow to overwrite the default settings without the need to edit the
    # settings file stored in the repository
    from mbed_settings import *
except ImportError:
    pass


##############################################################################
# User Settings (env vars)
##############################################################################
_ENV_PATHS = ['ARM_PATH', 'GCC_ARM_PATH', 'GCC_RISCV_PATH', 'IAR_PATH', 'ARMC6_PATH']

for _n in _ENV_PATHS:
    if getenv('MBED_'+_n):
        # It's common to provide paths with quotes for certain OSes
        env_path = getenv('MBED_'+_n).strip("\"'")
        if exists(env_path):
            globals()[_n] = env_path
        else:
            print("WARNING: MBED_%s set as environment variable but doesn't"
                  " exist" % _n)

_ENV_VARS = ['PRINT_COMPILER_OUTPUT_AS_LINK', 'COLOR', 'COMPARE_FIXED']
for _n in _ENV_VARS:
    value = getenv('MBED_%s' % _n)
    if value:
        globals()[_n] = value


##############################################################################
# Test System Settings
##############################################################################
SERVER_PORT = 59432
SERVER_ADDRESS = "10.2.200.94"
LOCALHOST = "10.2.200.94"

MUTs = {
    "1" : {"mcu": "LPC1768",
        "port":"COM41", "disk":'E:\\',
        "peripherals": ["TMP102", "digital_loop", "port_loop", "analog_loop", "SD"]
    },
    "2": {"mcu": "LPC11U24",
        "port":"COM42", "disk":'F:\\',
        "peripherals":  ["TMP102", "digital_loop", "port_loop", "SD"]
    },
    "3" : {"mcu": "KL25Z",
        "port":"COM43", "disk":'G:\\',
        "peripherals":  ["TMP102", "digital_loop", "port_loop", "analog_loop", "SD"]
    },
}
