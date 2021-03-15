# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import distutils.ccompiler as compiler
import logging
import os
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
import re
from prettytable import PrettyTable

L1_MEM_USAGE = re.compile(r'\s*(?P<column>Shared L1 Memory size) \(Bytes\)\s+\: Given\:\s+(?P<given>[0-9]+)\,\sUsed\:\s+(?P<used>[0-9]+)')
L2_MEM_USAGE = re.compile(r'\s*(?P<column>L2 Memory size) \(Bytes\)\s+\: Given\:\s+(?P<given>[0-9]+)\,\sUsed\:\s+(?P<used>[0-9]+)')
L3_MEM_USAGE = re.compile(r'\s*(?P<column>L3 Memory size) \(Bytes\)\s+\: Given\:\s+(?P<given>[0-9]+)\,\sUsed\:\s+(?P<used>[0-9]+)')
L3_MEM_BW = re.compile(r'(?P<column>L3 Memory bandwidth for 1 graph run)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')
L2_MEM_BW = re.compile(r'(?P<column>L2 Memory bandwidth for 1 graph run)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')
KER_ARGS = re.compile(r'(?P<column>Sum of all Kernels arguments size)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')
TIL_OH = re.compile(r'(?P<column>Tiling Bandwith overhead)\s+\:\s+(?P<value>[0-9.]+)\s+Move/KerArgSize\n')
L2_MEM_BW_PER = re.compile(r'(?P<column>Percentage of baseline BW for L2)\s+\:\s+(?P<value>[0-9.]+)\s+\%\n')
L3_MEM_BW_PER = re.compile(r'(?P<column>Percentage of baseline BW for L3)\s+\:\s+(?P<value>[0-9.]+)\s+\%\n')
KER_OPS = re.compile(r'(?P<column>Sum of all Kernels operations)\s+\:\s+(?P<value>[0-9]+)\s+Operations\n')
TOT_COEFFS = re.compile(r'(?P<column>Total amount of flash coefficients)\s+\:\s+(?P<value>[0-9]+)\s+Bytes\n')

MATCHES = [L1_MEM_USAGE, L2_MEM_USAGE, L3_MEM_USAGE, L3_MEM_BW, L2_MEM_BW, TIL_OH, KER_ARGS, L2_MEM_BW_PER, L3_MEM_BW_PER, KER_OPS, TOT_COEFFS]

LOG = logging.getLogger("nntool")

class RunATCommand(NNToolShellBase):
    # GEN COMMAND
    parser_compile = Cmd2ArgumentParser()
    parser_compile.add_argument('executable', completer_method=Cmd.path_complete,
                                help='path to the executable')
    parser_compile.add_argument('out_dir',
                                completer_method=Cmd.path_complete, default=None,
                                help='Path to the output dir')
    parser_compile.add_argument('--flash_dir', '-f', type=str, default=None,
                                help='Flash dir')
    parser_compile.add_argument('--l1_memory', type=int, default=51.2,
                                help="L1 Memory constraint (defualt: 51.2 [kB])")
    parser_compile.add_argument('--l2_memory', type=int, default=350,
                                help="L2 Memory constraint (defualt: 350 [kB])")
    parser_compile.add_argument('--l3_memory', type=int, default=8*1024,
                                help="L3 Memory constraint (defualt: 8*1024 [kB])")

    parser_compile.add_argument('--l1_memory_range', type=int, nargs=3,
                                help="Range of L1 Memory constraints (format: start, end, step)")
    parser_compile.add_argument('--l2_memory_range', type=int, nargs=3,
                                help="Range of L2 Memory constraints (format: start, end, step)")
    parser_compile.add_argument('--l3_memory_range', type=int, nargs=3,
                                help="Range of L3 Memory constraints (format: start, end, step)")
    parser_compile.add_argument('--verbose', action='store_true',
                                help='Print out the whole Autotiler log')

    @with_argparser(parser_compile)
    def do_run_at(self, args):
        extra_args = []
        if args.flash_dir:
            extra_args.append("-f {}".format(args.flash_dir))
        if args.l1_memory_range:
            l1_memories = [i for i in range(args.l1_memory_range[0], args.l1_memory_range[1], args.l1_memory_range[2])]
        else:
            l1_memories = [args.l1_memory]
        if args.l2_memory_range:
            l2_memories = [i for i in range(args.l2_memory_range[0], args.l2_memory_range[1], args.l2_memory_range[2])]
        else:
            l2_memories = [args.l2_memory]
        if args.l3_memory_range:
            l3_memories = [i for i in range(args.l3_memory_range[0], args.l3_memory_range[1], args.l3_memory_range[2])]
        else:
            l3_memories = [args.l3_memory]
        LOG.info("Memory settings: {} {} {}".format(l1_memories, l2_memories, l3_memories))

        fields = None
        rows = []
        for l1 in l1_memories:
            for l2 in l2_memories:
                for l3 in l3_memories:
                    cmd = "./{} -o {} -c {} --L1 {} --L2 {} --L3 {} {}".format(args.executable,
                                                                               args.out_dir, args.out_dir,
                                                                               l1*1024, l2*1024, l3*1024, " ".join(extra_args))
                    LOG.info(cmd)
                    stream = os.popen(cmd)

                    out_log = stream.readlines()
                    fields = []
                    row = []
                    for line in out_log:
                        if args.verbose:
                            print(line, end='')
                        for match in MATCHES:
                            m = match.search(line)
                            if m:
                                if "Memory size" in m['column']:
                                    fields.append(m['column'] + ' Given')
                                    row.append(float(m['given']))
                                    fields.append(m['column'] + ' Used')
                                    row.append(float(m['used']))
                                else:
                                    fields.append(m['column'])
                                    row.append(float(m['value']))
                                continue
                    rows.append(row)

        if fields:
            x = PrettyTable()
            x.field_names = fields
            for row in rows:
                x.add_row(row)
            print(x)
