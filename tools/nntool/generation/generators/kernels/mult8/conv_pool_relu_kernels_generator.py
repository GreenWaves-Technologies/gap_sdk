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

from generation.at_types.at_params import (NO_ACTIVATION, NO_CONV, NO_POOL,
                                           ConvATParam, GroupedConvATParam,
                                           gen_active_at_params, gen_conv_at_params,
                                           gen_pool_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.dim import PadDim
from graph.types import (ActivationParameters, Conv2DParameters,
                         ConvFusionParameters, PoolingParameters)
from utils.node_id import NodeId

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels",
                     (Conv2DParameters,
                      ConvFusionParameters,
                      ActivationParameters),
                     qrec_types=(QREC_MULT8,))
def conv_pool_relu_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    del in_eparams, out_eparams
    if isinstance(node, Conv2DParameters):
        gen.kernels.append(ConvPoolReluKernel(node.name, cname, node, qrec, None,
                                              None, None, None, at_ver=gen.opts['at_ver'],
                                              gen_ctrl=node.get_gen_ctrl(), force_relu=gen.force_relu))
    # We want to match the pool_act generator for PoolingParameters
    # elif isinstance(node, PoolingParameters):
    #     gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
    #                                           node, qrec, None, None, at_ver=gen.opts['at_ver'],
    #                                           gen_ctrl=node.get_gen_ctrl()))
    elif isinstance(node, ActivationParameters):
        # self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
        gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
                                              None, None, node, qrec, at_ver=gen.opts['at_ver'],
                                              gen_ctrl=node.get_gen_ctrl(), force_relu=gen.force_relu))
    elif isinstance(node, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if node.fusion_type == "conv_active_pool":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], cnodes[2], quants[2],
                                                  cnodes[1], quants[1], at_ver=gen.opts['at_ver'],
                                                  gen_ctrl=node.get_gen_ctrl(), force_relu=gen.force_relu))
        elif node.fusion_type == "conv_pool_active":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1],
                                                  cnodes[2], quants[2], at_ver=gen.opts['at_ver'],
                                                  gen_ctrl=node.get_gen_ctrl(), force_relu=gen.force_relu))
        elif node.fusion_type == "conv_active":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], None, None, cnodes[1],
                                                  quants[1], at_ver=gen.opts['at_ver'],
                                                  gen_ctrl=node.get_gen_ctrl(), force_relu=gen.force_relu))
        elif node.fusion_type == "conv_pool":
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1], None,
                                                  None, at_ver=gen.opts['at_ver'], gen_ctrl=node.get_gen_ctrl(),
                                                  force_relu=gen.force_relu))
        else:
            return False
    else:
        return False
    return True

def gen_cnn_conv_pool_act_qs8(code_block, cname,
                              in_feat, out_feat, width, height, bias_size,
                              conv_oper, fcx, fcy, dcx, dcy, scx, scy, conv_pad,
                              pool_oper, fpx, fpy, dpx, dpy, spx, spy, pool_pad,
                              act_oper, gen_ctrl, at_ver=3):
    del at_ver
    code_block.write('CNN_ConvolutionPoolAct_SQ8("{}", {}, {}, {}, {}, {}, {}, {},',
                     cname,
                     gen_ctrl,
                     bias_size,
                     1,
                     in_feat,
                     out_feat,
                     width,
                     height)
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     conv_oper, fcx, fcy, dcx, dcy, scx, scy, conv_pad)
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     pool_oper, fpx, fpy, dpx, dpy, spx, spy, pool_pad)
    code_block.write('{});', act_oper)
    code_block.deindent()


def gen_cnn_grp_conv_pool_act_qs8(code_block, cname,
                                  grp_in, grp_out, in_feat, out_feat, width, height, bias_size,
                                  conv_oper, fcx, fcy, dcx, dcy, scx, scy, conv_pad,
                                  pool_oper, fpx, fpy, dpx, dpy, spx, spy, pool_pad,
                                  act_oper, gen_ctrl, at_ver=3):
    del at_ver
    code_block.write('CNN_GroupedConvolutionPoolAct_SQ8("{}", {}, {}, {}, {}, {}, {}, {}, {}, {},',
                     cname,
                     gen_ctrl,
                     grp_in,
                     grp_out,
                     bias_size,
                     1,
                     in_feat,
                     out_feat,
                     width,
                     height)
    code_block.indent()
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     conv_oper, fcx, fcy, dcx, dcy, scx, scy, conv_pad)
    code_block.write('{}, {}, {}, {}, {}, {}, {}, {},',
                     pool_oper, fpx, fpy, dpx, dpy, spx, spy, pool_pad)
    code_block.write('{});', act_oper)
    code_block.deindent()


