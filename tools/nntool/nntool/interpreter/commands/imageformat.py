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
from nntool.interpreter.nntool_shell_base import NNToolShellBase
from nntool.quantization.qtype import QType

from nntool.graph.types import ImageFormatNode, NNEdge, TransposeNode

from nntool.graph.manipulations.formatter import insert_formatter, remove_formatter
class ImageFormatCommand(NNToolShellBase):
    def inputs_choices(self):
        if self.G is None:
            return []
        return [node.name for node in self.G.inputs()]

    def format_choices(self):
        return [fmt.lower() for fmt in ImageFormatNode.FORMAT_CHANGES] + ['none']

    def norm_choices(self):
        return [fmt.lower() for fmt in ImageFormatNode.NORMALIZATIONS] + ['none']

    # IMAGEFORMAT COMMAND
    parser_imageformat = Cmd2ArgumentParser(
        "inserts image format node into graphs")
    parser_imageformat.add_argument('input_node',
                                    choices_method=inputs_choices,
                                    help='input node name to format')
    parser_imageformat.add_argument('image_formatter',
                                    choices_method=format_choices,
                                    help='input node name to format')
    parser_imageformat.add_argument('image_normalizer',
                                    choices_method=norm_choices,
                                    help='input node name to format')

    @with_argparser(parser_imageformat)
    def do_imageformat(self, args: argparse.Namespace):
        """ Add or modify image format options."""
        self._check_graph()
        if args.input_node not in self.G:
            self.perror("input node not found")
            return
        input_node = self.G[args.input_node]
        out_edges = self.G.out_edges(input_node.name)
        if len(out_edges) == 1 and isinstance(out_edges[0].to_node, ImageFormatNode):
            remove_formatter(self.G, out_edges[0].to_node)
            self.G.add_dimensions()
            self.pfeedback(f'removed image formatter {out_edges[0].to_node.name}')
            return
        if args.image_formatter == "none" and args.image_normalizer == "none":
            self.pfeedback("no formatting set")
            self.G.add_dimensions()
            return
        insert_formatter(self.G, input_node,
                         args.image_formatter, args.image_normalizer)
        self.G.add_dimensions()
        self.pfeedback(f'inserted image formatter after node {input_node.name} with'
                       f'format {args.image_formatter} and normalization {args.image_normalizer}')


