# Copyright (C) 2022  GreenWaves Technologies, SAS

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

import numpy as np

from graph.types import (HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         ReluActivationParameters, SigmoidActivationParameters,
                         SoftMaxParameters, TanHActivationParameters)
from graph.types.activations import HTanHActivationParameters

def gen_act_infos(act_params, act_q):
    comment = ""
    if isinstance(act_params, ReluActivationParameters):
        contents = {
            'actscale': act_q.cache['scale_mul_biases_q'].qbiases.astype(np.uint8),
            'actscalen': act_q.cache['scale_mul_biases_q'].qnorms.astype(np.uint8)
        }
        if act_params.upper_bound is None:  # or fnode is not None:
            if act_q.in_qs[0].zero_point == 0:
                contents.update({
                    'a0': np.uint8(0),
                    'b0': np.uint8(0),
                    'c0': np.uint8(0),
                })
            else:
                contents.update({
                    'a0': act_q.in_qs[0].zero_point.astype(act_q.in_qs[0].dtype),
                    'b0': np.uint8(0),
                    'c0': np.uint8(0),
                })
        else:
            if act_q.in_qs[0].zero_point == 0:
                contents.update({
                    'a0': act_q.in_qs[0].quantize(act_params.upper_bound),
                    'b0': np.uint8(0),
                    'c0': np.uint8(0),
                })
            else:
                contents.update({
                    'a0': act_q.in_qs[0].zero_point.astype(act_q.in_qs[0].dtype),
                    'b0': act_q.in_qs[0].quantize(act_params.upper_bound),
                    'c0': np.uint8(0),
                })

    elif isinstance(act_params, (HSigmoidActivationParameters, HSwishActivationParameters)):
        # currently combines all scaling factors into one scale and shift
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        contents = {
            'actscale': act_q.cache['scale_mul_biases_q'].qbiases.astype(np.uint8),
            'actscalen': act_q.cache['scale_mul_biases_q'].qnorms.astype(np.uint8),
            'a0': act_q.cache['upper_bound'],
            'b0': act_q.cache['offset'],
            'c0': act_q.cache['mult']
        }
    elif isinstance(act_params, SoftMaxParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        contents = {
            # 'bias_sm': np.uint8(15 + np.ceil(np.log2(act_q.in_qs[0].scale)))
            'bias_sm': act_q.cache['bias_sm']
        }
    elif isinstance(act_params, LeakyActivationParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        contents = {
            'actscale': act_q.cache['scale_mul_biases_q'].qbiases.astype(np.uint8),
            'actscalen': act_q.cache['scale_mul_biases_q'].qnorms.astype(np.uint8),
            'a0': act_q.cache['leak_factor'],
            'b0': np.uint8(0),
            'c0': np.uint8(0),
        }
    elif isinstance(act_params, (SigmoidActivationParameters, TanHActivationParameters, HTanHActivationParameters)):
        contents = {
            'actscale': act_q.cache['scale_mul_biases_q'].qbiases.astype(np.uint8),
            'actscalen': act_q.cache['scale_mul_biases_q'].qnorms.astype(np.uint8),
            'a0': np.uint8(0),
            'b0': np.uint8(0),
            'c0': np.uint8(0),
        }
    else:
        raise NotImplementedError(
            "activation type not implemented in model generator")
    comment += f"in: {act_q.in_qs[0].scale[0]:.5f} out: {act_q.out_qs[0].scale[0]:.5f} "
    comment += f"actscale: {contents['actscale']} actscalen: {contents['actscalen']} "
    comment += f"A0: {contents['a0']} B0: {contents['b0']} C0: {contents['c0']}"

    return contents, comment
