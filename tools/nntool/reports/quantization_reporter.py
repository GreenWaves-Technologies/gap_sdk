# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from utils.tabular import Tabular, TabularColumn

from graph.types import FilterParameters
from .reporter import Reporter

def emit_q(qtype):
    return "Q{}.{}".format(qtype.bits - qtype.q, qtype.q)

def emit_qs(qtypes):
    return ",".join([emit_q(qtype) for qtype in qtypes])

class QuantizationReporter(Reporter):
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
            node = G.node(key.node_name)
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
