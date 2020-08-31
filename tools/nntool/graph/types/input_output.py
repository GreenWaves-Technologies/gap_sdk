# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import logging
import sys


from .base import (Transposable, Parameters)

LOG = logging.getLogger("nntool." + __name__)

class InputOutputParameters(Transposable):

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
    def short_name(self):
        return self._short_name

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

    def clone(self, name, groupn=None):
        raise NotImplementedError()


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
        return "I {} {} {}".format(
            self.dims,
            Transposable.__str__(self),
            self.at_options
        )

    def get_output_size(self, _):
        out_dim = self.dims.clone()
        if self.transpose_out:
            out_dim.transpose(self.transpose_out[0])
        if self.out_dims_hint:
            out_dim.apply_naming_hints(self.out_dims_hint[0])
        return [out_dim]


class InputParameters(InputBaseParameters):
    op_name = "input"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.at_options.valid_options['EXTERN_INPUT_POINTER'] = int

    def set_input(self, value):
        try:
            value = value.reshape(self.dims.shape)
        except ValueError as ex:
            trace_back = sys.exc_info()[2]
            raise ValueError(
                "Input data dimensions are not compatible with graph input: {!s}".format(ex)
            ).with_traceback(trace_back)
        self.output_value = value

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    # @property
    # def can_promoteq(self):
    #     return self.out_q.bits < STATS_BITS[-1]

    # def promoteq(self):
    #     if self.out_q.bits == STATS_BITS[-1]:
    #         raise ValueError("can't step further")
    #     self.out_q = get_quantization(self.activation_stats, None, self.out_q.bits * 2)
    #     return True


class ConstantInputParameters(InputBaseParameters):
    op_name = "constant"

    def __init__(self, *args, adjust_transpose=None, is_mutated=False,
                 is_intermediate=False, **kwargs):
        self.value = None
        super(ConstantInputParameters, self).__init__(*args, **kwargs)
        del self.at_options.valid_options['FIXED_ORDER']
        self.at_options.valid_options['RESET_NAME'] = str
        self.at_options.valid_options['GENERATE_VALUE'] = bool
        self._adjust_transpose = adjust_transpose
        self._is_mutated = is_mutated
        self._is_intermediate = is_intermediate
        self._value_quantization = None
        self._concated_nodes = []
        self.generate_value = True
        self._is_constant = True
        self._is_global = True

    @property
    def concated_nodes(self):
        return self._concated_nodes

    @property
    def reset_name(self):
        return self.at_options.reset_name

    @reset_name.setter
    def reset_name(self, val):
        self.at_options.reset_name = val

    @property
    def generate_value(self):
        return self.at_options.generate_value

    @generate_value.setter
    def generate_value(self, val):
        self.at_options.generate_value = val

    @property
    def value_quantization(self):
        return self._value_quantization

    @value_quantization.setter
    def value_quantization(self, val):
        self._value_quantization = val

    @property
    def adjust_transpose(self):
        return self._adjust_transpose

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

    @property
    def is_intermediate(self):
        return self._is_intermediate

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def get_parameter_size(self):
        return self.dims.size()

    def get_parameters(self):
        return {'value': self.value}

    def set_parameters(self, val):
        self.value = val['value']

    def __str__(self):
        props = [param for param in ["is_mutated", "is_intermediate"] if getattr(self, param)]
        return "Const {} {} {} {}".format(
            self.dims,
            " ".join(props),
            Transposable.__str__(self),
            self.at_options
        )


class OutputParameters(InputOutputParameters):
    op_name = "output"

    def __init__(self, *args, **kwargs):
        super(OutputParameters, self).__init__(*args, **kwargs)
        self.at_options.valid_options['EXTERN_OUTPUT_POINTER'] = int

    def get_output_size(self, in_dims):
        out_dim = in_dims[0].clone()
        if self.transpose_in:
            out_dim.transpose(self.transpose_in[0])
        return [out_dim]

    @property
    def out_dims(self):
        return [self.dims]

    @out_dims.setter
    def out_dims(self, val):
        self.dims = val[0]

    def __str__(self):
        return "O {} {} {}".format(
            self.dims,
            Transposable.__str__(self),
            self.at_options
        )

    def clone(self, name, groupn=None):
        raise NotImplementedError()
