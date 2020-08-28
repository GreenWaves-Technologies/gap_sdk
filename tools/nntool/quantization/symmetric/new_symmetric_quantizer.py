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
from collections import OrderedDict

from graph.nngraph import NNGraph
from graph.types import (ActivationParameters, ConcatParameters,
                         ConstantInputParameters, Conv2DParameters,
                         ConvFusionParameters, FcParameters, InputParameters,
                         MatrixBroadcastedLinearOpParameters,
                         MatScaleFusionParameters,
                         MultiplicativeBiasParameters, Parameters,
                         SoftMaxParameters)
from quantization.qtype import QType
from quantization.quantizer import Quantizer
from quantization.quantization_set import QuantizationSet
from quantization.symmetric.symmetric_quantization import (
    SymmetricFilterQuantizationRecord, SymmetricQuantizationRecord,
    SymmetricScalableFilterQuantizationRecord)
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys
from utils.stats_funcs import bits, calc_bits

LOG = logging.getLogger('nntool.' + __name__)


class SymmetricQuantizer(Quantizer, JsonSerializable):
    def __init__(self, activation_stats, force_width=None):
        self._activation_stats = activation_stats
        self._force_width = force_width

    # for tests
    def __eq__(self, value):
        return self._activation_stats == value._activation_stats and \
            self._force_width == value._force_width

    def _encapsulate(self):
        return {
            'activation_stats': convert_keys_to_str(self._activation_stats),
            'force_width': self._force_width
        }

    @classmethod
    def _dencapsulate(cls, val):
        return SymmetricQuantizer(convert_str_to_keys(val['activation_stats']),
                                  val['force_width'])

    # pylint: disable=too-many-locals
    def calculate_filter_q(self,
                           node: Parameters,
                           astats,
                           in_q: QType,
                           force_width=None,
                           force_out=None,
                           out_as_acc=False):
                           #biases_bits_as_acc=False):

        w_q = QType.from_array(arr=node.weights, bits=force_width, signed=True)

        calc_width = 32
        calc_q = in_q.q + w_q.q

        acc_bits = bits(astats['range_acc']['max'], astats['range_acc']['min'])
        act_bits = bits(astats['range_out'][0]['max'], astats['range_out'][0]['min'])
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
            calc_int_bits = calc_width - calc_q

        c_q = acc_q = QType(bits=calc_width, q=calc_q, signed=True)

        if out_as_acc:
            o_q = c_q
            if node.has_bias:
                b_q = QType.from_array(node.biases, bits=force_width, signed=True)
        else:
            # The output size is requested to be force_out_width size
            if force_out and force_out.bits:
                # The output fixed point position is also forced
                if force_out.q:
                    if (force_out.bits - force_out.q) < act_acc_bits:
                        # clipping so cannot completely satisfy
                        o_q = QType(bits=force_out.bits,
                                    q=force_out.bits - act_acc_bits,
                                    signed=True)
                    else:
                        if force_out.q > calc_q:
                            # We cannot shift left in the kernel
                            # TODO - This should try to increase the input q
                            # Unlikely to happen
                            raise NotImplementedError()
                        # We can satisfy the force
                        o_q = QType(bits=force_out.bits,
                                    q=force_out.q,
                                    signed=True)
                else:
                    # Only the width is forced
                    o_q = QType.from_min_max(astats['range_out'][0]['max'], astats['range_out'][0]['min'],
                                             bits=force_out.bits)
            else:
                # The output width is not forced so calculate the output q normally
                o_q = QType.from_min_max(astats['range_out'][0]['max'], astats['range_out'][0]['min'],
                                         bits=force_width)
                if force_out and force_out.q:
                    # The output fixed point position is forced
                    if force_out.q > calc_q:
                        # We cannot shift left in the kernel
                        # TODO - This should try to increase the input q
                        # Unlikely to happen
                        raise NotImplementedError()
                    o_q.q = force_out.q

            if node.has_bias:
                b_q = o_q
        # make sure that the biases are not stored more precisily than the accumulator. It's pointless and will
        # cause a negative shift
        if b_q.q > acc_q.q:
            b_q.q = acc_q.q

        if isinstance(node, MultiplicativeBiasParameters):
            if node.has_mul_bias:
                mb_q = QType.from_array(node.mul_biases, bits=force_width)
            else:
                mb_q = None
            qrec = SymmetricScalableFilterQuantizationRecord(in_qs=[in_q], out_qs=[o_q], calc_q=c_q,
                                                             acc_q=acc_q, biases_q=b_q, weights_q=w_q,
                                                             mul_biases_q=mb_q,
                                                             constants_are_quantized=False)
        else:
            qrec = SymmetricFilterQuantizationRecord(in_qs=[in_q], out_qs=[o_q], calc_q=c_q,
                                                     acc_q=acc_q, biases_q=b_q, weights_q=w_q,
                                                     constants_are_quantized=False)

        LOG.debug("filter %s qrec %s", node.name, qrec)
        return qrec

    # pylint: disable=too-many-locals
    def calculate_output_q(self,
                           node: Parameters,
                           astats,
                           in_qs,
                           force_width=None,
                           force_out=None):
        del node
        if force_out:
            if force_out.bits:
                if force_out.q:
                    o_q = QType(bits=force_out.bits,
                                q=force_out.q,
                                signed=True)
                else:
                    o_q = QType.from_min_max(max_val=astats['range_out'][0]['max'],
                                             min_val=astats['range_out'][0]['min'],
                                             bits=force_out.bits)
            elif force_out.q:
                o_q = QType.from_min_max(max_val=astats['range_out'][0]['max'],
                                         min_val=astats['range_out'][0]['min'],
                                         bits=force_width)
                o_q.q = force_out.q
        else:
            o_q = QType.from_min_max(max_val=astats['range_out'][0]['max'],
                                     min_val=astats['range_out'][0]['min'],
                                     bits=force_width)
        return SymmetricQuantizationRecord(in_qs=in_qs,
                                           out_qs=[o_q])

    def calculate_q(self,
                    node,
                    astats,
                    in_qs,
                    force_width,
                    force_out=None):

        if isinstance(node, (InputParameters, MatrixBroadcastedLinearOpParameters,
                             ConstantInputParameters, MatScaleFusionParameters)):
            qrec = self.calculate_output_q(node,
                                           astats,
                                           in_qs,
                                           force_width=force_width,
                                           force_out=force_out)
        elif isinstance(node, Conv2DParameters):
            qrec = self.calculate_filter_q(node,
                                           astats,
                                           in_q=in_qs[0],
                                           force_width=force_width,
                                           force_out=force_out)
        elif isinstance(node, FcParameters):
            qrec = self.calculate_filter_q(node,
                                           astats,
                                           in_q=in_qs[0],
                                           force_width=force_width,
                                           force_out=force_out)
        elif isinstance(node, SoftMaxParameters):
            # softmax always outputs Q15
            qrec = SymmetricQuantizationRecord(in_qs=in_qs, out_qs=[QType(16, 15, True)])
        elif isinstance(node, ActivationParameters):
            qrec = SymmetricQuantizationRecord(in_qs=in_qs,
                                               out_qs=[self.compute_activation_out_qtype(node, in_qs[0])])
        else:
            qrec = SymmetricQuantizationRecord(in_qs=in_qs, out_qs=in_qs)
        return qrec

    @staticmethod
    def compute_activation_out_maxq(node, num_bits):
        relun = None
        if node.activation == "relu6":
            relun = 6
        elif node.activation == "relun":
            relun = node.activation_params
            if isinstance(relun, list):
                relun = max(relun)
        if relun is None:
            return None
        relu_bits = calc_bits(relun)
        return num_bits - relu_bits

    def compute_activation_out_qtype(self, node, in_q):
        max_q = self.compute_activation_out_maxq(node, in_q.bits)
        if max_q is None:
            return in_q

        return QType(bits=in_q.bits,
                     q=min(in_q.q, max_q),
                     signed=True)

    def default_quantize_fusion(self,
                                G: NNGraph,
                                node: ConvFusionParameters,
                                in_qs,
                                force_out=None) -> SymmetricQuantizationRecord:
        del G
        result = OrderedDict()
        fin_qs = in_qs
        for fnode in node.contained_nodes():
            qrec = self.calculate_q(
                fnode,
                self._activation_stats.get(NodeId(node, fnode)),
                fin_qs,
                self._force_width,
                force_out=force_out)
            result[NodeId(node, fnode)] = qrec
            fin_qs = qrec.out_qs
        return SymmetricQuantizationRecord(in_qs=in_qs, out_qs=fin_qs), result

    def quantize_fusion(self,
                        G: NNGraph,
                        node: ConvFusionParameters,
                        in_qs,
                        force_out=None) -> SymmetricQuantizationRecord:
        if node.fusion_type == 'conv_active':
            result = OrderedDict()
            nodes = node.contained_nodes()
            conv_node = nodes[0]
            conv_astats = self._activation_stats.get(NodeId(node, conv_node))
            conv_qrec = self.calculate_filter_q(conv_node,
                                                conv_astats,
                                                in_q=in_qs[0],
                                                force_width=self._force_width,
                                                out_as_acc=True)
            result[NodeId(node, conv_node)] = conv_qrec
            act_node = nodes[1]
            act_astats = self._activation_stats.get(NodeId(node, act_node))
            if force_out and force_out.bits:
                act_max_q = self.compute_activation_out_maxq(act_node, force_out.bits)
                if force_out.q is not None:
                    if (act_max_q is not None and force_out.q > act_max_q) or force_out.q > conv_qrec.out_qs[0].q:
                        # We cannot shift left in the kernel
                        # TODO - This should try to increase the input q and perhaps the width
                        # Unlikely to happen
                        raise NotImplementedError()
                    act_o_q = QType(bits=force_out.bits,
                                    q=force_out.q,
                                    signed=True)
                else:
                    if act_max_q is not None:
                        act_o_q.q = min(act_max_q, act_o_q.q)
            else:
                act_o_q = QType.from_min_max(max_val=act_astats['range_out'][0]['max'],
                                             min_val=act_astats['range_out'][0]['min'],
                                             bits=self._force_width)
                act_o_q.q = min(act_o_q.q, conv_qrec.out_qs[0].q)
                if force_out and force_out.q:
                    if force_out.q > act_max_q or force_out.q > conv_qrec.out_qs[0].q:
                        # We cannot shift left in the kernel
                        # TODO - This should try to increase the input q and perhaps the width
                        # Unlikely to happen
                        raise NotImplementedError()
                    act_o_q.q = force_out.q
            act_qrec = SymmetricQuantizationRecord(in_qs=conv_qrec.out_qs,
                                                   out_qs=[act_o_q])
            result[NodeId(node, act_node)] = act_qrec
            return SymmetricQuantizationRecord(in_qs=in_qs, out_qs=act_qrec.out_qs), result
        else:
            return self.default_quantize_fusion(G, node, in_qs, force_out=force_out)

    @staticmethod
    def get_in_qs(G, edge_recs, node):
        if isinstance(node, InputParameters):
            in_qs = []
        else:
            in_qs = [edge_recs[edge.params]
                     for edge in G.indexed_in_edges(node.name)]
        return in_qs

    @staticmethod
    def is_filter_node(node):
        conv_fusion_types = set(['conv_active_pool',
                                 'conv_pool_active',
                                 'conv_active',
                                 'conv_pool'])
        return (isinstance(node, ConvFusionParameters) and node.fusion_type in conv_fusion_types) or\
            isinstance(node, (Conv2DParameters, FcParameters))

    @staticmethod
    def satisfied(x, y):
        return x is None or x == y

    def satisfied_force(self, force_out, o_q):
        return not force_out or\
            (self.satisfied(force_out.q, o_q.q) and self.satisfied(force_out.bits, o_q.bits))

    def quantize_backward(self,
                          G: NNGraph,
                          result,
                          edge_recs,
                          node,
                          force_out=None):

        LOG.debug("quantize backwards %s", node.name)
        recalculated = False
        while True:
            in_qs = self.get_in_qs(G, edge_recs, node)
            if self.is_filter_node(node):
                if isinstance(node, ConvFusionParameters):
                    qrec, qrecs = self.quantize_fusion(G,
                                                       node,
                                                       in_qs,
                                                       force_out=force_out)
                    for node_id, fqrec in qrecs.items():
                        result[node_id] = fqrec
                else:
                    qrec = self.calculate_q(node,
                                            self._activation_stats.get(NodeId(node, None)),
                                            in_qs,
                                            self._force_width,
                                            force_out=force_out)

                if force_out and force_out.q is not None and qrec.out_qs[0].q < force_out.q:
                    if recalculated:
                        raise NotImplementedError("no quantization solution found")
                    bits_to_gain = force_out.q - qrec.q
                    if bits_to_gain > in_qs[0].q:
                        raise NotImplementedError()
                    # Try to adjust the inputs to satisfy and then
                    # recalculate
                    pnode = G.in_edges(node.name)[0].from_node
                    self.quantize_backward(G,
                                           result,
                                           edge_recs,
                                           pnode,
                                           force_out=QType(bits=force_out.bits,
                                                           q=in_qs[0].q - bits_to_gain,
                                                           signed=True))
            elif isinstance(node, ConcatParameters):
                assert not recalculated
                max_width = max(in_q.bits for in_q in in_qs)
                min_q = min(in_q.q for in_q in in_qs)
                if force_out:
                    if not self.satisfied(force_out.bits, max_width):
                        max_width = force_out.bits
                    if not self.satisfied(force_out.q, min_q):
                        min_q = force_out.q
                LOG.debug("normalizing concat to %s", QType(bits=max_width, q=min_q, signed=True))
                for pidx, pnode in enumerate([edge.from_node for edge in G.in_edges(node.name)]):
                    pqrec = in_qs[pidx]
                    if pqrec.q != min_q or pqrec.bits != max_width:
                        self.quantize_backward(G,
                                               result,
                                               edge_recs,
                                               pnode,
                                               force_out=QType(bits=max_width,
                                                               q=min_q,
                                                               signed=True))
                o_q = QType(bits=max_width,
                            q=min_q,
                            signed=True)
                qrec = SymmetricQuantizationRecord(
                    in_qs=self.get_in_qs(G, edge_recs, node), out_qs=[o_q])
            elif isinstance(node, SoftMaxParameters):
                raise NotImplementedError("softmax kernel cannot change width or q")
            else:
                if isinstance(node, ConvFusionParameters):
                    qrec, qrecs = self.quantize_fusion(G,
                                                       node,
                                                       in_qs,
                                                       force_out=force_out)
                    for node_id, fqrec in qrecs.items():
                        result[node_id] = fqrec
                else:
                    qrec = self.calculate_q(node,
                                            self._activation_stats.get(NodeId(node, None)),
                                            in_qs,
                                            self._force_width,
                                            force_out=force_out)
                o_q = qrec.out_qs[0]
                if not(self.satisfied(force_out.q, o_q.q) and
                       self.satisfied(force_out.bits, o_q.bits)):
                    if recalculated:
                        raise NotImplementedError("no quantization solution found")
                    if len(G.in_edges(node.name)) > 1:
                        raise NotImplementedError("Nodes with multiple input edges \
                            need custom handling")
                    pnode = G.in_edges(node.name)[0].from_node
                    self.quantize_backward(G,
                                           result,
                                           edge_recs,
                                           pnode,
                                           force_out=force_out)

            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]

            result[NodeId(node, None)] = qrec

            o_q = qrec.out_qs[0]
            if self.satisfied_force(force_out, o_q):
                break
            if recalculated:
                raise NotImplementedError("no quantization solution found")
            LOG.debug("recalculate %s", node.name)
            recalculated = True
        LOG.debug("back complete %s %s", node.name, qrec)
        return qrec

    def quantize_forward(self, G: NNGraph, edge_recs, result=None):
        if result is None:
            result = QuantizationSet()
        for node in [step['node'] for step in G.graph_state.steps]:
            LOG.debug("quantize forward %s", node.name)
            in_qs = self.get_in_qs(G, edge_recs, node)
            if isinstance(node, ConvFusionParameters):
                qrec, qrecs = self.quantize_fusion(G, node, in_qs)
                for node_id, fqrec in qrecs.items():
                    result[node_id] = fqrec
            elif isinstance(node, ConcatParameters):
                qrec = self.quantize_backward(G,
                                              result,
                                              edge_recs,
                                              node)
            else:
                qrec = self.calculate_q(
                    node,
                    self._activation_stats.get(NodeId(node, None)),
                    in_qs,
                    self._force_width)
            result[NodeId(node, None)] = qrec
            if not qrec:
                break

            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]
        return result

    @staticmethod
    def initialize_edge_recs(G: NNGraph, qrecs):
        '''Initialize edge rec dictionary to current quantization settings'''
        edge_recs = {}
        for node in [step['node'] for step in G.graph_state.steps]:
            nodeid = NodeId(node)
            qrec = qrecs[nodeid]
            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]
        return edge_recs

    def propagate_forward(self, G: NNGraph, edge_recs, start_node, new_out_qrec, result):
        '''Propagate a new output qrec at node start_node in the graph'''
        found_node = False
        for node in [step['node'] for step in G.graph_state.steps]:
            if found_node:
                LOG.debug("propagate forwards %s", node.name)
                in_qs = self.get_in_qs(G, edge_recs, node)
                if isinstance(node, ConvFusionParameters):
                    qrec, qrecs = self.quantize_fusion(G, node, in_qs)
                    for node_id, fqrec in qrecs.items():
                        result[node_id] = fqrec
                elif isinstance(node, ConcatParameters):
                    qrec = self.quantize_backward(G,
                                                  result,
                                                  edge_recs,
                                                  node)
                else:
                    qrec = self.calculate_q(
                        node,
                        self._activation_stats.get(NodeId(node, None)),
                        in_qs,
                        self._force_width)
            else:
                if node == start_node:
                    found_node = True
                    qrec = self.quantize_backward(G,
                                                  result,
                                                  edge_recs,
                                                  node,
                                                  force_out=new_out_qrec)
                else:
                    continue

            result[NodeId(node, None)] = qrec
            if not qrec:
                break

            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]

    def quantize(self, G: NNGraph) -> OrderedDict:
        '''quantize the graph'''
        edge_recs = {}
        qrecs = self.quantize_forward(G, edge_recs)
        qrecs['__quantizer'] = self
        G.graph_identity.quantization_type = 'POW2'
        return qrecs

    @classmethod
    def propagate(cls, G: NNGraph, current_qrecs, start_node, new_out_qrec) -> OrderedDict:
        '''propagate new quantization record new_out_qrec at start node through the graph'''
        edge_recs = cls.initialize_edge_recs(G, current_qrecs)
        return current_qrecs['__quantizer'].propagate_forward(G, edge_recs, start_node, new_out_qrec, current_qrecs)
