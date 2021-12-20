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

from generation.at_generators import (NO_ACTIVATION, NO_CONV, NO_POOL,
                                      ConvATParam, GroupedConvATParam,
                                      gen_active_at_params, gen_conv_at_params,
                                      gen_pool_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.generator_decorators import QREC_POW2, generation_function
from graph.dim import PadDim
from graph.types import (ActivationParameters, Conv2DParameters,
                         ConvFusionParameters, PoolingParameters)
from utils.node_id import NodeId

from ..autotiler_kernel import NewAutoTilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels",
                     (Conv2DParameters,
                      ConvFusionParameters,
                      PoolingParameters,
                      ActivationParameters),
                     qrec_types=(QREC_POW2,))
def conv_pool_relu_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    del in_eparams, out_eparams
    if isinstance(node, Conv2DParameters):
        gen.kernels.append(ConvPoolReluKernel(node.name, cname, node, qrec, None,
                                              None, None, None, at_ver=gen.opts['at_ver'],
                                              gen_ctrl=node.get_gen_ctrl()))
    elif isinstance(node, PoolingParameters):
        gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
                                              node, qrec, None, None, at_ver=gen.opts['at_ver'],
                                              gen_ctrl=node.get_gen_ctrl()))
    elif isinstance(node, ActivationParameters):
        # self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
        gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
                                              None, None, node, qrec, at_ver=gen.opts['at_ver'],
                                              gen_ctrl=node.get_gen_ctrl()))
    elif isinstance(node, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if node.fusion_type == "conv_active_pool":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], cnodes[2], quants[2],
                                                  cnodes[1], quants[1], at_ver=gen.opts['at_ver'],
                                                  gen_ctrl=node.get_gen_ctrl()))
        elif node.fusion_type == "conv_pool_active":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1],
                                                  cnodes[2], quants[2], at_ver=gen.opts['at_ver'],
                                                  gen_ctrl=node.get_gen_ctrl()))
        elif node.fusion_type == "conv_active":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], None, None, cnodes[1],
                                                  quants[1], at_ver=gen.opts['at_ver'],
                                                  gen_ctrl=node.get_gen_ctrl()))
        elif node.fusion_type == "conv_pool":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1], None,
                                                  None, at_ver=gen.opts['at_ver'], gen_ctrl=node.get_gen_ctrl()))
        elif node.fusion_type == "pool_active":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None, cnodes[0], quants[0], cnodes[1], quants[1],
                                                  at_ver=gen.opts['at_ver'], gen_ctrl=node.get_gen_ctrl()))
        else:
            return False
    else:
        return False
    return True


class ConvPoolReluKernel(NewAutoTilerKernel):
    CALL_TEMPLATE_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_ConvolutionPoolReLU("{cname}", {gen_ctrl},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {conv_oper}, {conv_w}, {conv_h}, {conv_d_w}, {conv_d_h}, {conv_s_w}, {conv_s_h}, {conv_pad},
                        {pool_oper}, {pool_w}, {pool_h}, {pool_d_w}, {pool_d_h}, {pool_s_w}, {pool_s_h}, {pool_pad},
                        {relu_oper});
'''
    CALL_TEMPLATE_MULBIAS_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_ConvolutionMulBiasPoolReLU("{cname}", {gen_ctrl},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)},
                        {at_bits(mulbias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {mulbias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {conv_oper}, {conv_w}, {conv_h}, {conv_d_w}, {conv_d_h}, {conv_s_w}, {conv_s_h}, {conv_pad},
                        {pool_oper}, {pool_w}, {pool_h}, {pool_d_w}, {pool_d_h}, {pool_s_w}, {pool_s_h}, {pool_pad},
                        {relu_oper});
'''
    CALL_TEMPLATE_GROUPED_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_GroupedConvolutionPoolReLU("{cname}", {gen_ctrl},
                        {group_in}, {group_out},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {conv_oper}, {conv_w}, {conv_h}, {conv_d_w}, {conv_d_h}, {conv_s_w}, {conv_s_h}, {conv_pad},
                        {pool_oper}, {pool_w}, {pool_h}, {pool_d_w}, {pool_d_h}, {pool_s_w}, {pool_s_h}, {pool_pad},
                        {relu_oper});
