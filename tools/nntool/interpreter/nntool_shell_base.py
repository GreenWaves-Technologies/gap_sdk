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

import json
import logging
import os
import re
from argparse import OPTIONAL
from copy import deepcopy
from itertools import chain
from typing import Any

from cmd2 import Cmd, Cmd2ArgumentParser, CompletionItem, plugin
from execution.execution_progress import ExecutionProgress
from importer.common.handler_options import HandlerOptions
from utils.json_serializable import JsonSerializableStateDecoder
from utils.make_var import make_expand, make_vars

from .settings import NNToolShellSettings
from .shell_utils import NNToolShellLogHandler

CHECK_GRAPH_ERROR = """
A graph must be opened to use this command. Use the open command to open a graph.
"""

CHECK_QUANTIZED_ERROR = """
The opened graph must be quantized to use this command. Run the aquant command.
"""

CHECK_ADJUSTED_ERROR = """
The opened graph must be adjusted to use this command. Run the adjust command.
"""

LOG = logging.getLogger("nntool")

NO_GRAPH = {
    'G': None,
    'graph_file': "",
}

MATCH_RANGE = re.compile(r'(\d)+ *\- *(\d)*')

NODE_SELECTOR_HELP = ("A list of nodes. You can select nodes with name, step(negative counts "
                      "from the end), name wildcard ending with * or a step range e.g. 1: 3")


def progress(step_idx, name):
    if not step_idx:
        return
    print("{}\r{} {}\r".format(" " * 70, step_idx, name), end="")
    return


class GraphNotReadyException(Exception):
    pass


class StateFileReplayError(Exception):
    pass


class NNToolArguementParser(Cmd2ArgumentParser):
    @property
    def _negative_number_matcher(self):
        return re.compile(r'^-?\d+(?::?-?\d+)$|^-\d*\.\d+$')

    @_negative_number_matcher.setter
    def _negative_number_matcher(self, val):
        pass

    @property
    def _has_negative_number_optionals(self):
        return False

    @_has_negative_number_optionals.setter
    def _has_negative_number_optionals(self, val):
        pass



