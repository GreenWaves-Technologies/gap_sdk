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


def quantization_options(
    clip_type: str='none',
    float_type: str='float32',
    kernel_type: str='fastfloat',
    hwc: bool=False,
    sq_bits: int=8,
    force_output_size: int=8,
    weight_bits: int=8,
    force_external_size: int=8,
    narrow_weights: bool=True,
    use_ne16: bool=False,
    narrow_state: bool=True,
    max_precision_limit: int=2,
    force_rnn_1_minus_1_out: bool=False,
    quantized_dimension: str='channel',
    force_ne16: bool=False,
    allow_asymmetric_out: bool=True,
    force_input_size: int=8,
    softmax_out_8bits: bool=False,
    bits: int=16,
    pow2_biases: int=0
):
    """ Create quantization options

    Utility method to create quantization options dictionary.

    Args:
        clip_type (str):
            Clipping method for any node that modifies its input:
            none - the minimum and maximum observed values are used.
            laplace, gaus - Values chosen based on laplace or gaussian distribution
            mix - MSE is used to estimate if distribution is laplace or gaussian
            std3, std5 - 3 or 5 times standard deviation from mean

            Default is 'none'.
        float_type (str):
            float type to use for quantization

            Default is 'float32'.
        kernel_type (str):
            Kernel to use for activation function

            Default is 'fastfloat'.
        hwc (bool):
            Use HWC kernel

            Default is False.
        sq_bits (int):
            bits for inputs and outputs of scaled kernels

            Default is 8.
        force_output_size (int):
            number of bits to use for output features

            Default is 8.
        weight_bits (int):
            how many bits to use in weights

            Default is 8.
        force_external_size (int):
            bits to use for features and state

            Default is 8.
        narrow_weights (bool):
            scales filter weights with a representation of both 1 and -1 (i.e. -127 - 127 in 8 bits)

            Default is True.
        use_ne16 (bool):
            enable use of NE16 kernels (if supported) on this layer

            Default is False.
        narrow_state (bool):
            scales state in Q14 so that 1 <-> -1 can be represented

            Default is True.
        max_precision_limit (int):
            maximum number of bits to degrade input scale precision by to stop overflow of accumulator.

            Default is 2.
        force_rnn_1_minus_1_out (bool):
            forces RNNs to have narrow, symmetric 1 - -1 output

            Default is False.
        quantized_dimension (str):
            scales filter weights by channel or tensor

            Default is 'channel'.
        force_ne16 (bool):
            force use of NE16 kernels on this layer - may not be supported for model generation

            Default is False.
        allow_asymmetric_out (bool):
            Allow soft kernels to use asymmetric quantization where possible. This option defaults to False on inputs and outputs except for inputs if their minimum value in the statistics is 0 in which case it is always True.

            Default is True.
        force_input_size (int):
            number of bits to use for input features

            Default is 8.
        softmax_out_8bits (bool):
            make the output scale8 8 bits

            Default is False.
        bits (int):
            bits for inputs and outputs

            Default is 16.
        pow2_biases (int):
            bits for filter biases - if set to 0 the same size as the output type will be used

            Default is 0.
    """
    return {
        "clip_type": clip_type,
        "float_type": float_type,
        "kernel_type": kernel_type,
        "hwc": hwc,
        "sq_bits": sq_bits,
        "force_output_size": force_output_size,
        "weight_bits": weight_bits,
        "force_external_size": force_external_size,
        "narrow_weights": narrow_weights,
        "use_ne16": use_ne16,
        "narrow_state": narrow_state,
        "max_precision_limit": max_precision_limit,
        "force_rnn_1_minus_1_out": force_rnn_1_minus_1_out,
        "quantized_dimension": quantized_dimension,
        "force_ne16": force_ne16,
        "allow_asymmetric_out": allow_asymmetric_out,
        "force_input_size": force_input_size,
        "softmax_out_8bits": softmax_out_8bits,
        "bits": bits,
        "pow2_biases": pow2_biases
    }
