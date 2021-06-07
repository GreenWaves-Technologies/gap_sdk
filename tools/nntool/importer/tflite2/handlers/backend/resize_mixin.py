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


from graph.dim import Dim
from graph.types.base import NNEdge
from utils.node_id import NodeId


class ResizeMixin():

    @classmethod
    def _common(cls, node, **kwargs):
        params_class = kwargs['params_class']
        opts_class = kwargs['opts_class']
        node_opts = node.get_options(opts_class)
        all_nodes = kwargs['all_nodes']
        opts = kwargs['opts']
        G = kwargs['G']

        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        new_shape = tuple(cls._verify_constant(inputs[1]))
        params = params_class(node.name,
                              new_shape=new_shape,
                              align_corners=node_opts.AlignCorners(),
                              halfpixel_centers=node_opts.HalfPixelCenters(),
                              in_dims_hint=[['h', 'w', 'c']],
                              out_dims_hint=[['h', 'w', 'c']])

        out_shape = params.get_output_size(
            [
                Dim(
                    shape=x[2].known_shape,
                    names=['h', 'w', 'c'],
                    is_ordered=True)
            ]
        )[0]
        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization(
                [node.input[0]], [node.output[0]])
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (
            params, 0, x[2].infer_mapping(out_shape.shape))
        return params
