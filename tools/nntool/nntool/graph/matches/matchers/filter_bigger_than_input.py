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

from nntool.graph.dim import Conv2DFilterDim, Dim, PadDim
from nntool.graph.types import Conv2DNode, ConvFusionNode
from nntool.utils.graph import GraphView

from ..matcher import Matcher, match_name, groups, description

LOG = logging.getLogger(__name__)

@match_name("filter_bigger_than_input")
@description("""Replace filters with kernel bigger than input feature""")
@groups('*')
class FilterBiggerThanInput(Matcher):
    def _match(self, G: GraphView, **kwargs):
        something_changed = False
        filt_nodes = [node for node in G.nodes()
                      if isinstance(node, (Conv2DNode, ConvFusionNode))]
        for filt_node in filt_nodes:
            pnode = filt_node
            if isinstance(filt_node, ConvFusionNode):
                cnodes = filt_node.contained_nodes()
                filt_node = cnodes[0]
            if not isinstance(filt_node, Conv2DNode):
                continue
            if filt_node.dilation != 1:
                continue
            in_dim = filt_node.in_dims
            filt_dim = filt_node.filter_dim
            if filt_dim.h <= in_dim[0].h and filt_dim.w <= in_dim[0].w:
                continue

            min_h = min(filt_dim.h, in_dim[0].h)
            min_w = min(filt_dim.w, in_dim[0].w)
            if min_h > 1 and min_w > 1:
                LOG.warning("Filter of %s [%dx%d] bigger than input [%dx%d] not optimal but will work on AT",
                            filt_node.name, filt_dim.h, filt_dim.w, in_dim[0].h, in_dim[0].w)
                continue

            if min_h == 1:
                ker_h = 1
                ker_h_reduced = True
            else:
                ker_h_reduced = False
                ker_h = filt_dim.h
            if min_w == 1:
                ker_w = 1
                ker_w_reduced = True
            else:
                ker_w_reduced = False
                ker_w = filt_dim.w
            if ker_h == filt_dim.h and ker_w == filt_dim.w:
                continue
            if filt_node.padding:
                if ker_h_reduced:
                    top = bottom = 0
                else:
                    top = filt_node.padding.t
                    bottom = filt_node.padding.b
                if ker_w_reduced:
                    left = right = 0
                else:
                    left = filt_node.padding.l
                    right = filt_node.padding.r
                padding = PadDim(top, bottom, left, right)
            else:
                padding = PadDim(0)

            new_filt_dim = Conv2DFilterDim(
                ker_h, ker_w, filt_dim.out_c, in_c=filt_dim.in_c)
            LOG.warning("Converting filter of %s from [%dx%d] -> [%dx%d]",
                        filt_node.name, filt_dim.h, filt_dim.w, new_filt_dim.h, new_filt_dim.w)
            filt_node.filter_dim = new_filt_dim
            filt_node.padding = padding
            new_w_idxs = []
            for dim in filt_dim.order:
                if dim in ('out_c', 'in_c'):
                    new_w_idxs.append(slice(None))
                elif dim == 'h':
                    if new_filt_dim.h == 1:
                        new_w_idxs.append(
                            slice(filt_node.padding.t, filt_node.padding.t + 1))
                    else:
                        new_w_idxs.append(slice(0, new_filt_dim.h))
                elif dim == 'w':
                    if new_filt_dim.w == 1:
                        new_w_idxs.append(
                            slice(filt_node.padding.l, filt_node.padding.l + 1))
                    else:
                        new_w_idxs.append(slice(0, new_filt_dim.w))
            weights_node = G.indexed_in_edges(pnode.name)[1].from_node
            weights_node.value = weights_node.value[tuple(new_w_idxs)]
            weights_node.dims = Dim.unnamed(weights_node.value.shape)
            something_changed = True



        return something_changed
