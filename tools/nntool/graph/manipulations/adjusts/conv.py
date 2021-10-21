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

from graph.types.pooling import PoolingParameters
import logging
from copy import deepcopy

from graph.types import Conv2DParameters, ConvFusionParameters
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
        if names != node.ker_in_order[0]:
            self.adjust_in_out_order(
                G, node, names, order=node.ker_in_order[0], has_batch=node.batch is not None)
            modified = True

        # if it doesn't then insert transpose before and after
        # check that the filter and bias match autotiler order
        if node.in_dims[1].order != node.ker_in_order[1]:
            modifier = True
            LOG.debug("step %s: %s adjust weights %s => %s",
                      node.step_idx, node.name, node.filter, " x ".join(node.ker_in_order[1]))
            trans = self.get_trans(node.filter.order, node.ker_in_order[1])
            self.apply_input_trans(G, node, trans, index=1)
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
                        wqtype.quantized_dimension = trans.index(
                            wqtype.quantized_dimension)
                        qrec.in_qs[1] = wqtype

        return modified


@handles(ConvFusionParameters, fusion_type=lambda x: x.startswith('conv'))
class ConvFusionAdjuster(AdjusterBase):
    def adjust(self, G, node):
        modified = False
        fusion_node = node
        filter_node = node.contained_filters()[0]

        # check that the transposed input 0 matches autotiler order
        names = filter_node.in_dims[0].order
        if names != filter_node.ker_in_order[0]:
            self.adjust_in_out_order(G, filter_node, names, order=filter_node.ker_in_order[0],
                                     fusion=fusion_node, has_batch=filter_node.batch is not None)
            modified = True

        # if it doesn't then insert transpose before and after
        # check that the filter and bias match autotiler order
        if filter_node.in_dims[1].order != filter_node.ker_in_order[1]:
            modifier = True
            LOG.debug("step %s: %s adjust weights %s => %s",
                      node.step_idx, node.name, filter_node.filter, " x ".join(filter_node.ker_in_order[1]))
            trans = self.get_trans(
                filter_node.filter.order, filter_node.ker_in_order[1])
            self.apply_input_trans(G, node, trans, index=1)
            filter_node.filter.impose_order(filter_node.ker_in_order[1])
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
                        wqtype.quantized_dimension = trans.index(
                            wqtype.quantized_dimension)
                        qrec.in_qs[1] = wqtype
                    G.quantization[NodeId(
                        filter_node, fnode=fusion_node)].in_qs[1] = wqtype

        pool_node = next(iter([node for node in fusion_node.contained_nodes()
                               if isinstance(node, PoolingParameters)]), None)
        if pool_node:
            pool_node.ker_in_order = [filter_node.ker_out_order[0]]
            pool_node.in_dims_hint = [filter_node.ker_out_order[0]]
            pool_node.out_dims_hint = [filter_node.ker_out_order[0]]

        return modified
