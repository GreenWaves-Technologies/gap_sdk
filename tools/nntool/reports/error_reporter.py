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
            TabularColumn("QSNR", fmt=">.0f"),
            TabularColumn("COSS", fmt=">.2f")
        ])
    else:
        header.extend([
            TabularColumn("av QSNR", fmt=">.0f"),
            TabularColumn("max QSNR", fmt=">.0f"),
            TabularColumn("min QSNR", fmt=">.0f"),
            TabularColumn("av COSS", fmt=">.2f"),
            TabularColumn("max COSS", fmt=">.2f"),
            TabularColumn("min COSS", fmt=">.2f"),
        ])

    table.add_row(header)


def do_drow(table, stat, cols):
    table.add_row([stat[k] for k in cols])


class ErrorReporter(Reporter):
    ONE_INPUT_COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',
                      'min_err', 'qsnr', 'cos']
    ONE_INPUT_WCHAN_COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',
                            'min_err', 'max_chan_err', 'qsnr', 'cos']
    COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',
            'min_err', 'qsnr', 'max_qsnr', 'min_qsnr',
            'cos', 'max_cos', 'min_cos']
    WCHAN_COLS = ['name', 'op_name', 'step', 'av_err', 'max_err',
                  'min_err', 'max_chan_err', 'qsnr', 'max_qsnr',
                  'min_qsnr', 'cos', 'max_cos', 'min_cos']

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
