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
from quantization.handlers_helpers import (add_options_to_parser,
                                           get_set_options_from_args)
from quantization.tuneq import (FLOAT_DTYPES, POW2_DTYPES, tune_float,
                                tune_options, tune_pow2, tune_scaled)


class QtuneCommand(NNToolShellBase):

    # QTUNE COMMAND
    parser_tune = NNToolArguementParser()
    parser_tune.add_argument(
        'step',
        help='step to tune. ' + NODE_SELECTOR_HELP,
        completer_method=NNToolShellBase.node_step_or_name_completer)
    parser_tune_sub = parser_tune.add_subparsers(
        title='qtune subcommands', help='atune action to carry out on selected layer')
    parser_tune_set_float = parser_tune_sub.add_parser(
        'float',
        help='tune quantization scheme of layer to float')
    parser_tune_set_float.add_argument(
        'float_type', choices=['float16', 'bfloat16', 'float32'],
        help='float type to use for selected layers'
    )
    parser_tune_set_scaled = parser_tune_sub.add_parser(
        'scaled',
        help='tune quantization scheme of layer to scaled')
    parser_tune_set_pow2 = parser_tune_sub.add_parser(
        'pow2', help='tune quantization scheme of layer to pow2')
    parser_tune_set_pow2.add_argument(
        'int_type', choices=['int8', 'int16'],
        help='integer type to use for selected layers'
    )
    parser_tune_set_option = parser_tune_sub.add_parser(
        'option', help='set specific quantizer options on a layer or layers')
    add_options_to_parser(parser_tune_set_option)

    def qtune_set_options(self, nodes, node_descr, args):
        self._check_quantized()
        options = get_set_options_from_args(args)
        tune_options(self.G, nodes, options)
        self.pfeedback(f'set options on {node_descr}')

    def qtune_set_float(self, nodes, node_descr, args):
        float_dtype = FLOAT_DTYPES.get(args.float_type)
        if float_dtype is None:
            self.perror(f'invalid float type {args.float_type}')
            return
        tune_float(self.G, nodes, args.float_type)
        self.pfeedback(f'set {node_descr} to {args.float_type} operation')

    def qtune_set_scaled(self, nodes, node_descr, args):
        self._check_quantized()
        tune_scaled(self.G, nodes)
        self.pfeedback(f'set {node_descr} to scaled operation')

    def qtune_set_pow2(self, nodes, node_descr, args):
        self._check_quantized()
        pow2_dtype = POW2_DTYPES.get(args.int_type)
        if pow2_dtype is None:
            self.perror(f'invalid int type {args.int_type}')
            return
        tune_pow2(self.G, nodes, args.int_type)
        self.pfeedback(f'set {node_descr} to pow2 {args.int_type} operation')

    parser_tune_set_option.set_defaults(func=qtune_set_options)
    parser_tune_set_float.set_defaults(func=qtune_set_float)
    parser_tune_set_scaled.set_defaults(func=qtune_set_scaled)
    parser_tune_set_pow2.set_defaults(func=qtune_set_pow2)

    @with_argparser(parser_tune)
    def do_qtune(self, args):
        """
Tune quantization of graph."""
        self._check_graph()
        nodes, node_descr = self.get_node_step_or_name(args.step)
        if not nodes:
            return
        func = getattr(args, 'func', None)

        if func is not None:
            # Call whatever subcommand function was selected
            func(self, nodes, node_descr, args)
        else:
            # No subcommand was provided, so call help
            self.do_help('qtune')
