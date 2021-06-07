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

from cmd2 import with_argparser
from interpreter.nntool_shell_base import (NODE_SELECTOR_HELP,
                                           NNToolArguementParser,
                                           NNToolShellBase)

from graph.types import ConstantInputParameters


class CompressCommand(NNToolShellBase):

    # COMPRESS COMMAND
    parser_compress = NNToolArguementParser()
    parser_compress.add_argument(
        'step',
        help='constant input to compress. ' + NODE_SELECTOR_HELP,
        completer_method=NNToolShellBase.node_step_or_name_completer)
    parser_compress_sub = parser_compress.add_subparsers(
        title='compress subcommands', help='compression strategy for the selected layers')
    parser_compress_bits = parser_compress_sub.add_parser(
        'bits',
        help='compress using a lookup index of a fixed number of bits')
    parser_compress_bits.add_argument(
        'num_bits', choices=list(str(v) for v in range(2, 9)),
        help='number of bits to use for lookup table indexes'
    )

    parser_compress_min_qnsr = parser_compress_sub.add_parser(
        'min_qnsr',
        help='compress keeping the value QSNR above a minimum value')

    parser_compress_min_qnsr.add_argument(
        'qsnr', type=int,
        help='QSNR to keep above'
    )

    parser_compress_auto = parser_compress_sub.add_parser(
        'auto',
        help='compress to a number of bits found automatically using inertia method')

    parser_compress_none = parser_compress_sub.add_parser(
        'none',
        help='disable compression')

    def compress_bits(self, node, args):
        comp_val = node.constant_store.compress(
            node, 0, bits=int(args.num_bits))
        if comp_val:
            node.use_compressed = True
            self.pfeedback(
                f'compress {node.name} to {args.num_bits} bits size'
                f' {node.value.size}->{comp_val.size} {"sparse" if comp_val.sparse else ""}')
            return node.value.size - comp_val.size
        return node.value.size

    def compress_qsnr(self, node, args):
        comp_val = node.constant_store.compress(node, 0, min_qsnr=args.qsnr)
        if comp_val:
            node.use_compressed = True
            self.pfeedback(
                f'compress {node.name} above QSNR of {args.qsnr} to {comp_val.bits} bits '
                f'size {node.value.size}->{comp_val.size} {"sparse" if comp_val.sparse else ""}')
            return node.value.size - comp_val.size
        return node.value.size

    def compress_auto(self, node, args):
        comp_val = node.constant_store.compress(node, 0)
        if comp_val:
            node.use_compressed = True
            self.pfeedback(
                f'auto compress {node.name} to {comp_val.bits} bits size '
                f'{node.value.size}->{comp_val.size} {"sparse" if comp_val.sparse else ""}')
            return node.value.size - comp_val.size
        return node.value.size

    def compress_none(self, node, args):
        node.use_compressed = False
        self.pfeedback(f'disable compression on {node.name}')
        return node.value.size

    parser_compress_bits.set_defaults(func=compress_bits)
    parser_compress_min_qnsr.set_defaults(func=compress_qsnr)
    parser_compress_auto.set_defaults(func=compress_auto)
    parser_compress_none.set_defaults(func=compress_none)

    @with_argparser(parser_compress)
    def do_compress(self, args):
        """
Tune quantization of graph."""
        self._check_graph()
        nodes, node_descr = self.get_node_step_or_name(
            args.step, classes=ConstantInputParameters)
        if not nodes:
            return
        func = getattr(args, 'func', None)

        if func is not None:
            # Call whatever subcommand function was selected
            comp_size = 0
            node_size = 0
            for node in nodes:
                node_size += node.value.size
                comp_size += func(self, node, args)
            self.pfeedback(
                f'uncompressed size {node_size} compressed size {comp_size} compression {(100*comp_size)//node_size}%')
        else:
            # No subcommand was provided, so call help
            self.do_help('compress')
