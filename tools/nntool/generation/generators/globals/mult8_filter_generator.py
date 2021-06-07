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

from generation.generator_decorators import QREC_MULT8, generation_function
from generation.helpers.gen_scales import gen_scales
from graph.types import ConvFusionParameters, FilterParameters
from utils.node_id import NodeId

# pylint: disable=wildcard-import, unused-wildcard-import
from .global_names import *


@generation_function("globals", (FilterParameters, ConvFusionParameters), qrec_types=(QREC_MULT8,))
def mult8_filter_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    if fnode is not None:
        return False
    if isinstance(pnode, FilterParameters):
        gen_scales(gen, pnode, pnode, qrec)
    elif isinstance(pnode, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if node.fusion_type in ("conv_active_pool", "conv_active", "linear_active", "conv_pool_active", "conv_pool"):
            gen_scales(gen, pnode, cnodes[0], quants[0])
        else:
            return False
    else:
        return False
    return True
