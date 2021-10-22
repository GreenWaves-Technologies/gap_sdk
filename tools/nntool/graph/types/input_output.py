# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import logging
import sys

import numpy as np

from .base import InsensitiveToQuantization, NNNodeRef, Parameters, cls_op_name, not_generated

LOG = logging.getLogger("nntool." + __name__)

@not_generated
class InputOutputParameters(Parameters):

    def __init__(self, *args, dims=None, fixed_order=False, short_name=None, **kwargs):
        super().__init__(*args, **kwargs)
        self._output_value = None
        self._index = None
        self._short_name = short_name
        self.dims = dims
        self.at_options.valid_options['ALLOCATE'] = int
        self.at_options.valid_options['FIXED_ORDER'] = int
        self.fixed_order = fixed_order

    @property
    def graph_anon_label(self):
        return self.graph_label

    @property
    def graph_label(self):
        shape = self.out_dims[0] if self.out_dims and self.out_dims[0] else "No Shape!"
        if self.fixed_order:
            return [self.name, f'{shape}', "Frozen Order"]
        return [self.name, f'{shape}']

    @property
    def short_name(self):
        return self._short_name

    @short_name.setter
    def short_name(self, val):
        self._short_name = val

    @property
    def fixed_order(self):
        if hasattr(self.at_options, 'fixed_order'):
            return self.at_options.fixed_order == 1
        return False

    @fixed_order.setter
    def fixed_order(self, val):
        self.at_options.fixed_order = 1 if val else 0

    @property
    def output_value(self):
        return self._output_value

    @output_value.setter
    def output_value(self, value):
        self._output_value = value

    @property
    def index(self):
        return self._index

    @index.setter
    def index(self, value):
        self._index = value

    @property
    def can_equalize(self):
        return False

    def get_parameter_size(self):
        return 0


class InputBaseParameters(InputOutputParameters):

    @property
    def in_dims(self):
        dim = self.dims.clone()
        if self.in_dims_hint:
            dim.apply_naming_hints(self.in_dims_hint[0])
        return [dim]

    @in_dims.setter
    def in_dims(self, val):
        pass

    def __str__(self):
        return "I {} {}".format(
            self.dims,
            self.at_options
        )

    def get_output_size(self, _):
        out_dim = self.dims.clone()
        if self.out_dims_hint:
            out_dim.apply_naming_hints(self.out_dims_hint[0])
        return [out_dim]


@cls_op_name('input')
class InputParameters(InputBaseParameters, InsensitiveToQuantization):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.at_options.valid_options['EXTERN_INPUT_POINTER'] = int

    def __call__(self, graph):
        if graph.__class__.__name__ != 'NNGraph':
            raise ValueError('expecting NNGraph as parameter')
        return NNNodeRef(self, 0, graph)


    def verify(self, G):
        problems = []
        for edge in G.in_edges(self.name):
            problems.append(
                f'input node {self.name} has input edge from {edge.from_node.name}:{edge.from_idx}')
        for edge in G.out_edges(self.name):
            if edge.from_idx > 0:
                problems.append(f'input node {self.name} has output edge on idx {edge.from_idx} '
                                f'to {edge.to_node.name}:{edge.to_idx}')
        return problems

    def set_input(self, value):
        try:
            value = value.reshape(self.dims.shape)
        except ValueError as ex:
            trace_back = sys.exc_info()[2]
            raise ValueError(
                "Input data dimensions are not compatible with graph input: {!s}".format(
                    ex)
            ).with_traceback(trace_back)
        self.output_value = value


