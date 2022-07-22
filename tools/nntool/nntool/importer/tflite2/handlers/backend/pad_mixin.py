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


from nntool.graph.types import NNEdge, PadNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler


class PadMixin():

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']
        pads = kwargs['pads']
        pad_vals = kwargs.get('pad_vals', [(0, 0)] * len(pads))

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        stripped_pads, stripped_pad_vals = zip(*[(tuple(int(p) for p in pad), pad_val)
                                                 for idx, (pad, pad_val) in enumerate(zip(pads, pad_vals))
                                                 if x_shape[idx] is not None])
        pout_shape = [dim + sum(pads[idx]) if dim is not None else dim
                      for idx, dim in enumerate(x_shape)]

        params = PadNode(node.name, padding=stripped_pads, pad_vals=stripped_pad_vals)

        if opts.get('load_quantization'):
            G.quantization[params.name] = BackendHandler.load_tf_quantization([node.input[0]], node.output)
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pout_shape))
        return params
