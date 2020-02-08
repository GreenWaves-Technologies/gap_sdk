# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from utils.stats_funcs import STATS_BITS
from utils.tabular import Tabular, TabularColumn

from .reporter import Reporter

def appendn(row, rep, val=""):
    for _ in range(rep):
        row.append(val)

class FilterDetailedStatsReporter(Reporter):
    def __init__(self, do_totals=True, threshold=30.0):
        self._do_totals = do_totals
        self._threshold = threshold

    @staticmethod
    def do_dheader(table):
        header = [
            TabularColumn("name"),
            TabularColumn("step", fmt=">d"),
            TabularColumn("min I range", fmt=">.3f"),
            TabularColumn("max I range", fmt=">.3f"),
            TabularColumn("min O range", fmt=">.3f"),
            TabularColumn("max O range", fmt=">.3f"),
            TabularColumn("min I prec", fmt=">.3f"),
            TabularColumn("avg I prec", fmt=">.3f"),
            TabularColumn("min O prec", fmt=">.3f"),
            TabularColumn("avg O prec", fmt=">.3f"),
        ]
        table.add_row(header)

    @staticmethod
    def do_drow(table, stats):
        table.add_row(stats)

    def report(self, G, stats):
        table = Tabular()
        self.do_dheader(table)
        for stat in stats.values():
            self.do_drow(table, stat)
        return table

class FilterStatsReporter(Reporter):
    def __init__(self, do_totals=True, threshold=30.0):
        self._do_totals = do_totals
        self._threshold = threshold

    @staticmethod
    def do_header(table):
        header = [
            TabularColumn("name"),
            TabularColumn("mean", fmt=">.3f"),
            TabularColumn("std dev", fmt=">.3f"),
            TabularColumn("min", fmt=">.3f"),
            TabularColumn("max", fmt=">.3f"),
            TabularColumn("weak\nOLs", fmt=">d"),
            TabularColumn("strong\nOLs", fmt=">d"),
            TabularColumn("min\nOL", fmt=">.3f"),
            TabularColumn("max\nOL", fmt=">.3f"),
            TabularColumn("int\nbits", fmt=">d"),
        ]

        for bit_size in STATS_BITS:
            header.append(TabularColumn("{}-bits\nformat".format(bit_size), fmt="^s"))
            header.append(TabularColumn("{}-bits\nQSNR".format(bit_size), fmt=">.1f"))

        header.append(TabularColumn("size\n(bytes)", fmt=">d"))

        table.add_row(header)

    def do_rows(self, stats, table):
        total = 0

        for key, stat in stats.items():
            # stats will be a list if coming from a fusion
            total = self.do_row(table, str(key), stat, total)

        return total

    def do_row(self, table, node_name, stat, total, op_name=""):
        for vartype in ['weights', 'biases']:
            if vartype not in stat:
                continue
            var = stat[vartype]
            row = ["{}{}_{}".format(node_name, op_name, vartype),\
                var['mean'], var['std'], var['min'], var['max'],\
                var['wols'], var['sols'], var['min_out'],\
                var['max_out'], var['ibits']]
            if 'qstats' not in var:
                appendn(row, 5)
                table.add_row(row)
                continue

            qstats = var['qstats']
            sel = None
            for bit_size in STATS_BITS:
                if bit_size in qstats:
                    qstat = qstats[bit_size]
                    if (sel is None or sel > bit_size) and qstat['qsnr'] > self._threshold:
                        sel = bit_size
                    row.append("Q{}".format(qstat['q']))
                    row.append(qstat['qsnr'])
                else:
                    appendn(row, 2)
            if sel is not None:
                size = var['size']*sel//8
                total += size
                row.append(size)
            else:
                row.append("")

            table.add_row(row)
        return total

    @staticmethod
    def do_total(table, total):
        total_row = ["TOTAL"]
        appendn(total_row, 9 + len(STATS_BITS) * 2)
        total_row.append(total)
        table.add_row(total_row)

    def report(self, G, stats):
        table = Tabular()
        self.do_header(table)
        total = self.do_rows(stats, table)
        if self._do_totals:
            self.do_total(table, total)
        return table
