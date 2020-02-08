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

from graph.types import FilterParameters
from utils.node_id import NodeId

from .reporter import Reporter

def emit_q(qtype):
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
            if isinstance(node, FilterParameters):
                for i in ["weights", "biases", "calc", "acc"]:
                    row.append(emit_q(getattr(qrec, i+'_q')))
            else:
                row += ["", "", "", ""]
            table.add_row(row)
        return table
