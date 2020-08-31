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

from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from generation.memory_device_info import AT_L3_FLASH_DEVICES, AT_L3_RAM_DEVICES
from utils.node_id import NodeId

def nodeoption_choices_method(self, arg_tokens):
    step_num = arg_tokens['step'][0]
    if step_num == '*':
        keys = []
        for step in self.G.graph_state.steps:
            node = step['node']
            keys.extend(node.at_options.valid_options.keys())
        return keys
    try:
        step_num = int(step_num)
        node = self.G.graph_state.steps[step_num]['node']
    except ValueError:
        node = self.G[step_num]
    return node.at_options.valid_options.keys()

def nodename_choices_method(self, arg_tokens):
    step_start = arg_tokens['step'][0]
    try:
        _ = int(step_start)
        return []
    except ValueError:
        return [step['node'].name for step in self.G.graph_state.steps if step['node'].name.startswith(step_start)] + ["*"]

def nodeoption_value_choices_method(arg_tokens):
    parameter = arg_tokens['parameter'][0]
    if parameter in ("OUT_HOME_MEM_LOC", "OUT_EXEC_MEM_LOC"):
        return AT_L3_RAM_DEVICES + AT_L3_FLASH_DEVICES + ("AT_MEM_UNDEF", )
    return []

class NodeoptionCommand(NNToolShellBase):
    # nodeoption COMMAND
    parser_nodeoption = Cmd2ArgumentParser()
    parser_nodeoption.add_argument('step', nargs=(0, 1), choices_method=nodename_choices_method,
                                   help='Set this step number or name')
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
        if args.step is None or (args.step == '*' and args.parameter is None):
            for nodeid, elem in self.G.node_options.items():
                print("{}: {}".format(nodeid, elem))
            return

        if args.step == '*':
            nodes = [step['node'] for step in self.G.graph_state.steps]
        else:
            try:
                try:
                    step = int(args.step)
                    nodes = [self.G.graph_state.steps[step]['node']]
                except ValueError:
                    nodes = [self.G[args.step]]
            except IndexError:
                self.perror("%s is not a valid step or node to set %s"%(args.step, args.parameter))
                return

        if args.parameter is None:
            node_options = self.G.node_options.get(NodeId(nodes[0]))
            if node_options:
                print(node_options)
            else:
                print("nothing set")
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
                except:
                    self.perror("%s is not a valid parameter for node %s"%(args.parameter, node.name))
                    continue
                val = option_type(args.value)
            try:
                setattr(node_options, args.parameter, val)
                self.G.node_options[NodeId(node)] = node_options
            except:
                self.perror("%s is not a valid parameter for node %s"%(args.parameter, node.name))