class ConvPoolReluKernel(AutotilerKernel):
    def __init__(self, node_name, cname, conv_params, conv_q,
                 pool_params, pool_q, act_params, act_q, at_ver=3,
                 gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        in_q = filter_q = out_q = bias_q = mul_biases_q = None
        in_dim = out_dim = None
        pad_compatibilities = []
        if conv_params is not None:
            at_conv_params = gen_conv_at_params(conv_params, pad_compatibilities)
            in_dim = conv_params.in_dims[0]
            out_dim = conv_params.out_dims[0]
            # Set ENABLEIM2COL on 1x1 filters by default
            if conv_params.filter.h == 1 and conv_params.filter.w == 1 and gen_ctrl.enableim2col is None:
                gen_ctrl.enableim2col = 1
            filter_q = conv_q.weights_q
            in_q = conv_q.in_qs[0]
            out_q = conv_q.out_qs[0]
            bias_q = conv_q.biases_q
            if conv_params.has_mul_bias:
                mul_biases_q = conv_q.mul_biases_q
        else:
            at_conv_params = NO_CONV

        if pool_params is not None:
            at_pool_params = gen_pool_at_params(pool_params, pad_compatibilities)
            if in_dim is None:
                in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
            if in_q is None:
                in_q = pool_q.in_qs[0]
            out_q = pool_q.out_qs[0]
        else:
            at_pool_params = NO_POOL

        if act_params is not None:
            at_act_params = gen_active_at_params(act_params, force_relu=force_relu)
            if in_dim is None:
                in_dim = act_params.in_dims[0]
            if out_dim is None:
                out_dim = act_params.out_dims[0]
            if in_q is None:
                in_q = act_q.in_qs[0]
            out_q = act_q.out_qs[0]

        else:
            at_act_params = NO_ACTIVATION

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug("%s: generating pad control block", node_name)
                self.gen_ctrl.PadType = at_pad_ctrl
        self.in_dim = in_dim
        self.out_dim = out_dim
        self.in_q = in_q
        self.bias_q = bias_q
        self.out_q = out_q
        self.filter_q = filter_q
        self.mul_biases_q = mul_biases_q
        self.at_act_params = at_act_params
        self.at_pool_params = at_pool_params
        self.at_conv_params = at_conv_params
        self.cname = cname
        self.node_name = node_name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)
            gen_ctrl = self.gen_ctrl.ctrl_name
        else:
            gen_ctrl = "0"

        if self.at_conv_params == NO_CONV:
            pp = self.at_pool_params
            ap = self.at_act_params
            gen_cnn_conv_pool_act_qs8(code_block, self.cname, self.in_dim.c,
                                      self.out_dim.c, self.in_dim.w, self.in_dim.h,
                                      self.bias_q.bits//8 if self.bias_q is not None else 0,
                                      "KOP_NONE", 0, 0, 0, 0, 0, 0, 0,
                                      pp.PoolOper, pp.Fpx, pp.Fpy, pp.Dpx, pp.Dpy,
                                      pp.Spx, pp.Spy, pp.PoolPad,
                                      ap.ReLUOper, gen_ctrl,
                                      at_ver=self.at_ver)
        else:
            cp = self.at_conv_params
            pp = self.at_pool_params
            ap = self.at_act_params
            if isinstance(self.at_conv_params, ConvATParam):
                LOG.debug("%s: conv pool relu inq %s outq %s control block",
                          self.node_name, self.in_q, self.out_q)
                gen_cnn_conv_pool_act_qs8(code_block, self.cname, self.in_dim.c,
                                          self.out_dim.c, self.in_dim.w, self.in_dim.h,
                                          self.bias_q.bits//8,
                                          cp.ConvOper, cp.Fcx, cp.Fcy, cp.Dcx, cp.Dcy,
                                          cp.Scx, cp.Scy, cp.ConvPad,
                                          pp.PoolOper, pp.Fpx, pp.Fpy, pp.Dpx, pp.Dpy,
                                          pp.Spx, pp.Spy, pp.PoolPad,
                                          ap.ReLUOper, gen_ctrl,
                                          at_ver=self.at_ver)
            elif isinstance(self.at_conv_params, GroupedConvATParam):
                LOG.debug("%s: grouped mulconv pool relu inq %s outq %s control block",
                          self.node_name, self.in_q, self.out_q)
                gen_cnn_grp_conv_pool_act_qs8(code_block, self.cname, cp.GroupIn, cp.GroupOut,
                                              self.in_dim.c,
                                              self.out_dim.c, self.in_dim.w, self.in_dim.h,
                                              self.bias_q.bits//8,
                                              cp.ConvOper, cp.Fcx, cp.Fcy, cp.Dcx, cp.Dcy,
                                              cp.Scx, cp.Scy, cp.ConvPad,
                                              pp.PoolOper, pp.Fpx, pp.Fpy, pp.Dpx, pp.Dpy,
                                              pp.Spx, pp.Spy, pp.PoolPad,
                                              ap.ReLUOper, gen_ctrl,
                                              at_ver=self.at_ver)
            else:
                raise ValueError('Internal error')

        return code_block
