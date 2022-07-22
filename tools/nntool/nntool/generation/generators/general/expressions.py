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

from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import GeneratorBase, paramstype
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.types import ExpressionFusionNode

LOG = logging.getLogger(__name__)


@paramstype(ExpressionFusionNode)
class GenExpressionParameters(GeneratorBase, InOutBindingsMixin):
    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cls.set_multi_in_out_bindings(
            gen, in_eparams, out_eparams, cname, node, qrec)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        func_name, _ = gen.expressions_get_names(node)
        gen_name = f'{func_name}_gen'
        gen.kernels.append(ExpressionKernel(cname, node, qrec, gen_name))
        return True


class ExpressionKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
{gen_name}("{cname}");
'''

    def __init__(self, cname, params, qrec, gen_name):

        attrs = {
            'gen_name': gen_name
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs,)
