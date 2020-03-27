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
import importlib

from errors import FatalError, NotSupportedError, InputError
import common
import json_tools as js

import partition
import binary
import gen_partition
import traces

gapyDir = os.path.dirname(os.path.realpath(__file__))
targetsDir = os.path.join(gapyDir, 'targets')
configsDir = os.path.join(gapyDir, 'configs')
gapyJsonPath = [targetsDir, configsDir]

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
    print("Fatal error: gen_flash_image needs to be run with python version 3")
    exit(1)


def get_platforms():
    platform_list = {}
    for path in js.get_paths(paths=common.gapyJsonPath):
        config_path = os.path.join(path, 'platforms')
        if os.path.exists(config_path):
            files = os.listdir(config_path)
            for platform_file in files:
                name = platform_file[:platform_file.index('.json')]
                if platform_list.get(name) is None:
                    platform_list[name] = os.path.join(config_path, platform_file)

    return platform_list


def import_runner(name):
    module = importlib.import_module(name)
    if module is None:
        raise InputError('Invalid runner module:' + module)

    return module


def appendPlatform(subparsers, config):

    platforms = get_platforms()

    for plt_name, plt_config_path in get_platforms().items():
        plt_config = js.import_config_from_file(plt_config_path, paths = gapyJsonPath, find=True, interpret=True)
        runner_module = plt_config.get_str('runner_module')

        if runner_module is not None:
            runner = import_runner(runner_module)

            parser = subparsers.add_parser(
                plt_name,
                help = plt_config.get_str('description'))
            runner.appendArgs(parser, config)


def appendArgs(parser: argparse.ArgumentParser, config: js.config) -> None:
    """
    Append specific module arguments.
    """
    
    #
    # Plateforms
    #
    platformParsers = parser.add_subparsers(
        dest = 'platform',
        help = 'Gives the name of the platform where the target is running. Run run {platform} -h for additional help')
    appendPlatform(platformParsers, config)
    
    #
    # Commons args
    #
    
    # Binary
    parser.add_argument("--binary", dest = "binary",
                        default = None,
                        help = "Binary to execute on the target")
    
    # Flash image
    parser.add_argument("--flash-image", dest = "flash_image",
                        default = None,
                        help = "Flash image containing the binary to execute on the target")

    parser.add_argument("--all", dest = "all",
                        action = "store_true",
                        help = "Do all the steps required to run the binary on the target")

    parser.add_argument("--image", dest = "image",
                        action = "store_true",
                        help = "Build the inages required to run on the target (e.g. flash images)")

    parser.add_argument("--flash", dest = "flash",
                        action = "store_true",
                        help = "Upload all images on the target (e.g. write flash image to the flash)")

    parser.add_argument("--exec", dest = "exec",
                        action = "store_true",
                        help = "Launch execution on the target")


def operationFunc(args, config = None):

    #if args.binary is None and args.flash_image is None:
    #    raise errors.InputError(
    #        'Either the binary to execute on the target or the flash image containing the binary must be specified')

    #if args.target is None:
    #    raise InputError('The target must be specified')

    if args.binary is not None:
        config.set('runner/boot-loader', args.binary)

    module_name = config.get_str(args.platform + '/runner_module')
    if module_name is None:
        raise InputError('Invalid platform: ' + args.platform)

    module = importlib.import_module(module_name)
    if module is None:
        raise InputError('Invalid runner module:' + module)

    runner = module.Runner(args, config)

    status = runner.run()

    if status != 0:
        raise RuntimeError('Runner has failed with value: %d' % status)




def main(custom_commandline = None, config = None):
    """
    Main function for run platform.
    
    custom_commandline - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments
    as strings.Arguments and their values need to be added as individual items to the list e.g."-b 115200" thus
    becomes['-b', '115200'].
    """
    
    parser = argparse.ArgumentParser(
        description = 'Run a specific platform - v%s' % __version__,
        prog = 'run',
        fromfile_prefix_chars = '@',
        formatter_class = argparse.ArgumentDefaultsHelpFormatter)
    
    common.appendCommonOptions(parser)
    
    if config is None:
        config = common.importConfig(parser)
    
    flashConfig = config.get(config.get_str("runner/boot/device"))
    appendArgs(parser, flashConfig)
    
    argcomplete.autocomplete(parser)
    args = parser.parse_args(custom_commandline)
    
    try:
        operationFunc(args, config, )
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
