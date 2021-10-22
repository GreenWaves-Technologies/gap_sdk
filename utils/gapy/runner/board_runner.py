#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import argparse
import os

import traces
import json_tools as js
import openocd
import common


def appendArgs(parser: argparse.ArgumentParser, config: js.config) -> None:
    """
    Append specific module arguments.

    """
    
    kwargs = {}
    if config:
        kwargs['default'] = config.get_str("openocd/cable")
    
    parser.add_argument('--cable',
                        required = not config,
                        help = 'JTAG cable.',
                        **kwargs)
    
    kwargs = {}
    if config:
        kwargs['default'] = config.get_str("openocd/target")
    parser.add_argument('--chip',
                        choices = ['gap8-rev1', 'gap8-rev2'],
                        help = 'The version of the chip to connect',
                        required = not config,
                        **kwargs)


def runnerOperation(args, config = None, runnerConfig = None):
    traces.info('Board runner')
    bootTCL = os.path.join(common.openocdDir, 'tcl/jtag_boot.tcl')
    oocd = openocd.OpenOCD(cable = args.cable, chip = args.chip, scriptsTCL = [bootTCL])
    oocd.run(['gap8_jtag_load_binary_and_start %s elf' % (args.binary)])
