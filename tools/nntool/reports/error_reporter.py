# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from typing import Sequence

from graph.nngraph import NNGraph
from utils.tabular import Tabular, TabularColumn

from .reporter import Reporter

def do_dheader(table, one_input):
    header = [
        TabularColumn("name"),
        TabularColumn("op type"),
        TabularColumn("step", fmt=">d"),
        TabularColumn("avg error", fmt=">.3f"),
        TabularColumn("max error", fmt=">.3f"),
        TabularColumn("min error", fmt=">.3f")
    ]
    if one_input:
        header.extend([
            TabularColumn("QSNR", fmt=">.0f")
        ])
    else:
        header.extend([
            TabularColumn("av QSNR", fmt=">.0f"),
            TabularColumn("max QSNR", fmt=">.0f"),
            TabularColumn("min QSNR", fmt=">.0f"),
        ])

    header.extend([
        TabularColumn("OverF dot", fmt=">d"),
        TabularColumn("OverF acc", fmt=">d"),
    ])
    table.add_row(header)

def do_drow(table, stat, one_input):
    if one_input:
        table.add_row([stat[k] for k in ['name', 'op_name', 'step', 'av_err', 'max_err',\
            'min_err', 'qsnr', 'overflow_dot', 'overflow_acc']])
    else:
        table.add_row([stat[k] for k in ['name', 'op_name', 'step', 'av_err', 'max_err',\
            'min_err', 'qsnr', 'max_qsnr', 'min_qsnr', 'overflow_dot', 'overflow_acc']])

class ErrorReporter(Reporter):
    def __init__(self, do_totals=True, threshold=30.0, one_input=False):
        self._do_totals = do_totals
        self._threshold = threshold
        self._one_input = one_input

    def report(self, G: NNGraph, stats: Sequence) -> Tabular:
        table = Tabular()
        do_dheader(table, self._one_input)
        for stat in stats.values():
            do_drow(table, stat, self._one_input)
        return table
