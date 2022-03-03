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

from graph.types import ConstantInputParameters
from graph.types.activations import ActivationParameters
from graph.types.base import FilterParameters
from utils.node_id import NodeId
from utils.tabular import Tabular, TabularColumn

DEFAULT_ACC_BITS = 32


class QuantizationReporter():
    def __init__(self, step=None):
        self._step = step

    def emit_qs(self, qtypes, limit=True):
        if qtypes is None:
            return "N/A"
        if limit and len(qtypes) > 10:
            qtypes = qtypes[0:10]
            extra = " ..."
        else:
            extra = ""
        return (",".join([str(qtype) for qtype in qtypes])) + extra

    def emit_q_chan(self, qtype, chan):
        return qtype.str_by_chan(chan)

    def report(self, G, stats, nodes=None):
        if nodes is None:
            nodes = G.nodes()
        nodes = sorted(nodes, key=lambda x: x.step_idx)
        table = Tabular()
        table.add_row([
            TabularColumn("Step", fmt="^d"),
            TabularColumn("Name"),
            TabularColumn("Type"),
            TabularColumn("In"),
            TabularColumn("Out"),
            TabularColumn("Weights"),
            TabularColumn("Bias"),
            TabularColumn("Mulbias"),
            TabularColumn("Calc"),
            TabularColumn("Acc"),
        ])

        # TODO - Fix report for weights_q
        single = nodes[0].step_idx == nodes[-1].step_idx
        for key, qrec in stats.sorted_iterator(G):
            if not isinstance(key, NodeId):
                continue
            node = G.node(key.node_name)
            if node.step_idx < nodes[0].step_idx or node.step_idx > nodes[-1].step_idx:
                continue
            fnode = node.get_contained_node(
                key.fnode_name) if key.fnode_name else None
            step_idx = node.step_idx
            node = fnode or node
            if qrec:
                if (not single or not qrec.ktype == 'scaled' or not isinstance(node, FilterParameters) or
                        len(qrec.in_qs[1].scale) == 1):
                    if isinstance(node, ConstantInputParameters):
                        row = [step_idx, node.name, qrec.ktype,
                               "",
                               self.emit_qs(qrec.out_qs,
                                            limit=self._step is None),
                               "", "", "", "", ""]
                    else:
                        row = [step_idx, node.name, qrec.ktype + (" NE16" if qrec.cache.get('ne16') else ""),
                               self.emit_qs(qrec.in_qs,
                                            limit=self._step is None),
                               self.emit_qs(qrec.out_qs,
                                            limit=self._step is None)]
                        if isinstance(node, FilterParameters):
                            for i in [1, 2]:
                                row.append(self.emit_qs([qrec.in_qs[i]]))
                            for i in ["mul_biases", "calc", "acc"]:
                                key = f'{i}_q'
                                if key in qrec.cache:
                                    row.append(self.emit_qs([qrec.cache[key]]))
                                else:
                                    row.append("")
                        elif "scale_mul_biases_q" in qrec.cache:
                            row += ["", "", self.emit_qs([qrec.cache["scale_mul_biases_q"]]), "", ""]
                        else:
                            row += ["", "", "", "", ""]
                else:
                    first = True
                    for chan in range(len(qrec.in_qs[1].scale)):
                        if first:
                            row = [step_idx, node.name, qrec.ktype,
                                   self.emit_qs(qrec.in_qs,
                                                limit=self._step is None),
                                   self.emit_qs(qrec.out_qs,
                                                limit=self._step is None),
                                   self.emit_q_chan(qrec.in_qs[1], chan),
                                   self.emit_q_chan(qrec.in_qs[2], chan),
                                   self.emit_q_chan(
                                       qrec.cache['mul_biases_q'], chan),
                                   str(qrec.cache['calc_q']),
                                   str(qrec.cache['acc_q']),
                                   ]
                            first = False
                        else:
                            row = [chan, "", "", "", "",
                                   self.emit_q_chan(qrec.in_qs[1], chan),
                                   self.emit_q_chan(qrec.in_qs[2], chan),
                                   self.emit_q_chan(
                                       qrec.cache['mul_biases_q'], chan),
                                   "", ""
                                   ]
                        table.add_row(row)
                    continue
            else:
                row = [step_idx, node.name, "IEEE32",
                       "None", "None", "", "", "", "", ""]
            table.add_row(row)
        return table
