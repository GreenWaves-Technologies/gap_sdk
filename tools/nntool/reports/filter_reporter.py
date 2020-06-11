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
            TabularColumn("step", fmt=">d"),
            TabularColumn("name"),
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
    def __init__(self, do_totals=True, threshold=30.0, step_idx=None):
        self._do_totals = do_totals
        self._threshold = threshold
        self._step_idx = step_idx

    def do_header(self, table):
        header = [
            TabularColumn("step"),
            TabularColumn("name") if self._step_idx is None else TabularColumn('channel'),
            TabularColumn("mean", fmt=">.3f"),
            TabularColumn("std dev", fmt=">.3f"),
            TabularColumn("min", fmt=">.3f"),
            TabularColumn("max", fmt=">.3f"),
            TabularColumn("weak\nOLs", fmt=">d"),
            TabularColumn("strong\nOLs", fmt=">d"),
            TabularColumn("min\nOL", fmt=">.3f"),
            TabularColumn("max\nOL", fmt=">.3f"),
            TabularColumn("int\nbits", fmt=">d"),
            TabularColumn("avg\nprec", fmt=">.3f"),
        ]

        for bit_size in STATS_BITS:
            header.append(TabularColumn("{}-bits\nformat".format(bit_size), fmt="^s"))
            header.append(TabularColumn("{}-bits\nQSNR".format(bit_size), fmt=">.1f"))

        header.append(TabularColumn("size\n(bytes)", fmt=">d"))

        table.add_row(header)

    def do_rows(self, stats, table):
        total = 0

        for key, stat in stats.items():
            if self._step_idx:
                if self._step_idx != stat['step_idx']:
                    continue
                total = self.do_row(table, str(stat['step_idx']),
                                    str(key), stat, total, by_channel=True)
                break
            # stats will be a list if coming from a fusion
            total = self.do_row(table, str(stat['step_idx']), str(key), stat, total)

        return total

    def do_row_item(self, table, step_idx, node_name, var, total, op_name, vartype):
        row = [step_idx, "{}{}_{}".format(node_name, op_name, vartype),
               var['mean'], var['std'], var['min'], var['max'],
               var['wols'], var['sols'], var['min_out'],
               var['max_out'], var['ibits'], var['avg_prec'] if 'avg_prec' in var else None]
        if 'qstats' not in var:
            appendn(row, 7)
            table.add_row(row)
            return total

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

    def do_row(self, table, step_idx, node_name, stat, total, op_name="", by_channel=False):
        for vartype in ['weights', 'biases', 'mul_biases']:
            if vartype not in stat:
                continue
            var = stat[vartype]
            if by_channel:
                if 'channel_stats' in var:
                    for channel_idx, channel_stat in enumerate(var['channel_stats']):
                        total += self.do_row_item(table, step_idx, str(channel_idx),
                                                  channel_stat, total, op_name, vartype)
                else:
                    total += self.do_row_item(table, step_idx, "all", var, total, op_name, vartype)
            else:
                total += self.do_row_item(table, step_idx, node_name, var, total, op_name, vartype)
        return total

    @staticmethod
    def do_total(table, total):
        total_row = ["TOTAL"]
        appendn(total_row, 11 + len(STATS_BITS) * 2)
        total_row.append(total)
        table.add_row(total_row)

    def report(self, G, stats):
        table = Tabular()
        self.do_header(table)
        total = self.do_rows(stats, table)
        if self._do_totals:
            self.do_total(table, total)
        return table