'''
    CALL_TEMPLATE_GROUPED_MULBIAS_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_GroupedConvolutionMulBiasPoolReLU("{cname}", {gen_ctrl},
                        {group_in}, {group_out},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)},
                        {at_bits(mulbias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {mulbias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {conv_oper}, {conv_w}, {conv_h}, {conv_d_w}, {conv_d_h}, {conv_s_w}, {conv_s_h}, {conv_pad},
                        {pool_oper}, {pool_w}, {pool_h}, {pool_d_w}, {pool_d_h}, {pool_s_w}, {pool_s_h}, {pool_pad},
                        {relu_oper});
'''
    CALL_TEMPLATE_POOL_RELU = '''
// generator for {node_name}
CNN_PoolReLU("{cname}", {gen_ctrl},
              {at_bits(in_qtype)}, {at_bits(out_qtype)},
              {in_qtype.q}, {out_qtype.q},
              1, 1, {in_c}, {out_c}, {in_w}, {in_h},
              {pool_oper}, {pool_w}, {pool_h}, {pool_d_w}, {pool_d_h}, {pool_s_w}, {pool_s_h}, {pool_pad},
              {relu_oper});
'''

    def __init__(self, node_name, cname, conv_params, conv_q,
                 pool_params, pool_q, act_params, act_q, at_ver=3, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_q = filter_q = out_q = bias_q = mul_biases_q = None
        in_dim = out_dim = None
        pad_compatibilities = []
        if conv_params is not None:
            at_conv_params = gen_conv_at_params(
                conv_params, conv_q, pad_compatibilities)
            in_dim = conv_params.in_dims[0]
            out_dim = conv_params.out_dims[0]
            filter_q = conv_q.in_qs[1]
            in_q = conv_q.in_qs[0]
            out_q = conv_q.out_qs[0]
            bias_q = conv_q.in_qs[2]
            if conv_params.has_mul_bias:
                mul_biases_q = conv_q.mul_biases_q
        else:
            at_conv_params = NO_CONV

        if pool_params is not None:
            at_pool_params = gen_pool_at_params(
                pool_params, pad_compatibilities)
            if in_dim is None:
                in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
            if in_q is None:
                in_q = pool_q.in_qs[0]
            out_q = pool_q.out_qs[0]
        else:
            at_pool_params = NO_POOL

        if act_params is not None:
            at_act_params = gen_active_at_params(act_params)
            if in_dim is None:
                in_dim = act_params.in_dims[0]
            if out_dim is None:
                out_dim = act_params.out_dims[0]
            if in_q is None:
                in_q = act_q.in_qs[0]
            out_q = act_q.out_qs[0]
            if at_ver < 3:
                if act_params.activation == "relu6" and out_q.q != 0:
                    gen_ctrl.ReluN = 6 << out_q.q
                    gen_ctrl.ReluNNoNorm = 1
            else:
                if act_params.activation == "relun":
                    gen_ctrl.ReluN = act_params.activation_params

        else:
            at_act_params = NO_ACTIVATION

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug("%s: generating pad control block", node_name)
                gen_ctrl.PadType = at_pad_ctrl

        attrs = {
            'in_qtype': in_q,
            'out_qtype': out_q,
            'filter_qtype': filter_q,
            'bias_qtype': bias_q,
            'mul_biases_qtype': mul_biases_q,
            'relu_oper': at_act_params.ReLUOper
        }

        if at_pool_params.PoolOper != 'KOP_NONE':
            attrs.update({
                'pool_oper': at_pool_params.PoolOper,
                'pool_w': at_pool_params.Fpx,
                'pool_h': at_pool_params.Fpy,
                'pool_d_w': at_pool_params.Dpx,
                'pool_d_h': at_pool_params.Dpy,
                'pool_s_w': at_pool_params.Spx,
                'pool_s_h': at_pool_params.Spy,
                'pool_pad': at_pool_params.PoolPad
            })
        else:
            attrs.update({
                'pool_oper': 'KOP_NONE',
                'pool_w': 0,
                'pool_h': 0,
                'pool_d_w': 0,
                'pool_d_h': 0,
                'pool_s_w': 0,
                'pool_s_h': 0,
                'pool_pad': 0
            })

        if at_conv_params == NO_CONV:
            if in_q.dtype_bits != out_q.dtype_bits:
                raise NotImplementedError(
                    "only homogenious operations are supported at present")
            LOG.debug("%s: pool relu inq %s outq %s control block",
                      node_name, in_q, out_q)
            if at_pool_params.PoolOper == 'KOP_NONE' and (not in_dim.is_named or not in_dim.has_keys(['c', 'w', 'h'])):
                in_shape = in_dim.shape + ([1] * (3 - len(in_dim.shape)))
                in_c, in_h, in_w = in_shape[0], in_shape[1], in_shape[2]
            else:
                in_c, in_h, in_w = in_dim.c, in_dim.h, in_dim.w
            if out_dim.is_named and out_dim.has_key('c'):
                out_c = out_dim.c
            else:
                out_c = in_c
            attrs.update({
                'in_c': in_c,
                'in_h': in_h,
                'in_w': in_w,
                'out_c': out_c,
                'conv_oper': 'KOP_NONE'
            })
            self.template = 'CALL_TEMPLATE_POOL_RELU'
        else:
            # swap w and h if w and filter w is 1 so generator sees 1D conv
            if in_dim.w == 1 and at_conv_params.Fcx == 1:
                attrs.update({
                    'in_c': in_dim.c,
                    'in_h': 1,
                    'in_w': in_dim.h,
                    'out_c': out_dim.c,
                    'conv_oper': at_conv_params.ConvOper,
                    'conv_w': at_conv_params.Fcy,
                    'conv_h': 1,
                    'conv_d_w': at_conv_params.Dcy,
                    'conv_d_h': at_conv_params.Dcx,
                    'conv_s_w': at_conv_params.Scy,
                    'conv_s_h': at_conv_params.Scx,
                    'conv_pad': at_conv_params.ConvPad

                })
            else:
                attrs.update({
                    'in_c': in_dim.c,
                    'in_h': in_dim.h,
                    'in_w': in_dim.w,
                    'out_c': out_dim.c,
                    'conv_oper': at_conv_params.ConvOper,
                    'conv_w': at_conv_params.Fcx,
                    'conv_h': at_conv_params.Fcy,
                    'conv_d_w': at_conv_params.Dcx,
                    'conv_d_h': at_conv_params.Dcy,
                    'conv_s_w': at_conv_params.Scx,
                    'conv_s_h': at_conv_params.Scy,
                    'conv_pad': at_conv_params.ConvPad
                })
            if isinstance(at_conv_params, ConvATParam):
                if mul_biases_q is not None:
                    LOG.debug("%s: mulconv pool relu inq %s outq %s control block",
                              node_name, in_q, out_q)
                    self.template = 'CALL_TEMPLATE_MULBIAS_CONV_POOL_RELU'
                else:
                    LOG.debug("%s: conv pool relu inq %s outq %s control block",
                              node_name, in_q, out_q)
                    self.template = 'CALL_TEMPLATE_CONV_POOL_RELU'
            elif isinstance(at_conv_params, GroupedConvATParam):
                attrs.update({
                    'group_in': at_conv_params.GroupIn,
                    'group_out': at_conv_params.GroupOut
                })
                if mul_biases_q is not None:
                    LOG.debug("%s: grouped conv pool relu inq %s outq %s control block",
                              node_name, in_q, out_q)
                    self.template = 'CALL_TEMPLATE_GROUPED_MULBIAS_CONV_POOL_RELU'
                else:
                    LOG.debug("%s: grouped mulconv pool relu inq %s outq %s control block",
                              node_name, in_q, out_q)
                    self.template = 'CALL_TEMPLATE_GROUPED_CONV_POOL_RELU'
            else:
                raise ValueError('Internal error')

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)

    def get_template(self):
        return getattr(self, self.template)
