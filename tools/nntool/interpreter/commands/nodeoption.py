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

from cmd2 import with_argparser
from generation.memory_device_info import (AT_L3_FLASH_DEVICES,
                                           AT_L3_RAM_DEVICES)
from interpreter.nntool_shell_base import (NODE_SELECTOR_HELP,
                                           NNToolArguementParser,
                                           NNToolShellBase)
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)


def nodeoption_choices_method(self, arg_tokens):
    step = arg_tokens['step'][0]
    nodes = self.get_node_step_or_name(step, show_errors=False)[0]
    keys = set()
    for node in nodes:
        keys |= set(node.at_options.valid_options.keys())
    return tuple(keys)


def nodeoption_value_choices_method(arg_tokens):
    parameter = arg_tokens['parameter'][0]
    if parameter in ("OUT_HOME_MEM_LOC", "OUT_EXEC_MEM_LOC"):
        return AT_L3_RAM_DEVICES + AT_L3_FLASH_DEVICES + ("AT_MEM_UNDEF", )
    return []


class NodeoptionCommand(NNToolShellBase):
    # nodeoption COMMAND
    parser_nodeoption = NNToolArguementParser()
    parser_nodeoption.add_argument('step', nargs=(0, 1),
                                   completer_method=NNToolShellBase.node_step_or_name_completer(
                                       allow_comma=True),
                                   help='Set this step number or name. ' + NODE_SELECTOR_HELP)
    parser_nodeoption.add_argument('parameter', nargs=(0, 1), choices_method=nodeoption_choices_method,
                                   help='Set this parameter')
    parser_nodeoption.add_argument('value', nargs=(0, 1), choices_function=nodeoption_value_choices_method,
                                   help='Set the parameter to this value')

    @with_argparser(parser_nodeoption)
    def do_nodeoption(self, args):
        """ Allows setting of autotiler generator control parameters and other code generation
options such as the location of inputs and outputs. For a complete set of the parameters that
can be set refer to the autotiler documentation."""
        self._check_graph()
        if args.step is None and args.parameter is None:
            for nodeid, elem in self.G.node_options.items():
                print("{}: {}".format(nodeid, elem))
            return

        nodes = self.get_node_step_or_name(args.step, allow_comma=True)[0]

        if args.parameter is None:
            nothing = True
            for node in nodes:
                node_options = self.G.node_options.get(NodeId(node))
                if node_options:
                    nothing = False
                    self.poutput(f'Node: {node.name}:')
                    print(node_options)
            if nothing:
                self.poutput("no nodeoptions set")
            return

        if not nodes:
            self.perror("No nodes selected")
            return

        for node in nodes:
            node_options = node.at_options
            if args.value is None:
                val = None
            else:
                try:
                    option_type = node_options.valid_options[args.parameter]
                except KeyError:
                    self.pwarning(
                        f"{args.parameter} is not a valid parameter for node {node.name}")
                    continue
                val = option_type(args.value)
            try:
                if args.parameter in ["RNN_STATES_AS_INPUTS", "LSTM_OUTPUT_C_STATE"] and val:
                    if args.parameter == "RNN_STATES_AS_INPUTS":
                        node.rnn_states_as_inputs = (val, self.G)
                    if args.parameter == "LSTM_OUTPUT_C_STATE":
                        node.lstm_output_c_state = val
                        node.set_c_state_as_output(self.G)
                else:
                    setattr(node_options, args.parameter, val)
                self.pfeedback(
                    f'set option {args.parameter} on node {node.name} to {val}')
                self.G.node_options[NodeId(node)] = node_options
            except KeyError:
                self.pwarning(
                    f"{args.parameter} is not a valid parameter for node {node.name}")
