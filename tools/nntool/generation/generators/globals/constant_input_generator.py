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

import os

from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.generator_decorators import generation_function, QREC_POW2
from graph.types import ConstantInputParameters


@generation_function("globals", (ConstantInputParameters,), qrec_types=(QREC_POW2, ))
def constant_input_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    del node, fnode
    # the name cache will be updated when all the edges are analysed by local_generator
    # the name of the constant is attached to the output edge so find it
    out_edge = gen.G.out_edges(pnode.name)[0]
    eparams = out_edge.params
    cname = gen.naming_convension.get_edge_name(eparams.creating_node.name,
                                                eparams.creating_step,
                                                eparams.edge_type,
                                                eparams.edge_order)
    file_name = os.path.join(gen.opts['tensor_directory'],
                             cname+".tensor")
    const_info = ConstantInfo(file_name, qrec.out_qs[0], contents=qrec.out_qs[0].quantize(node.value))
    gen.globals.append(GlobalArgInfo(qrec.out_qs[0].ctype, cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info))
    return True
