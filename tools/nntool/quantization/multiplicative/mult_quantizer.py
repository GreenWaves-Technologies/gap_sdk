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

import numpy as np
from graph.nngraph import NNGraph
from graph.types import InputParameters
from graph.types.fusions import FusionBase
from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.quantization_set import QuantizationSet
from quantization.quantizer import Quantizer
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys

from .mult_quantization_handler import MultQuantizionHandler
# pylint: disable=wildcard-import,unused-wildcard-import
from .quantizers import *  # noqa

LOG = logging.getLogger('nntool.' + __name__)

WIDTH_TO_DTYPE = {
    8: np.int8,
    16: np.int16
}


class MultQuantizer(Quantizer, JsonSerializable):
    def __init__(self, activation_stats, force_width=None,
                 quantized_dimension=None, narrow_weights=False,
                 options=None):
        self._options = options or {}
        self._activation_stats = activation_stats
        self._force_width = force_width
        self._quantized_dimension = quantized_dimension
        self._narrow_weights = narrow_weights
        self.qrecs = None
        self.handlers = self.get_handlers()

    # for tests
    def __eq__(self, value):
        return self._activation_stats == value._activation_stats and \
            self._force_width == value._force_width

    def _encapsulate(self):
        return {
            'activation_stats': convert_keys_to_str(self._activation_stats),
            'force_width': self._force_width,
            'quantized_dimension': self._quantized_dimension
        }

    @classmethod
    def _dencapsulate(cls, val):
        return MultQuantizer(convert_str_to_keys(val['activation_stats']),
                             val['force_width'],
                             val['quantized_dimension'])

    @staticmethod
    def get_in_qs(G, edge_recs, node):
        if isinstance(node, InputParameters):
            in_qs = []
        else:
            in_qs = [edge_recs[edge.params] if edge is not None else None
                     for edge in G.indexed_in_edges(node.name)]
        return in_qs

    def handle(self, handler, params, in_qs, out_dtype, stats=None, **kwargs):
        if stats is None:
            stats = kwargs['all_stats'].get(NodeId(params, None))
        return handler.quantize(params, in_qs, out_dtype, stats, **kwargs)

    @staticmethod
    def get_handlers():
        quant_handlers = {}
        dequant_handlers = {}
        for handler in MultQuantizionHandler.__subclasses__():
            if handler.PARAMS_TYPE is None:
                raise ValueError(
                    "handler of class %s does not register for any params classes" % handler)
            for params_class in handler.PARAMS_TYPE:
                quant_handlers[params_class] = handler
                if handler.CAN_DEQUANTIZE:
                    dequant_handlers[params_class] = handler
        return quant_handlers, dequant_handlers

    def quantize_forward_fusion(self, pparams, in_qs, out_dtype, **kwargs):
        fin_qs = in_qs
        for fparams in pparams.contained_nodes():
            handler = self.handlers[0].get(fparams.__class__, self.handlers[0]['__default__'])
            stats = kwargs['all_stats'].get(NodeId(pparams, fparams))
            qrec = self.handle(handler, fparams, fin_qs, out_dtype, stats=stats, fusion=pparams, **kwargs)
            assert qrec, "handler did not return a result"
            self.qrecs[NodeId(pparams, fparams)] = qrec
            fin_qs = qrec.out_qs
        return MultQuantizationRecord(in_qs=in_qs, out_qs=fin_qs)

    def quantize_forward(self, edge_recs, out_dtype=np.int8, **kwargs):
        G = kwargs['G']
        for params in [step['node'] for step in G.graph_state.steps]:
            LOG.debug("quantize forward %s", params.name)
            in_qs = self.get_in_qs(G, edge_recs, params)
            handler = self.handlers[0].get(params.__class__, None)
            if handler:
                qrec = self.handle(handler, params, in_qs, out_dtype, **kwargs)
            elif isinstance(params, FusionBase):
                qrec = self.quantize_forward_fusion(params, in_qs, out_dtype, **kwargs)
            else:
                handler = self.handlers[0]['__default__']
                qrec = self.handle(handler, params, in_qs, out_dtype, **kwargs)
            assert qrec, "handler did not return a result"
            self.qrecs[NodeId(params, None)] = qrec
            for edges in G.indexed_out_edges(params.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]

    def dequantize(self, G: NNGraph):
        qrecs = G.quantization
        LOG.info("dequantizing graph parameters")
        for _, node, _, fnode in G.nodes_iterator():
            qrec = qrecs[NodeId(node, fnode)]
            anode = node if fnode is None else fnode
            handler = self.handlers[1].get(anode.__class__)
            if handler:
                handler.dequantize(anode, qrec)

    def quantize(self, G: NNGraph) -> OrderedDict:
        '''quantize the graph'''
        if G.has_quantized_parameters:
            self.dequantize(G)
            G.has_quantized_parameters = False
            G.quantization = None

        self.qrecs = QuantizationSet()
        edge_recs = {}
        opts = {
            'force_width': self._force_width,
            'quantized_dimension': self._quantized_dimension,
            'narrow_weights': self._narrow_weights
        }
        opts.update(self._options)
        quant_kwargs = {
            'opts': opts,
            'all_stats': self._activation_stats,
            'G': G,
            'qrecs': self.qrecs
        }
        dtype = WIDTH_TO_DTYPE[self._force_width]
        self.quantize_forward(edge_recs, dtype=dtype, **quant_kwargs)
        self.qrecs['__quantizer'] = self
        G.graph_identity.quantization_type = 'SQ8'
        return self.qrecs
