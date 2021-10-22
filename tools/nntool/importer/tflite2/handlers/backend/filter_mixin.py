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


from copy import deepcopy

import numpy as np
from graph.dim import Dim
from importer.common.handler_options import HandlerOptions, handler_option
from importer.tflite2.common import LOG
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.new_qrec import QRec
from quantization.qtype import QType
from utils.node_id import NodeId

from .filter_pad_mixin import FilterPadMixin


@handler_option(
    'rescale_perchannel',
    val_type=bool,
    default=True,
    desc="rescale weight tensors for filters to per channel scaling"
)
class FilterMixin(FilterPadMixin, HandlerOptions):
    TF_LITE_FILT_IN_C = 0
    TF_LITE_FILT_H = 1
    TF_LITE_FILT_W = 2
    TF_LITE_FILT_OUT_C = 3
    TF_LITE_FILTER_ORDER = ['out_c', 'h', 'w', 'in_c']
    # in_c should always be 1 in this case
    # Why does TFLite use this weird order for depthwise?
    TF_LITE_DW_FILTER_ORDER = ['in_c', 'h', 'w', 'out_c']
    TF_LITE_DW_FILTER_TRANSPOSE = [3, 1, 2, 0]

    @classmethod
    def get_weights_qtype_by_channel(cls, filter_shape, out_idx, weights_node):
        assert len(filter_shape) == 4 or len(
            filter_shape) == 2
        dqweights = weights_node.dqvalue
        filter_axis = tuple(
            idx for idx in range(len(filter_shape))
            if idx != out_idx)
        # get the minimums and maximums above and below 0
        w_mins = np.minimum(np.min(dqweights, axis=filter_axis), 0)
        w_maxes = np.maximum(np.max(dqweights, axis=filter_axis), 0)
        wqtype = QType.from_min_max_sq(
            w_mins, w_maxes, quantized_dimension=out_idx, narrow_range=True, scale_zero_as_one=True)
        tiny_weight_scales = wqtype.scale < QType.kInt8NearZeroTolerance
        if np.count_nonzero(tiny_weight_scales):
            # Sets weights scaled under a very small value to zero to avoid
            # silly mult biases.
            shape = tuple(slice(None) if idx !=
                          out_idx else tiny_weight_scales for idx, _ in enumerate(dqweights.shape))
            if np.any(shape):
                dqweights[shape] = 0
                wqtype.scale = np.where(tiny_weight_scales, 1, wqtype.scale)
                weights_node.value = dqweights
                weights_node.qtype = None

        # weights_node.value = wqtype.quantize(dqweights)
        # weights_node.qtype = deepcopy(wqtype)
        return wqtype

    @classmethod
    def get_weights_qtype_by_tensor(cls, weights_node):
        dqweights = weights_node.dqvalue
        w_mins = np.minimum(np.min(dqweights), 0)
        w_maxes = np.maximum(np.max(dqweights), 0)
        wqtype = QType.from_min_max_sq(
            w_mins, w_maxes, narrow_range=True, scale_zero_as_one=True)
        # weights_node.value = wqtype.quantize(dqweights)
        # weights_node.qtype = deepcopy(wqtype)
        return wqtype

    @classmethod
    def new_load_filter_parameters(cls, G, params, filter_shape, filter_scale_axis,
                                   input_tensor, weights_node,
                                   bias_node, output_tensor, opts, dw_to_pw=False):
        weights_node.meta['filter_params'] = True
        bias_node.meta['filter_params'] = True
        # if quantizaton is not loaded then the constants will already be dequantized
        if dw_to_pw:
            # Conv has been converted from depthwise to pointwise so reorder the weights tensor
            weights_node.value = np.transpose(
                weights_node.value, cls.TF_LITE_DW_FILTER_TRANSPOSE)
            weights_node.dims = Dim.unnamed(weights_node.value.shape)
        if not opts.get('load_quantization'):
            return
        wqtype = weights_node.qtype
        if wqtype is None:
            LOG.warning('quantization is missing on node %s', params.name)
            return
        # scale weights as requested. change asymmetric and/or unsigned weights to signed symmetric
        if wqtype.asymmetric or not wqtype.signed:
            if opts.get('rescale_perchannel'):
                wqtype = cls.get_weights_qtype_by_channel(
                    filter_shape, filter_scale_axis, weights_node)
            else:
                wqtype = cls.get_weights_qtype_by_tensor(weights_node)
        else:
            if opts.get('rescale_perchannel'):
                if len(wqtype.scale) != filter_shape[filter_scale_axis]:
                    wqtype = cls.get_weights_qtype_by_channel(
                        filter_shape, filter_scale_axis, weights_node)
            else:
                if len(wqtype.scale) > 1:
                    wqtype = cls.get_weights_qtype_by_tensor(weights_node)

        iqtype = input_tensor.qtype
        # correct input qtype to symmetric tensor scaled
        if iqtype.asymmetric or not iqtype.signed or len(iqtype.scale) > 1:
            iqtype = QType.from_min_max_sq(
                min_val=iqtype.min_val, max_val=iqtype.max_val)
        else:
            iqtype = deepcopy(iqtype)

        oqtype = output_tensor.qtype
        # correct output qtype to symmetric tensor scaled
        if oqtype.asymmetric or not oqtype.signed or len(oqtype.scale) > 1:
            oqtype = QType.from_min_max_sq(
                min_val=oqtype.min_val, max_val=oqtype.max_val)
        else:
            oqtype = deepcopy(oqtype)

        # dqbias = bias_node.dqvalue
        bias_scale = (iqtype.scale * wqtype.scale).astype(np.float32)
        bqtype = QType(dtype=np.int32, scale=bias_scale)
        # NOTE: In some tensorflow graphs the biases are hugely negative or hugely
        # positive. I've never seen this without a relun after and the weights on
        # these channels were 0. Actually they should be pruned.
        # don't overwrite the quantized values since we may move around quantization later
        # bias_node.value = bqtype.quantize(dqbias)
        # bias_node.qtype = bqtype
        if dw_to_pw and wqtype.quantized_dimension:
            wqtype.quantized_dimension = 0

        mulbiases_q = MultMulBiasScaleQType.from_filter(
            iqtype, wqtype, oqtype, params)
        qrec = QRec.scaled(in_qs=[iqtype, wqtype, bqtype],
                           out_qs=[oqtype],
                           calc_q=bqtype,
                           acc_q=bqtype,
                           mul_biases_q=mulbiases_q)
        # now set the quantization records on the node and its constants
        G.quantization[NodeId(params)] = qrec
        G.quantization[NodeId(weights_node)] = QRec.scaled(
            out_qs=[deepcopy(wqtype)])
        G.quantization[NodeId(bias_node)] = QRec.scaled(
            out_qs=[deepcopy(bqtype)])
