# Copyright 2018-present PIO Plus <contact@pioplus.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""
mbed framework

Fork of Mbed OS for GAP8 by GreenWaves Technologies

https://github.com/GreenWaves-Technologies/mbed-os
"""

from os.path import isdir, join

Import("env")

platform = env.PioPlatform()
SDK_DIR = platform.get_package_dir("framework-gap_sdk")
FRAMEWORK_DIR = join(SDK_DIR, "mbed-os")
TOOLCHAIN_DIR = platform.get_package_dir("toolchain-riscv-pulp")
assert SDK_DIR and isdir(SDK_DIR)
assert FRAMEWORK_DIR and isdir(FRAMEWORK_DIR)
assert TOOLCHAIN_DIR and isdir(TOOLCHAIN_DIR)


def get_toolchain_version():
    version = platform.get_package_version("toolchain-riscv-pulp")
    version = version.split(".")[1]
    version = version.replace('0', '.')
    return version


board_config = env.BoardConfig()

toolchain_version = get_toolchain_version()
target_path = join(FRAMEWORK_DIR, "targets",
                   "TARGET_%s" % board_config.get("build.target"))
variant = board_config.get("build.variant")
variant_path = join(target_path, "TARGET_%s" % variant)

env.Append(
    CCFLAGS=[
        "-Os",
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
        "-Wno-unused-function",
        "-Wno-unused-variable",
        "-Wno-deprecated-declarations",
        "-ffunction-sections",
        "-fdata-sections",
        "-mPE=8",
        "-mFC=1",
        "-Wno-missing-field-initializers",
        "-Wno-unused-but-set-variable",
        "-fmessage-length=0",
        "-fno-exceptions",
        "-fno-builtin",
        "-funsigned-char",
        "-fno-delete-null-pointer-checks",
        "-fomit-frame-pointer",
        ("-include", join(target_path, "mbed_config.h")),
        "-march=%s" % board_config.get("build.march")
    ],

    CFLAGS=["-std=gnu99"],

    CXXFLAGS=[
        "-std=gnu++98",
        "-fno-rtti",
        "-Wvla"
    ],

    CPPDEFINES=[
        # DEVICE_FLAGS
        ("DEVICE_SPI_ASYNCH", 1),
        ("DEVICE_SPI", 1),
        ("DEVICE_SERIAL", 1),
        ("DEVICE_SERIAL_ASYNCH", 1),
        ("DEVICE_HYPERBUS_ASYNCH", 1),
        ("DEVICE_HYPERBUS", 1),
        ("DEVICE_STDIO_MESSAGES", 1),
        ("DEVICE_SLEEP", 1),
        ("DEVICE_PORTIN", 1),
        ("DEVICE_PORTOUT", 1),
        ("DEVICE_PORTINOUT", 1),
        ("DEVICE_I2C", 1),
        ("DEVICE_I2C_ASYNCH", 1),
        ("DEVICE_I2S", 1),
        ("DEVICE_RTC", 1),
        ("DEVICE_INTERRUPTIN", 1),
        ("DEVICE_PWMOUT", 1),

        # FEATURE_FLAGS
        ("FEATURE_CLUSTER", 1),

        # MBED_FLAGS
        ("__MBED__", 1),
        "TOOLCHAIN_GCC_RISCV",
        "TOOLCHAIN_GCC"
    ],

    CPPPATH=[
        variant_path,
        join(variant_path, "device"),
        join(variant_path, "driver"),
        join(variant_path, "pins"),
        join(FRAMEWORK_DIR),
        join(FRAMEWORK_DIR, "platform"),
        join(FRAMEWORK_DIR, "hal"),
        join(FRAMEWORK_DIR, "drivers"),
        join(FRAMEWORK_DIR, "events"),
        join(FRAMEWORK_DIR, "events", "equeue"),
        join(FRAMEWORK_DIR, "cmsis"),
        join(FRAMEWORK_DIR, "cmsis", "TARGET_RISCV_32"),
        join(FRAMEWORK_DIR, "features"),
        join(FRAMEWORK_DIR, "features", "storage"),
        join(FRAMEWORK_DIR, "features", "storage", "blockdevice"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem", "rofs"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem", "bd"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem", "fat"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem", "fat",
             "ChaN"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem", "littlefs"),
        join(FRAMEWORK_DIR, "features", "storage", "filesystem", "littlefs",
             "littlefs"),
        join(FRAMEWORK_DIR, "features", "spif-driver"),
        join(FRAMEWORK_DIR, "features", "i2cee-driver"),
        join(FRAMEWORK_DIR, "features", "FEATURE_CLUSTER"),
        join(FRAMEWORK_DIR, "features", "frameworks", "greentea-client"),
        join(FRAMEWORK_DIR, "features", "frameworks", "greentea-client",
             "greentea-client"),
        join(FRAMEWORK_DIR, "features", "frameworks", "unity"),
        join(FRAMEWORK_DIR, "features", "frameworks", "unity", "unity"),
        join(FRAMEWORK_DIR, "features", "frameworks", "utest"),
        join(FRAMEWORK_DIR, "features", "frameworks", "utest", "utest"),
        join(FRAMEWORK_DIR, "rtos"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV", "rtx4"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV", "rtx5"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV", "rtx5", "Include"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV", "rtx5", "RTX", "Include"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV", "rtx5", "RTX", "Source"),
        join(FRAMEWORK_DIR, "rtos", "TARGET_RISCV", "rtx5", "RTX", "Config"),
        join(FRAMEWORK_DIR, "targets", "TARGET_GWT", "api"),
        join(FRAMEWORK_DIR, "targets", "TARGET_GWT", "TARGET_GAP8"),
        join(FRAMEWORK_DIR, "targets", "TARGET_GWT", "TARGET_GAP8", "device"),
        join(FRAMEWORK_DIR, "targets", "TARGET_GWT", "TARGET_GAP8", "driver"),
        join(FRAMEWORK_DIR, "targets", "TARGET_GWT", "TARGET_GAP8", "pins"),
        join(target_path, "libs", "newlib", "extra", "stdio", "tinyprintf"),
        join(target_path, "api")
    ],

    LINKFLAGS=[
        "-nostartfiles",
        "-Wl,--gc-sections",
        "-Wl,--wrap,main",
        "-Wl,--wrap,_malloc_r",
        "-Wl,--wrap,_free_r",
        "-Wl,--wrap,_realloc_r",
        "-Wl,--wrap,_memalign_r",
        "-Wl,--wrap,_calloc_r",
        "-Wl,--wrap,atexit",
        "-Wl,-n",
        join(TOOLCHAIN_DIR, "lib", "gcc", "riscv32-unknown-elf",
             toolchain_version, "crtbegin.o"),
        join(TOOLCHAIN_DIR, "lib", "gcc", "riscv32-unknown-elf",
             toolchain_version, "crti.o"),
        join(TOOLCHAIN_DIR, "lib", "gcc", "riscv32-unknown-elf",
             toolchain_version, "crtn.o"),
        join(TOOLCHAIN_DIR, "lib", "gcc", "riscv32-unknown-elf",
             toolchain_version, "crtend.o")
    ],

    LIBPATH=[
        join(target_path, "libs"),
        join(target_path, "libs", "newlib"),
        join(target_path, "TARGET_%s" % variant, "device", "ld")
    ],

    LIBS=["c", "m", "gcc", "-lstdc++"]
)

env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])

#
# Target: Build Core files from framework
#

env.BuildSources(
    join("$BUILD_DIR", "CmsisTargetCortexA"),
    join(FRAMEWORK_DIR, "cmsis", "TARGET_CORTEX_A")
)

env.BuildSources(
    join("$BUILD_DIR", "CmsisTargetCortexM"),
    join(FRAMEWORK_DIR, "cmsis", "TARGET_CORTEX_M"),
    src_filter="+<*> -<TOOLCHAIN_IAR>"
)

env.BuildSources(
    join("$BUILD_DIR", "TargetAPI"),
    join(target_path, "api")
)

env.BuildSources(
    join("$BUILD_DIR", "TargetDriver"),
    join(variant_path, "driver")
)

env.BuildSources(
    join("$BUILD_DIR", "TargetDevice"),
    join(variant_path, "device")
)

env.BuildSources(
    join("$BUILD_DIR", "TargetPins"),
    join(variant_path, "pins")
)

env.BuildSources(
    join("$BUILD_DIR", "FeaturesCluster"),
    join(FRAMEWORK_DIR, "features", "FEATURE_CLUSTER")
)


env.BuildSources(
    join("$BUILD_DIR", "FeaturesFrameworksGreenTeaClient"),
    join(FRAMEWORK_DIR, "features", "frameworks", "greentea-client")
)

env.BuildSources(
    join("$BUILD_DIR", "FeaturesFrameworksUnity"),
    join(FRAMEWORK_DIR, "features", "frameworks", "unity")
)

env.BuildSources(
    join("$BUILD_DIR", "FeaturesFrameworksUtest"),
    join(FRAMEWORK_DIR, "features", "frameworks", "utest"),
    src_filter="+<*> -<TESTS>"
)

env.BuildSources(
    join("$BUILD_DIR", "Rtos"),
    join(FRAMEWORK_DIR, "rtos"),
    src_filter="+<*> -<TARGET_CORTEX>"
)


env.BuildSources(
    join("$BUILD_DIR", "MbedDrivers"),
    join(FRAMEWORK_DIR, "drivers")
)

env.BuildSources(
    join("$BUILD_DIR", "MbedHAL"),
    join(FRAMEWORK_DIR, "hal"),
    src_filter="+<*> -<TARGET_FLASH_CMSIS_ALGO> -<storage_abstraction>"
)

env.BuildSources(
    join("$BUILD_DIR", "MbedPlatform"),
    join(FRAMEWORK_DIR, "platform")
)

env.BuildSources(
    join("$BUILD_DIR", "MbedEvents"),
    join(FRAMEWORK_DIR, "events"),
    src_filter="+<*> -<equeue/test*>"
)

env.BuildSources(
    join("$BUILD_DIR", "MbedStorage"),
    join(FRAMEWORK_DIR, "features", "storage"),
    src_filter=[
        "+<*>",
        "-<FEATURE_STORAGE>",
        "-<nvstore>",
        "-<system_storage>",
        "-<TESTS>",
        "-<filesystem/littlefs/TESTS>",
        "-<filesystem/littlefs/littlefs/test>",
        "-<filesystem/littlefs/littlefs/emubd>"
    ]
)