@cls_op_name('constant')
class ConstantInputParameters(InputBaseParameters):

    def __init__(self, *args, adjust_transpose=None, is_mutated=False,
                 is_intermediate=False, always_copy=False, value=None, qtype=None, **kwargs):
        super(ConstantInputParameters, self).__init__(*args, **kwargs)
        self.value = value
        del self.at_options.valid_options['FIXED_ORDER']
        self.at_options.valid_options['RESET_NAME'] = str
        self._adjust_transpose = adjust_transpose
        self._is_mutated = is_mutated
        self._is_intermediate = is_intermediate
        self._is_constant = True
        self._is_global = True
        self._always_copy = always_copy
        self._use_compressed = False
        self._qtype = qtype

    def __call__(self, graph):
        if graph.__class__.__name__ != 'NNGraph':
            raise ValueError('expecting NNGraph as parameter')
        return NNNodeRef(self, 0, graph)


    def verify(self, G):
        problems = []
        for edge in G.in_edges(self.name):
            problems.append(
                f'constant input node {self.name} has input edge from {edge.from_node.name}:{edge.from_idx}')
        for edge in G.out_edges(self.name):
            if edge.from_idx > 0:
                problems.append(f'constant input node {self.name} has output edge on idx {edge.from_idx} '
                                f'to {edge.to_node.name}:{edge.to_idx}')
        if self.value is None:
            problems.append(f'constant node {self.name} has no value')
        return problems

    @property
    def graph_anon_label(self):
        return ['Constant']

    @property
    def qtype(self):
        return self._qtype

    @qtype.setter
    def qtype(self, val):
        self._qtype = val

    @property
    def dqvalue(self):
        if self._qtype:
            return self._qtype.dequantize(self.value)
        return self.value

    @dqvalue.setter
    def dqvalue(self, val):
        self._qtype = None
        self.value = val

    @property
    def constant_store(self):
        return self._constant_store

    @property
    def value(self):
        if self._constant_store:
            # compressed returns the real value with the error induced by the compression
            value = self._constant_store.get(
                self, 0, compressed=self._use_compressed)
        else:
            value = self._value

        if self._always_copy and isinstance(value, np.ndarray):
            return value.copy()
        return value

    @value.setter
    def value(self, val):
        if self._constant_store:
            self._constant_store.set(self, 0, val)
        else:
            self._value = val

    def value_as(self, qtype, generation=False):
        # handles both None or both equal
        if self._qtype == qtype:
            return self.value
        # need to quantize
        if self._qtype is None:
            return qtype.quantize(self.value)
        # need to dequantize maybe need to quantize
        return qtype.quantize(self.dqvalue) if qtype else self.dqvalue

    @property
    def always_copy(self):
        return self._always_copy

    @always_copy.setter
    def always_copy(self, val):
        self._always_copy = val

    @property
    def reset_name(self):
        return self.at_options.reset_name

    @reset_name.setter
    def reset_name(self, val):
        self.at_options.reset_name = val

    @property
    def adjust_transpose(self):
        return self._adjust_transpose

    @adjust_transpose.setter
    def adjust_transpose(self, val):
        self._adjust_transpose = val

    @property
    def is_compressed(self):
        return self._use_compressed

    @property
    def use_compressed(self):
        return self._use_compressed

    @use_compressed.setter
    def use_compressed(self, val):
        self._use_compressed = val

    @property
    def is_constant(self):
        return self._is_constant

    @is_constant.setter
    def is_constant(self, val):
        self._is_constant = val

    @property
    def is_global(self):
        return self._is_global

    @is_global.setter
    def is_global(self, val):
        self._is_global = val

    @property
    def is_mutated(self):
        return self._is_mutated

    @is_mutated.setter
    def is_mutated(self, val):
        self._is_mutated = val

    @property
    def is_intermediate(self):
        return self._is_intermediate

    @is_intermediate.setter
    def is_intermediate(self, val):
        self._is_intermediate = val

    def get_parameter_size(self):
        if self._use_compressed and self._constant_store:
            return self._constant_store.get_compressed_size(self, 0)
        return self.dims.size()

    def get_parameters(self):
        return {'value': self.value}

    def set_parameters(self, val):
        self.value = val['value']

    def __str__(self):
        props = [param for param in ["is_mutated", "is_intermediate",
                                     "is_compressed"] if getattr(self, param)]
        return "Const {} {} {}".format(
            self.dims,
            " ".join(props),
            self.at_options
        )


@cls_op_name('output')
class OutputParameters(InputOutputParameters, InsensitiveToQuantization):

    def __init__(self, *args, **kwargs):
        super(OutputParameters, self).__init__(*args, **kwargs)
        self.at_options.valid_options['EXTERN_OUTPUT_POINTER'] = int

    def verify(self, G):
        problems = []
        for edge in G.out_edges(self.name):
            problems.append(
                f'output node {self.name} has output edge to {edge.to_node.name}:{edge.to_idx}')
        for edge in G.in_edges(self.name):
            if edge.to_idx > 0:
                problems.append(f'output node {self.name} has input edge on idx {edge.to_idx} '
                                f'from {edge.from_node.name}:{edge.from_idx}')
        return problems

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        return [out_dim]

    @property
    def out_dims(self):
        return [self.dims]

    @out_dims.setter
    def out_dims(self, val):
        self.dims = val[0]

    def __str__(self):
        return "O {} {}".format(
            self.dims,
            self.at_options
        )
