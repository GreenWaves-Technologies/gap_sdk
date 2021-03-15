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

from quantization.float32.bfloat16_quantization import (
    Bfloat16FilterQuantizationRecord, Bfloat16QuantizationRecord,
    Bfloat16ScalableFilterQuantizationRecord)
from quantization.float32.float16_quantization import (
    Float16FilterQuantizationRecord, Float16QuantizationRecord,
    Float16ScalableFilterQuantizationRecord)
from quantization.float32.float32_quantization import (
    Float32FilterQuantizationRecord, Float32QuantizationRecord,
    Float32ScalableFilterQuantizationRecord)
from quantization.quantization_record_base import (
    FilterQuantizationRecordBase, InputOutputQuantizationRecordBase,
    ScalableFilterQuantizationRecordBase)

QREC_TYPES = {
    InputOutputQuantizationRecordBase: {
        'ieee32': Float32QuantizationRecord,
        'ieee16': Float16QuantizationRecord,
        'bfloat16': Bfloat16QuantizationRecord
    },
    FilterQuantizationRecordBase: {
        'ieee32': Float32FilterQuantizationRecord,
        'ieee16': Float16FilterQuantizationRecord,
        'bfloat16': Bfloat16FilterQuantizationRecord

    },
    ScalableFilterQuantizationRecordBase: {
        'ieee32': Float32ScalableFilterQuantizationRecord,
        'ieee16': Float16ScalableFilterQuantizationRecord,
        'bfloat16': Bfloat16ScalableFilterQuantizationRecord
    }
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
