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

import logging
from copy import deepcopy

from graph.types import Conv2DParameters
from quantization.qtype import QType
from utils.node_id import NodeId

from ..adjust_base import AdjusterBase, handles

LOG = logging.getLogger("nntool." + __name__)

@handles(Conv2DParameters)
class ConvAdjuster(AdjusterBase):
    def adjust(self, G, node):
        modified = False
        # check that the transposed input 0 matches autotiler order
        names = node.in_dims[0].order
        if node.transpose_in is not None and node.transpose_in[0] is not None:
            names = self.trans_names(names, node.transpose_in[0])
        if names != node.ker_in_order[0]:
            self.adjust_in_out_order(G, node, names, order=node.ker_in_order[0])
            modified = True

        # if it doesn't then insert transpose before and after
        # check that the filter and bias match autotiler order
        if node.filter.order != node.ker_in_order[1]:
            LOG.debug("step %s: %s adjust weights %s => %s",
                      node.step_idx, node.name, node.filter, " x ".join(node.ker_in_order[1]))
            trans = self.get_trans(node.filter.order, node.ker_in_order[1])
            self.apply_input_trans(node, trans, index=1)
            node.filter.impose_order(node.ker_in_order[1])
            if G.quantization:
                qrec = G.quantization.get(NodeId(node), None)
                if qrec is None:
                    LOG.warning(
                        'quantization set on graph but quantization parameters '
                        'for %s not found during adjust', node.name)
                else:
                    wqtype = qrec.in_qs[1]
                    # if the weights are channel quantized we need to update the qrec
                    if isinstance(wqtype, QType) and wqtype.quantized_dimension is not None:
                        wqtype = deepcopy(wqtype)
                        wqtype.quantized_dimension = trans.index(wqtype.quantized_dimension)
                        qrec.in_qs[1] = wqtype

        return modified
