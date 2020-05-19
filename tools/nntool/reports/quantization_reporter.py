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

from graph.types import FilterParameters
from utils.node_id import NodeId
from quantization.qtype import QType
from quantization.quantization_record import FilterQuantizationRecord
from importer.tflite.tflite_qtype import TfliteQType

from .reporter import Reporter

DEFAULT_ACC_BITS = 32

def emit_q(qtype):
    if qtype is None:
        return ""
    if isinstance(qtype, TfliteQType):
        return ', '.join(map(str, ["{} = {}".format(x, y) for x,y \
                in zip(qtype.report_columns(), qtype.to_report())]))
    return "Q{}.{}".format(qtype.bits - qtype.q, qtype.q)

def emit_qs(qtypes):
    return ",".join([emit_q(qtype) for qtype in qtypes])

class QuantizationReporter(Reporter):
    def __init__(self, step=None):
        super(QuantizationReporter).__init__()
        self._step = step

    def report(self, G, stats):
        table = Tabular()
        table.add_row([
            TabularColumn("Step", fmt="^d"),
            TabularColumn("Name"),
            TabularColumn("In"),
            TabularColumn("Out"),
            TabularColumn("Weights"),
            TabularColumn("Bias"),
            TabularColumn("Mulbias"),
            TabularColumn("Calc"),
            TabularColumn("Acc"),
        ])

        for key, qrec in stats.items():
            if not isinstance(key, NodeId):
                continue
            node = G.node(key.node_name)
            if self._step is not None and self._step != node.step_idx:
                continue
            fnode = node.get_contained_node(key.fnode_name) if key.fnode_name else None
            step_idx = node.step_idx
            node = fnode or node
            row = [step_idx, node.name, emit_qs(qrec.in_qs), emit_qs(qrec.out_qs)]
            if isinstance(qrec, FilterQuantizationRecord):
                for i in ["weights", "biases", "mul_biases", "calc", "acc"]:
                    row.append(emit_q(getattr(qrec, i+'_q')))
            else:
                row += ["", "", "", "", ""]
            table.add_row(row)
        return table
