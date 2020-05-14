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

import copy
from collections import OrderedDict

from graph.nngraph import NNGraph
from graph.types import FilterParameters, ConvFusionParameters
from utils.stats_funcs import STATS_BITS, astats, calculate_qsnrs
from utils.tabular import Tabular, TabularColumn
from utils.node_id import NodeId

from .reporter import Reporter


def gather_stats(activation, force_ideal=False):
    stat = astats(activation)
    stat['qstats'] = calculate_qsnrs(activation, stat['ibits'], force_ideal)
    return stat

def appendn(row, rep, val=""):
    for _ in range(rep):
        row.append(val)

def do_header(table):
    header = [
        TabularColumn("step"),
        TabularColumn("name"),
        TabularColumn("mean", fmt=">.3f"),
        TabularColumn("std dev", fmt=">.3f"),
        TabularColumn("min", fmt=">.3f"),
        TabularColumn("max", fmt=">.3f"),
        TabularColumn("min acc", fmt=">.3f"),
        TabularColumn("max acc", fmt=">.3f"),
        TabularColumn("weak\nOLs", fmt=">d"),
        TabularColumn("strong\nOLs", fmt=">d"),
        TabularColumn("min\nOL", fmt=">.3f"),
        TabularColumn("max\nOL", fmt=">.3f"),
        TabularColumn("int\nbits", fmt=">d"),
        TabularColumn("avg\nprec", fmt=">.3f"),
    ]

    for bit_size in STATS_BITS:
        header.append(TabularColumn("{}-bits\nformat".format(bit_size), fmt="^s"))
        header.append(TabularColumn("{}-bits\nQSNR".format(bit_size), fmt=">.0f"))

    header.append(TabularColumn("size\n(bytes)", fmt=">d"))

    table.add_row(header)

def do_row(table, step_idx, node_name, stat, threshold, total):

    row = [step_idx, node_name, stat['mean'], stat['std'],
           stat['min'], stat['max']]
    if 'min_acc' in stat:
        row.append(stat['min_acc'])
        row.append(stat['max_acc'])
    else:
        row.append("")
        row.append("")
    row.extend([stat['wols'], stat['sols'], stat['min_out'],
                stat['max_out']])
    row.append(stat['ibits'])
    row.append(stat['avg_prec'] if 'avg_prec' in stat else None)

    if 'qstats' not in stat:
        appendn(row, len(STATS_BITS) * 2 + 1)
        table.add_row(row)
        return total

    qstats = stat['qstats']
    sel = None
    for bit_size in STATS_BITS:
        if bit_size in qstats:
            qstat = qstats[bit_size]
            if (sel is None or sel > bit_size) and qstat['qsnr'] > threshold:
                sel = bit_size
            row.append("Q{}".format(qstat['q']))
            row.append(qstat['qsnr'])
        else:
            appendn(row, 2)
    if sel is not None:
        size = stat['size']*sel//8
        total += size
        row.append(size)
    else:
        row.append("")

    table.add_row(row)
    return total

def in_and_true(elem, *vals):
    return all(val in elem and elem[val] for val in vals)

def do_rows(stats, table, threshold):
    total = 0

    for node_name, stat in stats.items():
        if in_and_true(stat, 'channel_stats'):
            for channel_idx, channel_stat in enumerate(stat['channel_stats']):
                do_row(table, stat['idx'], "channel {}".format(channel_idx), channel_stat, threshold, 0)
 
        total = do_row(table, stat['idx'], node_name, stat, threshold, total)

    return total

def do_total(table, total):
    total_row = ["TOTAL"]
    appendn(total_row, 13 + len(STATS_BITS) * 2)
    total_row.append(total)
    table.add_row(total_row)

def dump_stats_table(stats, do_totals=True, threshold=30):
    table = Tabular()
    do_header(table)
    total = do_rows(stats, table, threshold)
    if do_totals:
        do_total(table, total)
    return table

class ActivationReporter(Reporter):
    def __init__(self, do_totals=True, threshold=30.0, yield_fusions=False, step_idx=None):
        self._do_totals = do_totals
        self._threshold = threshold
        self._yield_fusions = yield_fusions
        self._step_idx = step_idx

    def report(self, G: NNGraph, stats):
        dump_stats = OrderedDict()
        for step_idx, node, fusion_idx, fnode in G.nodes_iterator(self._yield_fusions):
            if self._step_idx is not None and self._step_idx != step_idx:
                continue
            nid = NodeId(node, fnode)
            if nid not in stats:
                continue
            stat = stats[nid]
            stat = copy.deepcopy(stat)
            if fusion_idx:
                name = "{}_{}".format(node.name, fusion_idx)
                idx = "{}_{}".format(step_idx, fusion_idx)
            else:
                name = node.name
                idx = str(step_idx)
            dump_stats[name] = stat
            stat['idx'] = idx
        return dump_stats_table(dump_stats, do_totals=self._do_totals, threshold=self._threshold)
