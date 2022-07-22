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

import numpy as np

from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.bindings import CommentBindingList, GNodeArgEdge, NodeBindingList
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import GeneratorBase, paramstype
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.types import ConcatNode

LOG = logging.getLogger(__name__)

@paramstype(ConcatNode)
class GenConcatLastAxisParameters(GeneratorBase, InOutBindingsMixin):
    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.bindings.append(
            CommentBindingList(f"Node {node.name} inq {[str(in_q) for in_q in qrec.in_qs]} outq {[str(out_q) for out_q in qrec.out_qs]}")
        )
        bind_list = [GNodeArgEdge(in_par) for in_par in in_eparams] + [GNodeArgEdge(out_eparams[0], "GNA_OUT")]
        gen.bindings.append(
            NodeBindingList(cname, *bind_list))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.kernels.append(ConcatLastAxisKernel(cname, node, qrec))
        return True


class ConcatLastAxisKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_ConcatLastAxis_Generator("{cname}", {gen_ctrl}, {data_size}, {height}, {width1}, {width2}, {width3}, {width4}, KOP_CONCAT);
'''

    def __init__(self, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if qrec.out_qs[0].is_floating:
            gen_ctrl.float_dump = 1

        in_shapes = [in_dim.shape for in_dim in params.in_dims]
        if len(in_shapes) > 4:
            raise NotImplementedError(f"{params.name} More than 4 inputs not supported in HWC concat")
        height = np.prod(in_shapes[0][:-1])
        widths = [0, 0, 0, 0]
        for i, shape in enumerate(in_shapes):
            widths[i] = shape[-1]

        attrs = {
            'height': height,
            'width1': widths[0],
            'width2': widths[1],
            'width3': widths[2],
            'width4': widths[3],
            'data_size': qrec.out_qs[0].dtype_bits//8 if qrec.out_qs[0].signed else -qrec.out_qs[0].dtype_bits//8
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
