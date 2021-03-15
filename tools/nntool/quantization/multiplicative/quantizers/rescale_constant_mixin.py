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

from graph.types import ConstantInputParameters
from utils.node_id import NodeId


class RescaleConstantMixin():
    @classmethod
    def rescale_constant(cls, node: ConstantInputParameters, scale, qrecs, dtype=None):
        qrec = qrecs[NodeId(node)]
        qtype = qrec.out_qs[0]
        if (qtype.scale == scale.astype(qtype.scale.dtype) and
                (dtype is None or dtype == qtype.dtype)):
            return
        if node.qtype:
            node.value = node.dqvalue
            node.qtype = None

        qtype.scale = scale
        if dtype:
            qtype.dtype = dtype
