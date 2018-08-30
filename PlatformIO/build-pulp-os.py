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
PULP OS

The open source embedded RTOS produced by the PULP project

https://github.com/GreenWaves-Technologies/gap_sdk/tree/master/pulp-os
"""

from os.path import isdir, join

Import("env")

platform = env.PioPlatform()
SDK_DIR = platform.get_package_dir("framework-gap_sdk")
PULP_TOOLS_DIR = platform.get_package_dir("tool-pulp_tools")
FRAMEWORK_DIR = join(SDK_DIR, "pulp-os")
assert SDK_DIR and isdir(SDK_DIR)
assert FRAMEWORK_DIR and isdir(FRAMEWORK_DIR)
assert PULP_TOOLS_DIR and isdir(PULP_TOOLS_DIR)

board_config = env.BoardConfig()

env.Append(
    ASFLAGS=["-DLANGUAGE_ASSEMBLY"],

    CCFLAGS=[
        "-Os",
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
        "-Wno-unused-function",
        "-Wno-unused-variable",
        "-ffunction-sections",
        "-fdata-sections",
        "-mPE=8",
        "-mFC=1",
        "-fno-jump-tables",
        "-fno-tree-loop-distribute-patterns",
        "-Werror",
        "-Wundef",
        "-march=%s" % board_config.get("build.march"),
        ("-include", join(FRAMEWORK_DIR, "include", "gap_config.h"))
    ],

    CPPDEFINES=[
        "__riscv__",
        "__pulp__"
    ],

    CPPPATH=[
        join(FRAMEWORK_DIR, "include"),
        join(FRAMEWORK_DIR, "include", "io"),
        join(SDK_DIR, "tools", "pulp-debug-bridge", "include"),
        join(PULP_TOOLS_DIR, "include")
    ],

    LINKFLAGS=[
        "-nostartfiles",
        "-nostdlib",
        "-Wl,--gc-sections",
        "-T", join(SDK_DIR, "tools", "ld", "link.gap8.ld")
    ],

    LIBS=["gcc"]
)

env.Append(ASFLAGS=env.get("CCFLAGS", [])[:])

env.Replace(
    LDSCRIPT_PATH=join(SDK_DIR, "tools", "ld", "gapuino.conf.ld")
)


libs = [
    env.BuildLibrary(
        join("$BUILD_DIR", "io"),
        join(FRAMEWORK_DIR, "pulp-rt", "libs", "io")
    ),

    env.BuildLibrary(
        join("$BUILD_DIR", "rt"),
        join(FRAMEWORK_DIR, "pulp-rt"),
        src_filter="+<*> -<libs>"
    )
]

env.Append(LIBS=libs)
