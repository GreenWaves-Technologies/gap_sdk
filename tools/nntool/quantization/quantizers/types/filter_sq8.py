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
from typing import Sequence

from graph.types import (FilterParameters,
                         MultiplicativeBiasParameters)
from quantization.multiplicative.mult_quantization import (
    MultAddQuantizationRecord, MultConstantQuantizationRecord,
    MultQuantizationRecord, MultScalableFilterQuantizationRecord)
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from utils.stats_funcs import bits

LOG = logging.getLogger('nntool.' + __name__)


def sq8_filter(quantizer,
               node: FilterParameters,
               in_qs: Sequence[QType],
               opts=None,
               force_out=None):
    weights_q = SymmetricMultQType.from_array(arr=node.weights,
                                              quantized_dimension=self.get_quantized_dimension(
                                                  node),
                                              dtype=dtype, narrow_range=self._narrow_weights)
    if node.has_bias:
        biases_q = SymmetricMultBiasesQType(dtype=np.int32, scale=weights_q.scale * in_qs[0].scale)
    else:
        biases_q = SymmetricMultBiasesQType(dtype=np.int32, scale=np.array([1], dtype=np.int32))
    mul_biases_q = MultMulBiasScaleQType.from_filter(in_qs[0], weights_q, o_q, node)
    qrec = MultScalableFilterQuantizationRecord(in_qs=[in_qs[0]],
                                                out_qs=[o_q],
                                                weights_q=weights_q,
                                                biases_q=biases_q,
                                                mul_biases_q=mul_biases_q,
                                                constants_are_quantized=False)
    LOG.debug("filter %s qrec %s", node.name, qrec)
