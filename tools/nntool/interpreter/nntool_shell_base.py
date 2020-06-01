import os
import logging
from itertools import chain
from cmd2 import Cmd, CompletionItem
from execution.execution_progress import ExecutionProgress
from .shell_utils import NNToolShellLogHandler
from .settings import NNToolShellSettings

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
    'tensor_file': ""
}


def progress(step_idx, name):
    if not step_idx:
        return
    print("{}\r{} {}\r".format(" " * 70, step_idx, name), end="")
    return


class GraphNotReadyException(Exception):
    pass


class NNToolShellBase(NNToolShellSettings, Cmd):
    def __init__(self, args, nntool_workdir, *rest, **kwargs):
        self._nntool_workdir = nntool_workdir
        self._graph_idx = 0
        self._graphs = []
        self._settings = []
        self._tensor_store = {}
        super(NNToolShellBase, self).__init__(*rest, **kwargs)
        self.py_locals['tensors'] = self._tensor_store

        if args and args.log_level is not None:
            self.settings['log_level'] = args.log_level.upper()

        self._graph_idx = 0

        # settings overide graph file
        graph_file = self.settings['graph_file']
        tensor_file = self.settings['tensor_file']

        # command line overides that
        if args:
            if args.graph_file:
                graph_file = args.graph_file

            if args.tensor_file:
                tensor_file = args.tensor_file

            if args.template_file:
                self.settings['template_file'] = args.template_file

            if args.tf_quant:
                self.settings['load_quantization'] = args.tf_quant

            if args.dequant_tf:
                self.settings['load_dequantized'] = args.dequant_tf

        if 'log_level' not in self.settings:
            self.settings['log_level'] = "INFO"

        if graph_file:
            self._graphs = []
            self._startup_commands.append(
                self.__build_open_graph(graph_file,
                                        tensor_file,
                                        self.load_quantization,
                                        load_dequantized=self.settings.get('load_dequantized'))
            )
        else:
            self._graphs = [
                NO_GRAPH.copy()
            ]

        ExecutionProgress().listen(progress)
        LOG.propagate = False
        handler = NNToolShellLogHandler(self)
        formatter = logging.Formatter('%(module)s - %(message)s')
        handler.setFormatter(formatter)
        LOG.addHandler(handler)
        LOG.setLevel(self.settings['log_level'])

    def run_script(self, script_path):
        expanded_path = os.path.abspath(os.path.expanduser(script_path))

        # Make sure the path exists and we can access it
        if not os.path.exists(expanded_path):
            self.perror("'{}' does not exist or cannot be accessed".format(expanded_path))
            return

        # Make sure expanded_path points to a file
        if not os.path.isfile(expanded_path):
            self.perror("'{}' is not a file".format(expanded_path))
            return

        # An empty file is not an error, so just return
        if os.path.getsize(expanded_path) == 0:
            return

        try:
            # Read all lines of the script
            with open(expanded_path, encoding='utf-8') as target:
                script_commands = target.read().splitlines()
        except OSError as ex:  # pragma: no cover
            self.pexcept("Problem accessing script from '{}': {}".format(expanded_path, ex))
            return

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
    def __build_open_graph(graph_file, tensor_file, load_quantization, load_dequantized=False):
        command = ["open", graph_file, "-n"]
        if tensor_file:
            command.append("-t {}".format(tensor_file))
        if load_quantization:
            command.append("-q")
        if load_dequantized:
            command.append("-d")
        return " ".join(command)

    def execute_adjust_order(self):
        self.G.adjust_order()
        self.G.add_dimensions()

    def _update_prompt(self):
        self.prompt = "(NNT {} {}) ".format(os.path.basename(self.graph_file),
                                            self._graph_idx)

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
    def tensor_file(self):
        return self._graphs[self._graph_idx]['tensor_file']

    @tensor_file.setter
    def tensor_file(self, val):
        self._graphs[self._graph_idx]['tensor_file'] = val

    @property
    def tensor_store(self):
        return self._tensor_store

    @tensor_store.setter
    def tensor_store(self, val):
        self._tensor_store = val

    @property
    def tensor_store_names(self):
        return self._tensor_store.keys()