class NNToolShellBase(NNToolShellSettings, Cmd):
    # commands to exclude from save state history
    EXCLUDE_FROM_HISTORY = ['help', 'py']
    STORE_ONCE_IN_HISTORY = []
    LOG_HANDLER_SET=False

    def __init__(self, args, *rest, **kwargs):
        self._graph_idx = 0
        self._graphs = [NO_GRAPH.copy()]
        self._cmd_history = [[]]
        self._history_stats = []
        self._first_graph_open = False
        self._replaying_history = False
        self._settings = []
        self._tensor_store = {}
        super(NNToolShellBase, self).__init__(*rest, **kwargs)
        self.feedback_to_output = True
        self.register_postcmd_hook(self._record_history)
        self.py_locals['tensors'] = self._tensor_store

        if args and args.log_level is not None:
            self._startup_commands.append(
                f'set log_level {args.log_level.upper()}')
        else:
            self._startup_commands.append('set log_level INFO')

        if args and args.anonymise:
            self._startup_commands.append(
                f'set anonymise true')

        self._graph_idx = 0

        # settings overide graph file
        graph_file = self.settings['graph_file']

        # command line overides that
        if args:
            if args.graph_file:
                graph_file = args.graph_file

            if args.template_file:
                self._startup_commands.append(
                    f'set template_file {args.template_file}')

        if graph_file:
            self._startup_commands.append(
                self.__build_open_graph(graph_file, args)
            )

        if not self.LOG_HANDLER_SET:
            ExecutionProgress().listen(progress)
            LOG.propagate = False
            handler = NNToolShellLogHandler(self, LOG)
            formatter = logging.Formatter('%(module)s - %(message)s')
            handler.setFormatter(formatter)
            NNToolShellBase.LOG_HANDLER_SET = True

        self.py_locals['graphs'] = self._graphs

    @classmethod
    def save_test_state(cls, commands, state_file):
        interpreter = cls({})
        interpreter.run_commands(commands)
        interpreter.do_save_state(state_file)
        return interpreter.G

    @classmethod
    def get_graph_from_state(cls, statefile):
        interpreter = cls({})
        interpreter.load_state_file(statefile)
        return interpreter.G

    @classmethod
    def get_graph_from_commands(cls, commands, settings=None):
        interpreter = cls({})
        if settings:
            interpreter.settings.update(settings)
        interpreter.run_commands(commands)
        if settings is not None:
            settings.update(interpreter.settings)
        return interpreter.G

    @classmethod
    def run_commands_on_graph(cls, G, commands, settings=None):
        interpreter = cls({})
        if settings:
            interpreter.settings.update(settings)
        interpreter.G = G
        interpreter.run_commands(commands)
        if settings is not None:
            settings.update(interpreter.settings)
        return interpreter.G

    def run_script(self, script_path):
        expanded_path = os.path.abspath(os.path.expanduser(script_path))

        # Make sure the path exists and we can access it
        if not os.path.exists(expanded_path):
            self.perror(
                f"'{expanded_path}' does not exist or cannot be accessed")
            return

        # Make sure expanded_path points to a file
        if not os.path.isfile(expanded_path):
            self.perror(f"'{expanded_path}' is not a file")
            return

        # An empty file is not an error, so just return
        if os.path.getsize(expanded_path) == 0:
            return

        try:
            # Read all lines of the script
            with open(expanded_path, encoding='utf-8') as target:
                script_commands = target.read().splitlines()
        except OSError as ex:  # pragma: no cover
            self.pexcept(
                f"Problem accessing script from '{expanded_path}': {ex}")
            return

        M = make_vars(origin=None)
        script_commands = [make_expand(M, command)
                           for command in script_commands]
        script_commands = [command.format(**os.environ)
                           for command in script_commands]

        orig_script_dir_count = len(self._script_dir)

        try:
            self.runcmds_plus_hooks(self._startup_commands)
            self._startup_commands.clear()
            self._script_dir.append(os.path.dirname(expanded_path))
            return self.runcmds_plus_hooks(script_commands)

        finally:
            with self.sigint_protection:
                # Check if a script dir was added before an exception occurred
                if orig_script_dir_count != len(self._script_dir):
                    self._script_dir.pop()

    def other_open_graphs(self, only_open=False):
        items = []
        for graph_idx, graph in enumerate(self._graphs):
            if graph_idx == self._graph_idx:
                continue
            if graph['G'] is None:
                if only_open:
                    continue
                name = "No Graph"
            else:
                name = graph['G'].name
            items.append(CompletionItem(graph_idx, name))
        return items

    def inputs_and_outputs(self):
        if self.G is None:
            return []
        return [node.name for node in chain(self.G.inputs_and_constants(), self.G.outputs())]

    def _check_adjusted(self):
        if not self.is_adjusted:
            raise GraphNotReadyException(CHECK_ADJUSTED_ERROR)

    def _check_graph(self):
        if self.G is None:
            raise GraphNotReadyException(CHECK_GRAPH_ERROR)

    def _check_quantized(self):
        if not self.is_quantized:
            raise GraphNotReadyException(CHECK_QUANTIZED_ERROR)

    @staticmethod
    def __build_open_graph(graph_file, args):
        command = ["open", graph_file]
        for option in HandlerOptions.get_all_handler_options().values():
            if hasattr(args, option['name']):
                if option['val_type'] == bool:
                    setting = getattr(args, option['name'])
                    if option['default']:
                        if not setting:
                            command.append(f"--no_{option['name']}")
                    else:
                        if setting:
                            command.append(f"--{option['name']}")
                else:
                    command.append(
                        f"--{option['name']} {getattr(args, option['name'])}")
        return " ".join(command)

    def execute_adjust_order(self):
        self.G.adjust_order()
        self.G.add_dimensions()

    def _update_prompt(self):
        self.prompt = f"(NNT {os.path.basename(self.graph_file)} {self._graph_idx}) "

    def _reset_history(self):
        if self._replaying_history:
            return
        if len(self._cmd_history) <= self._graph_idx:
            set_commands = [command for command in self._cmd_history[0]
                            if command.startswith('set')]
            for _ in range(len(self._cmd_history), self._graph_idx + 1):
                self._cmd_history.append(deepcopy(set_commands))
        else:
            self._cmd_history[self._graph_idx] = [
                command for command in self._cmd_history[self._graph_idx]
                if command.startswith('set')]
        if len(self._history_stats) <= self._graph_idx:
            self._history_stats.append(
                [None] * (self._graph_idx + 1 - len(self._history_stats)))

    def _record_history(self, data: plugin.PostcommandData) -> plugin.PostcommandData:
        if self._replaying_history:
            return data
        if data.statement.command == 'set':
            # set commands are stored in all histories
            for history_set in self._cmd_history:
                history_set.append(data.statement.raw)
        else:
            if data.statement.command not in self.EXCLUDE_FROM_HISTORY:
                if data.statement.command in self.STORE_ONCE_IN_HISTORY:
                    self._cmd_history[self._graph_idx] = [
                        elem for elem in self._cmd_history[self._graph_idx]
                        if not elem.startswith(data.statement.command)]
                self._cmd_history[self._graph_idx].append(data.statement.raw)
        return data

    def _record_stats(self, astats):
        self._history_stats[self._graph_idx] = astats

    def perror(self, msg: Any = '', *, end: str = '\n', apply_style: bool = True):
        if self._replaying_history:
            if isinstance(msg, Exception):
                raise msg
            raise StateFileReplayError(msg)
        super(NNToolShellBase, self).perror(
            msg, end=end, apply_style=apply_style)

    def pexcept(self, msg: Any, *, end: str = '\n', apply_style: bool = True):
        if self._replaying_history:
            if isinstance(msg, Exception):
                raise msg
            raise StateFileReplayError(msg)
        super(NNToolShellBase, self).pexcept(
            msg, end=end, apply_style=apply_style)

    def _replay_history(self):
        self._replaying_history = True
        for command in self._cmd_history[self._graph_idx]:
            if self.onecmd_plus_hooks(command, add_to_history=False):
                break
        self._replaying_history = False

    def run_commands(self, commands):
        self.graph_history = {'history': commands, 'stats': None}
        self._replay_history()

    def load_state_file(self, filepath, orgmodel_path=None):
        with open(filepath) as fp:
            history = json.load(fp, cls=JsonSerializableStateDecoder)
        if orgmodel_path:
            idxs = [i for i, cmd in enumerate(history['history']) if "open" in cmd]
            for idx in idxs:
                cmd_list = history['history'][1].split()
                cmd_list[1] = orgmodel_path
                history['history'][idx] = ' '.join([str(elem) for elem in cmd_list])
        self.graph_history = history
        self._replay_history()

    @staticmethod
    def no_history(function):
        func_name = function.__name__
        assert func_name.startswith('do_')
        NNToolShellBase.EXCLUDE_FROM_HISTORY.append(func_name[3::])
        return function

    @staticmethod
    def store_once_in_history(function):
        func_name = function.__name__
        assert func_name.startswith('do_')
        NNToolShellBase.STORE_ONCE_IN_HISTORY.append(func_name[3::])
        return function

    @property
    def graph_history(self):
        return {'history': self._cmd_history[self._graph_idx],
                'stats': self._history_stats[self._graph_idx]}

    @graph_history.setter
    def graph_history(self, val):
        if not isinstance(val, dict):
            raise ValueError('invalid history file')
        self._reset_history()
        if 'history' not in val:
            raise ValueError('invalid history file')
        history = val['history']
        if not isinstance(history, list) or not all(isinstance(elem, str) for elem in val['history']):
            raise ValueError('invalid history file')
        self._cmd_history[self._graph_idx] = history
        stats = val.get('stats', None)
        self._history_stats[self._graph_idx] = stats

    @property
    def history_stats(self):
        return self._history_stats[self._graph_idx]

    @property
    def replaying_history(self):
        return self._replaying_history

    @replaying_history.setter
    def replaying_history(self, val):
        self._replaying_history = val

    @property
    def settings(self):
        return self._settings

    @settings.setter
    def settings(self, val):
        self._settings = val

    @property
    def G(self):
        return self._graphs[self._graph_idx]['G']

    @G.setter
    def G(self, val):
        self._graphs[self._graph_idx]['G'] = val

    @property
    def graph_name(self):
        if self._graph_idx is None:
            return ""
        return self._graphs[self._graph_idx]['G'].name

    @graph_name.setter
    def graph_name(self, val):
        if self._graph_idx is None:
            return
        self._graphs[self._graph_idx]['G'].name = val

    @property
    def is_quantized(self):
        return self._graphs[self._graph_idx]['G'].quantization is not None

    @property
    def is_adjusted(self):
        return self._graphs[self._graph_idx]['G'].graph_identity.is_adjusted

    @property
    def is_equalized(self):
        return self._graphs[self._graph_idx]['G'].graph_identity.is_equalized

    @property
    def graph_file(self):
        return self._graphs[self._graph_idx]['graph_file']

    @graph_file.setter
    def graph_file(self, val):
        self._graphs[self._graph_idx]['graph_file'] = val

    @property
    def tensor_store(self):
        return self._tensor_store

    @tensor_store.setter
    def tensor_store(self, val):
        self._tensor_store = val

    @property
    def tensor_store_names(self):
        return self._tensor_store.keys()

    @staticmethod
    def is_int(numstr: str):
        try:
            int(numstr)
            return True
        except ValueError:
            return False

    def node_step_or_name_completer(self, text: str, line: str, begidx: int, endidx: int):
        self._check_graph()
        if self.is_int(text) or text.endswith("*"):
            return []
        if self.G is None:
            return []
        range_match = text.split(':', maxsplit=2)
        elem = range_match[-1]
        if elem.isnumeric():
            return []
        if len(range_match) == 2:
            first_node = self.find_node_from_index_or_name(range_match[0])
            if not elem:
                return [f'{range_match[0]}:{node.name}' for node in self.G.nodes()
                        if first_node is None or node.step_idx > first_node.step_idx]
            return [f'{range_match[0]}:{node.name}' for node in self.G.nodes()
                    if node.name.startswith(elem) and (first_node is None or node.step_idx > first_node.step_idx)]
        if not elem:
            return [node.name for node in self.G.nodes()]
        return [node.name for node in self.G.nodes() if node.name.startswith(elem)]

    def find_node_from_index_or_name(self, idx_or_name: str):
        if self.is_int(idx_or_name):
            num = int(idx_or_name)
            if num < 0:
                num = len(self.G.graph_state.steps) + num
            if num < 0 or num > len(self.G.graph_state.steps):
                return None
            return self.G.graph_state.steps[num]['node']
        if idx_or_name in self.G:
            return self.G[idx_or_name]
        return None

    def get_node_step_or_name(self, arg, show_errors=True, classes=None):
        if arg == '*':
            return self.G.nodes(node_classes=classes), 'all nodes'
        elif arg.endswith('*'):
            arg = arg[0:-1:]
            return [node for node in self.G.nodes(node_classes=classes)
                    if node.name.startswith(arg)], f'nodes starting with {arg}'

        elems = arg.split(':', maxsplit=2)
        nodes = [self.find_node_from_index_or_name(
            elem) if elem else False for elem in elems]
        if nodes[0] is None:
            self.perror(f"can't find node {elems[0]}")
            return [], ""

        if not nodes[0]:
            nodes[0] = self.G.graph_state.steps[0]['node']
        if len(nodes) == 2:
            if nodes[1] is None:
                self.perror(f"can't find node {elems[1]}")
                return [], ""
            if not nodes[1]:
                nodes[1] = self.G.graph_state.steps[-1]['node']
            return ([node for node in self.G.nodes(node_classes=classes)
                     if node.step_idx >= nodes[0].step_idx and node.step_idx <= nodes[1].step_idx],
                    f"{nodes[0].name} -> {nodes[1].name}")
        if classes and not isinstance(nodes[0], classes):
            if show_errors:
                self.perror(
                    'selected node is not compatible with this command')
            return [], ""
        return [nodes[0]], nodes[0].name


#pylint: disable=invalid-name
no_history = NNToolShellBase.no_history
store_once_in_history = NNToolShellBase.store_once_in_history
