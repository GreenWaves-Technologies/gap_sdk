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

import sys
import argparse

###############################################################################
# Functions
###############################################################################

def parse_sdkconfig(sdkconfig, cmakeconfig):
    for line in sdkconfig:
        if "Menu" in line and not "end of" in line:
            cmakeconfig.write("\n")
            cmakeconfig.write(line)
        if "CONFIG_" in line and not "is not set" in line: 
            option_name = line.rstrip("\n").split("=",1)[0]
            option_value = line.rstrip("\n").split("=",1)[1]
            cmakeconfig.write("set({} {})\n".format(option_name, option_value))

###############################################################################
# Parsing script args
###############################################################################

parser = argparse.ArgumentParser(description='gencmakeconfig.py')

parser.add_argument('--config',
                    help='Configuration file to process',
                    nargs=1,
                    default=None)

parser.add_argument('--cmakeconfig',
                    help='CMake compliant Kconfig configuration file',
                    nargs=1,
                    default=None)

args = parser.parse_args()

# Open files
sdkconfig = open(args.config[0], 'r')
cmakeconfig = open(args.cmakeconfig[0], 'a')

# Flush cmakeconfig file
cmakeconfig.truncate(0)
cmakeconfig.write("# This is a generated file from gencmakeconfig.py script and sdk.config file\n")

# Fill cmakeconfig file with formated options
parse_sdkconfig(sdkconfig, cmakeconfig)

sdkconfig.close()
cmakeconfig.close()