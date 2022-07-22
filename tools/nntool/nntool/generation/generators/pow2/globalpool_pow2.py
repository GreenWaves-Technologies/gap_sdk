# Copyright (C) 2021, 2022  GreenWaves Technologies, SAS

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
from functools import reduce

from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.types.global_pooling import (GlobalAveragePoolNode,
                                        GlobalMaxPoolNode,
                                        GlobalPoolingNodeBase)
from nntool.utils.largest_factor import balanced_divisors

LOG = logging.getLogger(__name__)


@paramstype(GlobalAveragePoolNode, GlobalMaxPoolNode)
@ktype('symmetric')
class GlobalPoolPOW2Generator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cls.set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.kernels.append(GlobalPoolKernel(
            cname, node, qrec, gen_ctrl=node.get_gen_ctrl()))
        return True


class GlobalPoolKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_GlobalPool("{cname}", {gen_ctrl}, {at_bits(in_qtype)}, {at_bits(out_qtype)},
                {in_qtype.q}, {out_qtype.q}, 1, 1, {in_c}, {out_c}, {in_w}, {in_h}, {kop_pool});
'''

    def __init__(self, cname, pool_params: GlobalPoolingNodeBase, pool_qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        kop_pool = {
            "average": "KOP_GLOBAL_AVGPOOL",
            "max": "KOP_GLOBAL_MAXPOOL",
        }.get(pool_params.pool_type)

        in_dim = pool_params.in_dims[0]

        in_c = reduce(lambda x, y: x * y, (sz for idx, sz in enumerate(in_dim.shape)
                                                if idx not in pool_params.axis))
        (in_h, in_w) = balanced_divisors(in_dim.size()/in_c)

        attrs = {
            'in_qtype': pool_qrec.in_qs[0],
            'out_qtype': pool_qrec.out_qs[0],
            'in_c': in_c,
            'out_c': in_c,
            'in_h': in_h,
            'in_w': in_w,
            'kop_pool': kop_pool
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': pool_params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
