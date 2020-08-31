from .bindings.mult8.conv_bindings_generator import \
    conv_bindings_generator as mult8_conv_bondings_generator
from .bindings.mult8.fc_binding_generator import \
    fc_bindings_generator as mult8_fc_bindigns_generator
from .bindings.mult8.inout_bindings_generator import \
    in_out_bindings_generator as mult8_in_out_bindings_generator
from .bindings.mult8.inout_infos_bindings_generator import \
    in_out_infos_bindings_generator
from .bindings.mult8.mat_vect_mult_bindings_generator import \
    mat_vect_mul_bindings_generator as mult8_mat_vect_mul_bindings_generator
from .bindings.mult8.matadd_bindings_generator import \
    matadd_bindings_generator as mult8_matadd_bindings_generator
from .bindings.mult8.rnn_bindings_generator import rnn_bindings_generator
from .bindings.mult8.softmax_bindings_generator import \
    softmax_bindings_generator as mult8_softmax_bindings_generator
from .bindings.mult8.ssd_postprocess_bindings_generator import \
    ssd_postprocess_bindings_generator
from .bindings.pow2.conv_bindings_generator import \
    conv_bindings_generator as pow2_conv_bindings_generator
from .bindings.pow2.fc_bindings_generator import \
    fc_bindings_generator as pow2_fc_bindings_generator
from .bindings.pow2.inout_bindings_generator import \
    in_out_bindings_generator as pow2_in_out_bindings_generator
from .bindings.pow2.matadd_bindings_generator import \
    matadd_bindings_generator as pow2_matadd_bindings_generator
from .bindings.pow2.matscale_bindings_generator import \
    matscale_bindings_generator as pow2_matscale_bindings_generator
from .bindings.pow2.softmax_bindings_generator import \
    softmax_bindings_generator as pow2_softmax_bindings_generator
from .bindings.pow2.transpose_bindings_generator import \
    transpose_bindings_generator
from .generator_decorators import (RegisteredGeneratorsMixin,
                                   generation_function)
from .globals.constant_input_generator import constant_input_globals_generator
from .globals.filter_generator import filter_globals_generator
from .globals.input_generator import inputs_input_generator
from .globals.mult8_filter_generator import mult8_filter_globals_generator
from .globals.mult8_infos_generator import mult8_infos_generator
from .globals.mult8_rnn_infos_generator import mult8_rnn_infos_generator
from .globals.mult8_ssd_globals_generator import mult8_ssd_globals_generator
from .globals.output_generator import outputs_output_generator
from .kernels.general.imageformat_kernels_generator import \
    imageformat_kernels_generator
from .kernels.mult8.conv_pool_relu_kernels_generator import \
    conv_pool_relu_kernels_generator as mult8_conv_pool_relu_kernels_generator
from .kernels.mult8.copy_kernels_generator import copy_kernel_generator
from .kernels.mult8.global_pool_kernels_generator import \
    global_pool_kernels_generator as mult8_global_pool_kernels_generator
from .kernels.mult8.linear_relu_kernels_generator import \
    linear_relu_kernels_generator
from .kernels.mult8.mat_vect_mult_kernels_generator import \
    mat_vect_mult_kernel_generator
from .kernels.mult8.matadd_kernels_generator import matadd_kernel_generator
from .kernels.mult8.pool_relu_kernels_generator import \
    pool_act_kernels_generator as mult8_pool_act_kernels_generatoor
from .kernels.mult8.rnn_kernels_generator import rnn_kernels_generator
from .kernels.mult8.softmax_kernels_generator import \
    softmax_kernels_generator as mult8_softmax_kernels_generator
from .kernels.mult8.ssd_postprocess_kernels_generator import \
    ssdpostprocess_kernels_generator
from .kernels.mult8.three_d_transpose_kernels_generator import \
    three_d_transpose_kernels_generator as \
    mult8_three_d_transpose_kernels_generator
from .kernels.pow2.conv_pool_relu_kernels_generator import \
    conv_pool_relu_kernels_generator as pow2_conv_pool_relu_kernels_generator
from .kernels.pow2.global_pool_kernels_generator import \
    global_pool_kernels_generator as pow2_global_pool_kernels_generator
from .kernels.pow2.linear_relu_kernels_generator import \
    linear_relu_kernels_generator as pow2_linear_relu_kernels_generator
from .kernels.pow2.matadd_kernels_generator import \
    matadd_kernels_generator as pow2_matadd_kernels_generator
from .kernels.pow2.matscale_kernels_generator import \
    matscale_kernels_generator as pow2_matscale_kernels_generator
from .kernels.pow2.pool_relu_kernels_generator import \
    pool_kernels_generator as pow2_pool_kernels_generator
from .kernels.pow2.softmax_kernels_generator import \
    softmax_kernels_generator as pow2_softmax_kernels_generator
from .kernels.pow2.three_d_transpose_kernels_generator import \
    three_d_transpose_kernels_generator as \
    pow2_three_d_transpose_kernels_generator
