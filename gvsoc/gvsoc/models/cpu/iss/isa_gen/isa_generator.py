#!/usr/bin/env python3

#
# Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
#                    University of Bologna
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

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#


import argparse
import os.path
import importlib


parser = argparse.ArgumentParser(description='Generate ISA for RISCV')

parser.add_argument("--header-file", dest="header_file", default=None, metavar="PATH", help="Specify header output file")
parser.add_argument("--source-file", dest="source_file", default=None, metavar="PATH", help="Specify source output file")
parser.add_argument("--implem", dest="implem", default=None, help="Specify implementation name")
parser.add_argument("--core", dest="core", default=None, help="Specify core class and module")

args = parser.parse_args()

core = args.core

if core is None:
    if args.implem is None:
        core = "Riscy@isa_pulp_gen"
    elif args.implem == 'zeroriscy':
        core = "Zeroriscy@isa_pulp_gen"

print (core)

class_name, module_name = core.split('@')
module = importlib.import_module(module_name)
core = getattr(module, class_name, None)()

core.gen(args.header_file, args.source_file)
