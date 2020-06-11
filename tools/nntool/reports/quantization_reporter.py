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

from quantization.symmetric.symmetric_quantization import \
    SymmetricScalableFilterQuantizationRecord
from quantization.multiplicative.mult_quantization import MultScalableFilterQuantizationRecord
from quantization.multiplicative.mult_qtype_base import WrapperMixin
from graph.types import ConstantInputParameters
from utils.node_id import NodeId
from utils.tabular import Tabular, TabularColumn

from .reporter import Reporter

DEFAULT_ACC_BITS = 32


class QuantizationReporter(Reporter):
    def __init__(self, step=None, emit_wrapped=True):
        super(QuantizationReporter).__init__()
        self._step = step
        self._emit_wrapped = emit_wrapped

    def emit_qs(self, qtypes, limit=True):
        if limit and len(qtypes) > 10:
            qtypes = qtypes[0:10]
            extra = " ..."
        else:
            extra = ""
        if self._emit_wrapped:
            return (",".join([str(qtype.wrapped) if isinstance(qtype, WrapperMixin)
                              else str(qtype) for qtype in qtypes])) + extra
        return (",".join([str(qtype) for qtype in qtypes])) + extra

    def emit_q_chan(self, qtype, chan):
        if self._emit_wrapped:
            return (qtype.wrapped.str_by_chan(chan) if isinstance(qtype, WrapperMixin)
                    else qtype.str_by_chan(chan))
        return qtype.str_by_chan(chan)

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

        for key, qrec in stats.sorted_iterator(G):
            if not isinstance(key, NodeId):
                continue
            node = G.node(key.node_name)
            if self._step is not None and self._step != node.step_idx:
                continue
            fnode = node.get_contained_node(key.fnode_name) if key.fnode_name else None
            step_idx = node.step_idx
            node = fnode or node
            if qrec:
                if self._step is None or not isinstance(qrec, MultScalableFilterQuantizationRecord) or len(qrec.weights_q.scale) == 1:
                    if isinstance(node, ConstantInputParameters):
                        row = [step_idx, node.name,
                               "",
                               self.emit_qs(qrec.out_qs,
                                            limit=self._step is None),
                               "", "", "", "", ""]
                    else:
                        row = [step_idx, node.name,
                               self.emit_qs(qrec.in_qs,
                                            limit=self._step is None),
                               self.emit_qs(qrec.out_qs,
                                            limit=self._step is None)]
                        if isinstance(qrec, (SymmetricScalableFilterQuantizationRecord, MultScalableFilterQuantizationRecord)):
                            for i in ["weights", "biases", "mul_biases", "calc", "acc"]:
                                row.append(self.emit_qs([getattr(qrec, i+'_q')]))
                        else:
                            row += ["", "", "", "", ""]
                else:
                    first = True
                    for chan in range(len(qrec.weights_q.scale)):
                        if first:
                            row = [step_idx, node.name,
                                   self.emit_qs(qrec.in_qs,
                                                limit=self._step is None),
                                   self.emit_qs(qrec.out_qs,
                                                limit=self._step is None),
                                   self.emit_q_chan(qrec.weights_q, chan),
                                   self.emit_q_chan(qrec.biases_q, chan),
                                   self.emit_q_chan(qrec.mul_biases_q, chan),
                                   str(qrec.calc_q),
                                   str(qrec.acc_q),
                                   ]
                            first = False
                        else:
                            row = [chan, "", "", "",
                                   self.emit_q_chan(qrec.weights_q, chan),
                                   self.emit_q_chan(qrec.biases_q, chan),
                                   self.emit_q_chan(qrec.mul_biases_q, chan),
                                   "", ""
                                   ]
                        table.add_row(row)
                    continue
            else:
                row = [step_idx, node.name, "None", "None", "", "", "", "", ""]
            table.add_row(row)
        return table
