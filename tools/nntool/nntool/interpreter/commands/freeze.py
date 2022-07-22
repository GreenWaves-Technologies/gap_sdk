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
import logging
from itertools import chain
from typing import Dict, List

from cmd2 import Cmd2ArgumentParser, CompletionError, with_argparser
from nntool.interpreter.nntool_shell_base import NNToolShellBase

from nntool.graph.dim import Dim
from nntool.graph.manipulations.eliminate_transposes.transpose_helpers import (
    apply_transpose, reverse_transpose)
from nntool.graph.types import InputNode, NNEdge, OutputNode, TransposeNode

LOG = logging.getLogger("nntool")


class FreezeCommand(NNToolShellBase):
    #pylint: disable=dangerous-default-value
    def freeze_transpose_completer(
        self,
        text: str,
        #pylint: disable=unused-argument
        line: str,
        begidx: int,
        endidx: int,
        arg_tokens: Dict[str, List[str]] = {}
    ) -> List[str]:
        if 'node_names' not in arg_tokens:
            axes = list(range(10))
        else:
            try:
                shapes = [self.G[node_name].out_dims[0].shape for node_name in arg_tokens['node_names']]
            except KeyError as ex:
                raise CompletionError(f'node {ex.args[0]} not in graph') from ex
            if not all(len(shapes[0]) == len(shape) for shape in shapes[1:]):
                raise CompletionError('shape lengths must match on all selected nodes to set transpose')
            axes = list(range(len(shapes[0])))
        if text:
            comma = ',' if text[-1] != ',' else ''
            cur = text.split(',')
            if '' in cur:
                cur.remove('')
            left = [n for n in axes if str(n) not in cur]
            if len(axes) - len(cur) == 2:
                return [
                    f'{text}{comma}{left[0]},{left[1]}',
                    f'{text}{comma}{left[1]},{left[0]}'
                ]
            return [f'{text}{comma}{n}' for n in left]
        if len(axes) == 2:
            return ['1,0']
        return [str(n) for n in axes]

    #pylint: disable=dangerous-default-value
    def freeze_inputs_and_outputs(self, arg_tokens: Dict[str, List[str]] = {}):
        if self.G is None:
            return []
        keys = list(arg_tokens.keys())
        if 'transpose' in arg_tokens:
            if keys[0] == 'node_names':
                return []
        return [node.name for node in chain(self.G.input_nodes(), self.G.outputs())]

    # FREEZE COMMAND
    parser_freeze = Cmd2ArgumentParser(
        "toggle freezing of channel order of inputs or outputs")
    parser_freeze.add_argument('node_names',
                               nargs='+',
                               choices_method=freeze_inputs_and_outputs,
                               help='input or output node names to toggle freeze')
    parser_freeze.add_argument('--transpose',
                               completer_method=freeze_transpose_completer,
                               help='transpose the input or output before freezing. list of axes separated by commas.')

    @with_argparser(parser_freeze)
    def do_freeze(self, args: argparse.Namespace):
        """
Toggle freezing of channel order on inputs and outputs. When graph is adjusted frozen nodes
 will not change channel order."""
        self._check_graph()
        try:
            nodes = [self.G[node_name] for node_name in args.node_names]
        except KeyError as ex:
            self.perror(f'node {ex.args[0]} not in graph')
            return
        if not all([isinstance(node, (InputNode, OutputNode)) for node in nodes]):
            self.perror("all nodes should be inputs or outputs")
            return
        if args.transpose:
            shapes = [node.out_dims[0].shape for node in nodes]
            if not all(len(shapes[0]) == len(shape) for shape in shapes[1:]):
                self.perror('shape lengths must match on all selected nodes to set transpose')
                return
            transpose = [int(axis) for axis in args.transpose.split(',')]
            transpose_len = len(transpose)
            if len(set(transpose)) != transpose_len:
                self.perror(f'{args.transpose} has repeating axes')
                return
            shapes_len = len(shapes[0])
            if transpose_len != shapes_len:
                self.perror(f'{args.transpose} does not have {shapes_len} axes')
                return
        else:
            transpose = None

        for node in nodes:
            if node.fixed_order:
                LOG.info("node %s is unfrozen", node.name)
                node.fixed_order = False
            else:
                LOG.info("node %s is frozen", node.name)
                node.fixed_order = True
            if transpose:
                node.dims = Dim.unnamed(apply_transpose(node.dims.shape, transpose))
                if isinstance(node, InputNode):
                    trans_node = TransposeNode(
                        self.G.unique_name(f'{node.name}_trans'),
                        transpose=reverse_transpose(transpose))
                    self.G.insert_node_after(node, trans_node, edge_class=NNEdge)
                else:
                    trans_node = TransposeNode(
                        self.G.unique_name(f'{node.name}_trans'),
                        transpose=transpose)
                    self.G.insert_node_before(trans_node, node, edge_class=NNEdge)
                self.G.node_options[node.name] = node.at_options
