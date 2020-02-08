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

from typing import Sequence

from graph.nngraph import NNGraph
from utils.tabular import Tabular, TabularColumn

from .reporter import Reporter


def do_dheader(table, one_input, with_chan):
    header = [
        TabularColumn("name"),
        TabularColumn("op type"),
        TabularColumn("step", fmt=">"),
        TabularColumn("avg error", fmt=">.3f"),
        TabularColumn("max error", fmt=">.3f"),
        TabularColumn("min error", fmt=">.3f")
    ]
    if with_chan:
        header.append(TabularColumn("max ch error", fmt=">.3f"))
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

def do_drow(table, stat, cols):
    table.add_row([stat[k] for k in cols])

class ErrorReporter(Reporter):
    ONE_INPUT_COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',\
            'min_err', 'qsnr', 'overflow_dot', 'overflow_acc']
    ONE_INPUT_WCHAN_COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',\
            'min_err', 'max_chan_err', 'qsnr', 'overflow_dot', 'overflow_acc']
    COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',\
            'min_err', 'qsnr', 'max_qsnr', 'min_qsnr', 'overflow_dot', 'overflow_acc']
    WCHAN_COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',\
            'min_err', 'max_chan_err', 'qsnr', 'max_qsnr', 'min_qsnr',\
            'overflow_dot', 'overflow_acc']

    def __init__(self, do_totals=True, threshold=30.0, one_input=False, with_chan=False):
        print('with chan', with_chan, one_input)
        self._do_totals = do_totals
        self._threshold = threshold
        self._one_input = one_input
        self._with_chan = with_chan

        if one_input:
            if with_chan:
                self._cols = self.ONE_INPUT_WCHAN_COLS
            else:
                self._cols = self.ONE_INPUT_COLS
        else:
            if with_chan:
                self._cols = self.WCHAN_COLS
            else:
                self._cols = self.COLS

    def report(self, G: NNGraph, stats: Sequence) -> Tabular:
        table = Tabular()
        do_dheader(table, self._one_input, self._with_chan)
        for stat in stats.values():
            do_drow(table, stat, self._cols)
        return table
