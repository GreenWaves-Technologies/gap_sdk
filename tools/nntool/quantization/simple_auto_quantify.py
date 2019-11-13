# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from collections import OrderedDict

from graph.nngraph import NNGraph
from graph.types import (Conv2DParameters, FcParameters, FusionParameters,
                         InputParameters, Parameters, SoftMaxParameters)
from utils.node_id import NodeId
from utils.stats_funcs import STATS_BITS, bits, closest_greater

from .qtype import QType
from .quantization_record import FilterQuantizationRecord, QuantizationRecord
from .quantizer import Quantizer


class SimpleQuantizer(Quantizer):
    def __init__(self, activation_stats, filter_stats, min_qsnr=None, force_width=None):
        self._activation_stats = activation_stats
        self._filter_stats = filter_stats
        self._min_qsnr = min_qsnr
        self._force_width = force_width

    # pylint: disable=too-many-locals
    def calculate_filter_q(self, node: Parameters, astats, fstats,
                           in_q: QType, min_qsnr, force_width,
                           acc_as_calc=False, bias_as_out=True):
        fstats = node.stats
        w_q = self.get_quantization(fstats['weights'], min_qsnr, force_width)
        o_q = self.get_quantization(astats, min_qsnr, force_width)
        calc_width = closest_greater(in_q.bits + w_q.bits)
        calc_q = in_q.q + w_q.q

        acc_bits = bits(astats['max_acc'], astats['min_acc'])
        act_bits = bits(astats['max'], astats['min'])
        act_acc_bits = max(acc_bits, act_bits)

        calc_int_bits = calc_width - calc_q
        if calc_int_bits < act_acc_bits:
            # we don't have enough space for the integer portion so reduce the precision of
            # the weights
            missing_bits = act_acc_bits - calc_int_bits
            # TODO - This needs improving
            assert w_q.q >= missing_bits, "no space in weights to reduce precision"
            w_q.q = w_q.q - missing_bits
            calc_q = in_q.q + w_q.q

        c_q = QType(bits=calc_width, q=calc_q, signed=True)

        if 'biases' in fstats:
            b_q = self.get_quantization(fstats['biases'], min_qsnr, force_width)
            if bias_as_out:
                o_q.q = min(b_q.q, o_q.q)
                b_q.q = o_q.q
        else:
            b_q = o_q.q

        if acc_as_calc or acc_bits > o_q.bits - o_q.q:
            acc_q = c_q
        else:
            acc_q = o_q

        norm = c_q.q - o_q.q

        node.quantization = {"input_q": in_q, "weights_q": w_q,
                             "biases_q": b_q, "norm": norm, "calc_q": c_q,
                             "acc_q": acc_q}
        return FilterQuantizationRecord(in_qs=[in_q], out_qs=[o_q], calc_q=c_q,
                                        acc_q=acc_q, biases_q=b_q, weights_q=w_q)

    @staticmethod
    def get_quantization(stats, min_qsnr, force_width):
        qstats = stats['qstats']
        if force_width is not None:
            return QType(bits=force_width,
                         q=qstats[force_width]['q'],
                         signed=True)
        for width in STATS_BITS:
            if qstats[width]['qsnr'] > min_qsnr:
                return QType(bits=width,
                             q=qstats[width]['q'],
                             signed=True)
        raise ValueError("no solution for this QSNR could be found")

    def calculate_q(self, node, astats, fstats, in_qs, min_qsnr, force_width):
        if isinstance(node, InputParameters):
            qreq = QuantizationRecord(in_qs=in_qs,
                                      out_qs=[self.get_quantization(astats, min_qsnr, force_width)])
        elif isinstance(node, Conv2DParameters):
            qreq = self.calculate_filter_q(node, astats, fstats, in_qs[0], min_qsnr, force_width)
        elif isinstance(node, FcParameters):
            qreq = self.calculate_filter_q(node, astats, fstats, in_qs[0], min_qsnr, force_width,
                                           acc_as_calc=True)
             # TODO - put back when fixed, bias_as_out=False)
        elif isinstance(node, SoftMaxParameters):
            # softmax always outputs Q15
            qreq = QuantizationRecord(in_qs=in_qs, out_qs=[QType(16, 15, True)])
        else:
            qreq = QuantizationRecord(in_qs=in_qs, out_qs=in_qs)
        return qreq

    def quantize(self, G: NNGraph) -> OrderedDict:
        edge_recs = {}
        result = OrderedDict()
        for step in G.graph_state.steps:
            node = step['node']
            if isinstance(node, InputParameters):
                in_qs = []
            else:
                in_qs = [edge_recs[edge.params]
                         for edge in G.indexed_in_edges(node.name)]
            if isinstance(node, FusionParameters):
                fin_qs = in_qs
                for fnode in node.contained_nodes():
                    qrec = self.calculate_q(
                        fnode,
                        self._activation_stats.get(NodeId(node, fnode)),
                        self._filter_stats.get(NodeId(node, fnode)),
                        fin_qs,
                        self._min_qsnr,
                        self._force_width)
                    result[NodeId(node, fnode)] = qrec
                    fin_qs = qrec.out_qs
                qrec = QuantizationRecord(in_qs=in_qs, out_qs=fin_qs)
            else:
                qrec = self.calculate_q(
                    node,
                    self._activation_stats.get(NodeId(node, None)),
                    self._filter_stats.get(NodeId(node, None)),
                    in_qs,
                    self._min_qsnr,
                    self._force_width)
            result[NodeId(node, None)] = qrec
            if not qrec:
                break

            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]
        return result
