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
