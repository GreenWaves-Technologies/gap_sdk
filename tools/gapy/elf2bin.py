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

import partition
import binary
import gen_partition
import traces

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
    print("Fatal error: gen_flash_image needs to be run with python version 3")
    exit(1)


def appendArgs(parser: argparse.ArgumentParser) -> None:
    """
    Append specific module arguments.
    """
    
    # Inputs ELF
    parser.add_argument("elf",
                        nargs = '+',
                        type = argparse.FileType('rb'),
                        help = "ELF programs to convert.")


def operationFunc(args, config=None):

    traces.info('Gapy ELF2Bin converter.')
    
    for elf in args.elf:
        traces.info("Convertting %s:" % elf.name)
        binApp = binary.App(elf = elf)
        baseName = os.path.basename(elf.name)
        if baseName[-4:] == '.elf':
            baseName = baseName[:-4]
        outName = os.path.join(os.path.dirname(elf.name), baseName + ".bin")
        with open(outName, 'wb') as outFile:
            outFile.write(binApp.dump())


def main(custom_commandline = None):
    """Main function for elf2bin.
    
custom_commandline - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments
    as strings.Arguments and their values need to be added as individual items to the list e.g."-b 115200" thus
    becomes['-b', '115200'].
    """
    
    parser = argparse.ArgumentParser(
        description = 'Convert GAP ELF App to a binary used by SSBL to load app. - v%s' % __version__,
        prog = 'elf2bin',
        fromfile_prefix_chars = '@',
        formatter_class = argparse.ArgumentDefaultsHelpFormatter)
    
    common.appendCommonOptions(parser)
    
    appendArgs(parser)
    
    argcomplete.autocomplete(parser)
    args = parser.parse_args(custom_commandline)
    
    try:
        operationFunc(args)
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
