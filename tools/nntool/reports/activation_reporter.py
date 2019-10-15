# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import copy
from collections import OrderedDict

from graph.nngraph import NNGraph
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
        TabularColumn("name"),
        TabularColumn("mean", fmt=">.3f"),
        TabularColumn("std dev", fmt=">.3f"),
        TabularColumn("min", fmt=">.3f"),
        TabularColumn("max", fmt=">.3f"),
        TabularColumn("min acc", fmt=">.3f"),
        TabularColumn("max acc", fmt=">.3f"),
        TabularColumn("int\nbits", fmt=">d"),
    ]

    for bit_size in STATS_BITS:
        header.append(TabularColumn("{}-bits\nformat".format(bit_size), fmt="^s"))
        header.append(TabularColumn("{}-bits\nQSNR".format(bit_size), fmt=">.0f"))

    header.append(TabularColumn("size\n(bytes)", fmt=">d"))

    table.add_row(header)

def do_row(table, node_name, stat, threshold, total):

    row = [node_name, stat['mean'], stat['std'],
           stat['min'], stat['max']]
    if 'min_acc' in stat:
        row.append(stat['min_acc'])
        row.append(stat['max_acc'])
    else:
        row.append("")
        row.append("")
    row.append(stat['ibits'])

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

def do_rows(stats, table, threshold):
    total = 0

    for node_name, stat in stats.items():
        total = do_row(table, node_name, stat, threshold, total)

    return total

def do_total(table, total):
    total_row = ["TOTAL"]
    appendn(total_row, 7 + len(STATS_BITS) * 2)
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
    def __init__(self, do_totals=True, threshold=30.0, yield_fusions=False):
        self._do_totals = do_totals
        self._threshold = threshold
        self._yield_fusions = yield_fusions

    def report(self, G: NNGraph, stats):
        dump_stats = OrderedDict()
        for _, node, fusion_idx, fnode in G.nodes_iterator(self._yield_fusions):
            stat = stats[NodeId(node, fnode)]
            stat = copy.deepcopy(stat)
            if fusion_idx:
                name = "{}_{}".format(node.name, fusion_idx)
            else:
                name = node.name
            dump_stats[name] = stat
        return dump_stats_table(dump_stats, do_totals=self._do_totals, threshold=self._threshold)
