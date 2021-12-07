from quantization.qtype import QType
import numpy as np
from graph.types import (HSigmoidActivationParameters, HSwishActivationParameters,
                         LeakyActivationParameters, TanHActivationParameters, ReluActivationParameters,
                         SigmoidActivationParameters, SoftMaxParameters)
from quantization.multiplicative.mulbias import compute_in_out_scale
from quantization.symmetric.kernels.activations import (hsigmoid_mult_gen_factors, hswish_mult_gen_factors, leak_mult_gen_factor_q7)

def gen_act_infos(act_params, act_q):
    comment = ""
    if isinstance(act_params, ReluActivationParameters):
        compute_in_out_scale(act_q)
        actscale = act_q.cache['scale_mul_biases_q'].qbiases[0]
        actscalen = act_q.cache['scale_mul_biases_q'].qnorms[0]
        if act_params.upper_bound is None:  # or fnode is not None:
            if act_q.in_qs[0].zero_point == 0:
                contents = np.array(
                    [actscale, actscalen, 0, 0, 0], dtype=np.int8)
                if len(comment) == 0:
                    comment = "all 0"
            else:
                fac_1 = act_q.in_qs[0].zero_point
                contents = np.array(
                    [actscale, actscalen, fac_1, 0, 0], dtype=np.int8)
                comment += str.format("in: {:05f} out: {:05f} A0: {} B0: 0 C0: 0",
                                      act_q.in_qs[0].scale[0],
                                      act_q.out_qs[0].scale[0],
                                      fac_1[0])
        else:
            if act_q.in_qs[0].zero_point == 0:
                fac_1 = act_q.in_qs[0].quantize(act_params.upper_bound)
                contents = np.array([actscale, actscalen, fac_1, 0, 0],
                                    dtype=np.int8)
                comment += str.format("in: {:05f} out: {:05f} A0: {} B0: 0 C0: 0",
                                      act_q.in_qs[0].scale[0],
                                      act_q.out_qs[0].scale[0],
                                      fac_1[0])
            else:
                fac_1 = act_q.in_qs[0].zero_point
                fac_2 = act_q.in_qs[0].quantize(act_params.upper_bound)
                contents = np.array([actscale, actscalen, fac_1, fac_2, 0],
                                    dtype=np.int8)
                comment += str.format("in: {:05f} out: {:05f} A0: {} B0: {} C0: 0",
                                      act_q.in_qs[0].scale[0],
                                      act_q.out_qs[0].scale[0],
                                      fac_1[0],
                                      fac_2[0])
    elif isinstance(act_params, HSigmoidActivationParameters):
        # currently combines all scaling factors into one scale and shift
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        fac_1, upper_bound, _ = hsigmoid_mult_gen_factors(act_params, act_q)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             upper_bound, fac_1, 1],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: {} C0: 1",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0],
                              upper_bound[0], fac_1[0])
    elif isinstance(act_params, HSwishActivationParameters):
        # currently combines all scaling factors into one scale and shift
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        fac_1, upper_bound, _ = hswish_mult_gen_factors(act_q)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             upper_bound, fac_1, 1],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: {} C0: 1",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0],
                              upper_bound[0], fac_1[0])
    elif isinstance(act_params, SoftMaxParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        norm = 15 + np.ceil(np.log2(act_q.in_qs[0].scale))
        contents = np.array([norm, 0, 0, 0, 0], dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} NORM: {}",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              int(norm[0]))
    elif isinstance(act_params, LeakyActivationParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        compute_in_out_scale(act_q)
        leak_factor_quant = leak_mult_gen_factor_q7(act_params)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             leak_factor_quant, 0, 0],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: x C0: x",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0],
                              leak_factor_quant)
    elif isinstance(act_params, (SigmoidActivationParameters, TanHActivationParameters)):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        compute_in_out_scale(act_q, extra_scale=QType.Pow2(
            bits=32, q=7, signed=True).scale/act_q.in_qs[0].scale)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             0, 0, 0],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: x B0: x C0: x",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0])
    else:
        raise NotImplementedError("activation tye not implemented")

    return contents, comment
