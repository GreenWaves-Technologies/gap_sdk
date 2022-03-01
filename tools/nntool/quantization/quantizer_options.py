# Copyright (C) 2021  GreenWaves Technologies, SAS

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

QUANTIZED_DIMENSION_OPTION = {
    'name': 'quantized_dimension',
    'shortcut': 'd',
    'type': str,
    'choices': ['tensor', 'channel'],
    'help': 'scales filter weights by channel or tensor',
    'default': 'channel'
}

NARROW_WEIGHTS_OPTION = {
    'name': 'narrow_weights',
    'shortcut': 'n',
    'type': bool,
    'help': 'scales filter weights with a representation of both 1 and -1 (i.e. -127 - 127 in 8 bits)',
    'default': True
}

MAX_PRECISION_LIMIT_OPTION = {
    'name': 'max_precision_limit',
    'type': int,
    'help': 'maximum number of bits to degrade input scale precision by to stop overflow of accumulator.',
    'default': 2
}

NARROW_STATE_OPTION = {
    'name': 'narrow_state',
    'type': bool,
    'help': 'scales state in Q14 so that 1 <-> -1 can be represented',
    'default': True
}

NE16_WEIGHT_BITS_OPTION = {
    'name': 'weight_bits',
    'type': int,
    'help': 'how many bits to use in weights',
    'choices': list(range(2, 9)),
    'default': 8
}

USE_NE16_OPTION = {
    'name': 'use_ne16',
    'type': bool,
    'help': 'enable use of NE16 kernels (if supported) on this layer',
    'default': False
}

FORCE_NE16_OPTION = {
    'name': 'force_ne16',
    'type': bool,
    'help': 'force use of NE16 kernels on this layer - may not be supported for model generation',
    'default': False
}

ALLOW_ASYMMETRIC_OPTION = {
    'name': 'allow_asymmetric',
    'type': bool,
    'help': 'Allow soft kernels to use asymmetric quantization where possible',
    'default': False
}

FORCE_INPUT_SIZE_OPTION = {
    'name': 'force_input_size',
    'type': int,
    'help': 'number of bits to use for input features',
    'choices': [8, 16],
    'default': 8
}

FORCE_OUTPUT_SIZE_OPTION = {
    'name': 'force_output_size',
    'type': int,
    'help': 'number of bits to use for output features',
    'choices': [8, 16],
    'default': 8
}

FORCE_EXTERNAL_SIZE_OPTION = {
    'name': 'force_external_size',
    'type': int,
    'help': 'bits to use for features and state',
    'choices': [8, 16],
    'default': 8
}

SQBITS_OPTION_DEFAULT_8 = {
    'name': 'sq_bits',
    'type': int,
    'choices': [8],
    'help': 'bits for inputs and outputs of scaled kernels',
    'default': 8
}

BITS_OPTION_DEFAULT_16 = {
    'name': 'bits',
    'shortcut': 'b',
    'type': int,
    'choices': [8, 16],
    'help': 'bits for inputs and outputs',
    'default': 16
}

HWC_OPTION = {
    'name': 'hwc',
    'type': bool,
    'help': 'Use HWC kernel',
    'default': False
}

SOFTMAX_OUT_8BITS_OPTION = {
    'name': 'softmax_out_8bits',
    'type': bool,
    'help': 'make the output scale8 8 bits',
    'default': False
}

FLOAT_MATH_KERNEL_TYPE_OPTION = {
    'name': 'kernel_type',
    'type': str,
    'choices': ['lut', 'fastfloat'],
    'help': 'Kernel to use for activation function',
    'default': 'fastfloat'
}

CLIP_TYPE_OPTION = {
    'name': 'clip_type',
    'type': str,
    'choices': ['laplace', 'gaus', 'mix', 'none'],
    'help': 'Clipping method for filter output activations min max. laplace or gaussian distribution or choose based on MSE or no clipping',
    'default': 'none'
}

BIAS_SIZE_OPTION = {
    'name': 'pow2_biases',
    'type': int,
    'choices': [0, 8, 16, 32],
    'help': 'bits for filter biases - if set to 0 the same size as the output type will be used',
    'default': 0
}

QTYPE_IND_OPTION = {
    'name': 'qtype_ind',
    'type': None,
    'default': None
}
