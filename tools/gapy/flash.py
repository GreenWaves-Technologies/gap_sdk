#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

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

import sys
import os
import argcomplete
import argparse

from errors import FatalError, NotSupportedError, InputError
import common
import json_tools as js
import traces
import openocd
import partition
import binary
import gen_partition

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
    print("Fatal error: gen_flash_image needs to be run with python version 3")
    exit(1)


def appendArgs(parser: argparse.ArgumentParser, config: js.config) -> None:
    """
    Append specific module arguments.
    """
    
    # Image
    parser.add_argument("image",
                        type = argparse.FileType('rb'),
                        help = "RAW image to flash on the device.")
    
    # JTAG cable
    kwargs = {}
    if config:
        kwargs['default'] = config.get_str("openocd/cable")
    parser.add_argument('--cable',
                        required = not config,
                        help = 'JTAG cable.',
                        **kwargs)
    
    # Chip target
    kwargs = {}
    if config:
        kwargs['default'] = config.get_str("openocd/target")
    parser.add_argument('--chip',
                        help = 'The version of the chip to connect',
                        required = not config,
                        **kwargs)


def operationFunc(args, config = None):
    traces.info('Gapy flash image tool')
    flashTCL = os.path.join(common.openocdDir, 'tcl/flash_image.tcl')
    bootTCL = os.path.join(common.openocdDir, 'tcl/jtag_boot.tcl')
    oocd = openocd.OpenOCD(cable = args.cable, chip = args.chip, scriptsTCL = [flashTCL, bootTCL])
    oocd.run(['gap_flash_raw %s %d %s' %(args.image.name, len(args.image.read()), common.openocdDir), 'exit'])


def main(custom_commandline = None, config = None):
    """Main function for flash image.
    
custom_commandline - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments
    as strings.Arguments and their values need to be added as individual items to the list e.g."-b 115200" thus
    becomes['-b', '115200'].
    """
    
    parser = argparse.ArgumentParser(
        description = 'Flash a RAW flash image files to a flash device. - v%s' % __version__,
        prog = 'flash',
        fromfile_prefix_chars = '@',
        formatter_class = argparse.ArgumentDefaultsHelpFormatter)
    
    common.appendCommonOptions(parser)
    
    if config is None:
        config = common.importConfig(parser)
    
    appendArgs(parser, config)
    
    argcomplete.autocomplete(parser)
    args = parser.parse_args(custom_commandline)
    
    try:
        operationFunc(args, config)
    finally:
        try:  # Clean up AddrFilenamePairAction files
            for address, argfile in args.addr_filename:
                argfile.close()
        except AttributeError:
            pass


if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print('\nA fatal error occurred: %s' % e)
        sys.exit(2)
