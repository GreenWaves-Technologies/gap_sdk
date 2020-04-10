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

from utils.tabular import Tabular, TabularColumn

from .reporter import Reporter

class TempsReporter(Reporter):
    def __init__(self, do_totals=True):
        self._do_totals = do_totals

    def report(self, G, stats):
        table = Tabular()
        header = [
            TabularColumn("name"),
            TabularColumn("creating node"),
            TabularColumn("type"),
            TabularColumn("dimensions", fmt="^"),
            TabularColumn("size", fmt=">d"),
            TabularColumn("count", fmt=">d"),
        ]
        is_quantized = stats and 'qtype' in stats[0]
        if is_quantized:
            header.append(TabularColumn("Q"))

        table.add_row(header)
        total = 0
        for stat in stats:
            row = [str(stat[k]) for k in ['name', 'creating_node', 'type']]
            row.append(str(stat['dims']))
            row.append(stat['dims'].size())
            row.append(stat['count'])
            if is_quantized:
                row.append(str(stat['qtype']))

            table.add_row(row)
            total += stat['dims'].size()
        if self._do_totals:
            row = ["Total", "", "", "", total, ""]
            if is_quantized:
                row.append("")
            table.add_row(row)
        return table
