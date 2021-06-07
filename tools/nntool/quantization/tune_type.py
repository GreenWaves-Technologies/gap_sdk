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

from graph.types.fusions import FusionBase
from utils.node_id import NodeId

QREC_TYPES = {
}

def tune_type(G, qrecs, node, qdtype):
    is_fusion = isinstance(node, FusionBase)
    nid = NodeId(node)
    qrec = QREC_TYPES[node.QREC_BASE][qdtype]()
    if nid in qrecs:
        old_qrec = qrecs[nid]
        qrec.clone_quantization_parameters(old_qrec)
    if is_fusion:
        for fnode in node.contained_nodes():
            fnid = NodeId(node, fnode=fnode)
            fqrec = QREC_TYPES[fnode.QREC_BASE][qdtype]()
            if fnid in qrecs:
                old_fqrec = qrecs[fnid]
                fqrec.clone_quantization_parameters(old_fqrec)
            qrecs[fnid] = fqrec

    qrecs[nid] = qrec
