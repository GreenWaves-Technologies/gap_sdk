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

import argparse
from copy import deepcopy

from cmd2 import Cmd2ArgumentParser, with_argparser

from graph.types import (NNEdge, ResizerParameters, BilinearResizerParameters,
                         NearestNeighborResizerParameters)
from interpreter.nntool_shell_base import NNToolShellBase

from utils.node_id import NodeId


class InputResizerCommand(NNToolShellBase):
    def inputs_choices(self):
        if self.G is None:
            return []
        return [node.name for node in self.G.inputs()]

    def op_choices(self):
        return [fmt.lower() for fmt in ResizerParameters.SUPPORTED_OP_TYPES] + ['none']

    # resizer COMMAND
    parser_resizer = Cmd2ArgumentParser("inserts image format node into graphs")
    parser_resizer.add_argument('input_node',
                                choices_method=inputs_choices,
                                help='input node name to format')
    parser_resizer.add_argument('resize_op',
                                choices_method=op_choices,
                                help='input node name to format')
    parser_resizer.add_argument('--from_w', type=int, default=None, help='from_width')
    parser_resizer.add_argument('--from_h', type=int, default=None, help='from_height')

    @with_argparser(parser_resizer)
    def do_input_resizer(self, args: argparse.Namespace):
        """ Add or modify image format options."""
        self._check_graph()
        if args.input_node not in self.G:
            self.perror("input node not found")
            return
        input_node = self.G[args.input_node]
        out_edge = self.G.out_edges(input_node.name)[0]
        if args.resize_op == "none":
            self.pfeedback("no resize operation set")
            self.G.add_dimensions()
            return
        if args.from_w is None or args.from_h is None:
            self.pfeedback("no from_shape arguments set")
            return
        from_shape = (args.from_h, args.from_w)
        self.G.changes.input_resizer(input_node.name, args.resize_op, from_shape)
        insert_resizer(self.G, out_edge, args.resize_op, from_shape)
        self.G.add_dimensions()


def insert_resizer(G, out_edge, resize_op, from_shape):
    input_node = out_edge.from_node
    net_in_dim = input_node.in_dims[0]
    from_dim = deepcopy(net_in_dim)
    from_dim.h = from_shape[0]
    from_dim.w = from_shape[1]
    if resize_op == 'bilinear':
        resize_node = BilinearResizerParameters(input_node.name + "_resizer", (net_in_dim.h, net_in_dim.w))
    elif resize_op == 'nearest':
        resize_node = NearestNeighborResizerParameters(input_node.name + "_resizer", (net_in_dim.h, net_in_dim.w))
    to_node = out_edge.to_node
    to_idx = out_edge.to_idx
    resize_node.in_dims = [from_dim]
    input_node.dims.h = from_shape[0]
    input_node.dims.w = from_shape[1]

    # qrec updated to reflect resizer
    input_qrec = G.quantization and G.quantization.get(NodeId(input_node))
    if input_qrec:
        resizer_qrec = deepcopy(input_qrec)
        resizer_qrec.in_qs = resizer_qrec.out_qs
        G.quantization[NodeId(resize_node)] = resizer_qrec

    G.remove_edge(out_edge)
    G.add_node(resize_node)
    G.add_edge(NNEdge(input_node, resize_node))
    G.add_edge(NNEdge(resize_node, to_node, to_idx=to_idx))
