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
from copy import deepcopy
from quantization.quantization_set import QuantizationSet

import numpy as np
from graph.nngraph import NNGraph
from graph.types.fusions import FusionBase, FusionInputParameters
from utils.node_id import NodeId

from .multiplicative.mult_quantization_scheme import MultQuantizationScheme
from .symmetric.pow2_quantization_scheme import Pow2QuantizationScheme

LOG = logging.getLogger('nntool.' + __name__)

WIDTH_TO_DTYPE = {
    8: np.int8,
    16: np.int16
}

SCHEMES = [MultQuantizationScheme, Pow2QuantizationScheme]


class ForceOutQs():
    def __init__(self) -> None:
        super().__init__()
        self._force_out_qs = {}

    def init_force_out_qs(self, parent_params, params):
        nid = NodeId(parent_params, fnode=params) if parent_params else NodeId(params)
        out_len = len(params.out_dims)
        return self._force_out_qs.setdefault(nid, [None]*out_len)

    def get_force_out_qs(self, nid):
        return self._force_out_qs.get(nid, None)

    def set_force_out_qs(self, parent_params, params, idx, qtype):
        out_qs = self.init_force_out_qs(parent_params, params)
        out_qs[idx] = qtype
        return out_qs


class EdgeQuantization():
    def __init__(self) -> None:
        super().__init__()
        self._edge_recs = {}

    def store_qtypes(self, params, qtypes, parent_params=None):
        self._edge_recs[self.get_nid(parent_params, params)] = qtypes

    def store_qtype(self, params, qtype, out_idx, parent_params=None):
        qtypes = self._edge_recs.setdefault(self.get_nid(parent_params, params), [
                                            None] * len(params.out_dims))
        qtypes[out_idx] = qtype

    @staticmethod
    def get_nid(parent_params, params):
        return NodeId(parent_params, fnode=params) if parent_params else NodeId(params)

    def get_in_qs(self, G, params, parent_params=None):
        # note: some edges can be None if not connected i.e. LSTM/GRU
        indexed_in_edges = G.indexed_in_edges(params.name)
        if not all(self.get_nid(parent_params, edge.from_node) in self._edge_recs
                   for edge in indexed_in_edges if edge is not None):
            return None
        in_qs = [self._edge_recs[self.get_nid(parent_params, edge.from_node)][edge.from_idx]
                 if edge is not None else None
                 for edge in indexed_in_edges]
        # check if we are missing an in_q for an in edge that exists
        if any(in_q is None and indexed_in_edges[idx] is not None for idx, in_q in enumerate(in_qs)):
            return None
        return in_qs

    def get_out_qs(self, params, parent_params=None):
        return deepcopy(self._edge_recs[self.get_nid(parent_params, params)])


class StateClass:
    def __init__(self, edge_recs: EdgeQuantization = None, qrecs: dict = None, frozen_out_qs: ForceOutQs = None):
        self.edge_recs = edge_recs if edge_recs is not None else EdgeQuantization()
        self.qrecs = qrecs if qrecs is not None else QuantizationSet()
        self.frozen_out_qs = frozen_out_qs if qrecs is not None else ForceOutQs()


def quantization_matches(qs1, qs2):
    return all(in_q1 == in_q2 for in_q1, in_q2 in zip(qs1, qs2))


class UnifiedQuantizer():
    def __init__(self, default_scheme, activation_stats, **kwargs):
        if not isinstance(default_scheme, str):
            raise ValueError(f'expecting name of default quantization scheme not {default_scheme}')
        self._options = kwargs
        self._activation_stats = activation_stats
        self.qrecs = None
        self.schemes = {scheme: self.get_handlers(scheme) for scheme in SCHEMES}
        self.scheme_names = {scheme.NAME: scheme for scheme in SCHEMES}
        self.default_scheme = self.scheme_names[default_scheme]

    @staticmethod
    def get_handlers(quantizer):
        quant_handlers = {}
        dequant_handlers = {}
        for handler in quantizer.HANDLERS_BASE.__subclasses__():
            if handler.PARAMS_TYPE is None:
                raise ValueError(
                    "handler of class %s does not register for any params classes" % handler)
            for params_class in handler.PARAMS_TYPE:
                quant_handlers[params_class] = handler
                if handler.CAN_DEQUANTIZE:
                    dequant_handlers[params_class] = handler
                if params_class == '__default__':
                    continue
                # handler overides subclasses as well unless they are explicitly tagged by
                # another handler
                for params_subclass in params_class.__subclasses__():
                    if params_subclass in quant_handlers:
                        continue
                    quant_handlers[params_subclass] = handler
                    if handler.CAN_DEQUANTIZE:
                        dequant_handlers[params_subclass] = handler
        return (quant_handlers, dequant_handlers)

    def get_scheme(self, params, qrec, **kwargs):
        params_options = self._options.get(params, {})
        scheme_name = params_options.get('scheme', None)
        if scheme_name:
            scheme = self.scheme_names[scheme_name]
        else:
            scheme = None
            if qrec:
                for scheme in self.schemes:
                    if isinstance(qrec, scheme.QREC_BASE):
                        break
            scheme = scheme or self.default_scheme
        return scheme

    def get_options(self, params, handler):
        params_options = self._options.get(params, {})
        handler_defaults = handler.get_default_options()
        opts = handler_defaults.copy()
        opts.update({k: v for k, v in self._options.items() if k in handler_defaults})
        opts.update({k: v for k, v in params_options.items() if k in handler_defaults})
        return opts

    # get a handler
    # options - overides, then defaults
    # scheme - overrides dict(params, {scheme: scheme, options: {})), qrec (existing scheme), default
    # handler - quantization options, params class default

    def handle(self, handler, params, in_qs, parent_params=None, **kwargs):
        stats = kwargs['all_stats'].get(EdgeQuantization.get_nid(parent_params, params))
        return handler.quantize(params, in_qs, stats, fusion=parent_params, **kwargs)

    def quantize_forwards(self, cur_G, state: StateClass, params,
                          parent_params=None, stop_at=None, **kwargs):
        if stop_at == params:
            return
        # find in_qs
        nid = EdgeQuantization.get_nid(parent_params, params)
        # if inside a fusion then pass the fusion subgraph
        in_qs = state.edge_recs.get_in_qs(
            cur_G if parent_params is None else parent_params.subgraph,
            params,
            parent_params=parent_params)
        # if in_qs is None then we have not visited all the edges leading to this node
        if in_qs is None:
            return
        # get the forced out_qs that may have been set on this node
        # this can contain Nones to indicate that an output has not been forced
        force_out_qs = state.frozen_out_qs.get_force_out_qs(nid)
        qrec = state.qrecs.get(nid)
        if qrec and quantization_matches(qrec.in_qs, in_qs):
            # if there is a qrec and all inputs match the current qrec then finish
            return

        scheme = self.get_scheme(params, qrec, **kwargs)

        quantizers = self.schemes[scheme][0]
        handler = quantizers.get(params.__class__)
        if parent_params is None and isinstance(params, FusionBase) and handler is None:
            # if we hit a fusion then recurse into its subgraph if we have no handler for it
            # first calculate or recalculate the qrec to generate appropriate input qrecs
            # TODO - Could we recurse endlessly here?
            cur_qrec = self.quantize_forwards_fusion(
                params, state, in_qs, stop_at, scheme, **kwargs)
            # if the outputs are forced then we need to quantize the fusion backwards
            if force_out_qs:
                cur_qrec = self.quantize_backwards_fusion(params, force_out_qs, cur_qrec or qrec,
                                                          state, scheme, in_qs, params, **kwargs)
        else:
            if handler is None:
                handler = quantizers['__default__']
            options = self.get_options(params, handler)
            kwargs['quantization_types'].add(handler.NAME)
            cur_qrec = self.handle(handler,
                                   params,
                                   in_qs,
                                   force_out_qs=force_out_qs,
                                   parent_params=parent_params,
                                   qrecs=state.qrecs,
                                   opts=options,
                                   **kwargs)

        # if the cur_qrec is None then the quantizer could not satisfy this input with the requested output
        if cur_qrec is None:
            raise ValueError(f'cannot find solution to quantize {params.name}')
        # check that all the in_qs on the qrec match the in_qs we wanted
        in_edges = cur_G.indexed_in_edges(params.name)
        for idx, in_q in enumerate(in_qs):
            if in_q is None:
                continue
            if cur_qrec.in_qs[idx] != in_q:
                # if the in edge does not match the requested in edge then we need to go back
                in_edge = in_edges[idx]
                # record that this output is forced
                state.frozen_out_qs.set_force_out_qs(
                    parent_params, in_edge.from_node, in_edge.from_idx, cur_qrec.in_qs[idx])
                self.quantize_backwards(cur_G, state, in_edge.from_node,
                                        stop_at=params,
                                        parent_params=parent_params,
                                        **kwargs)

        state.edge_recs.store_qtypes(params, cur_qrec.out_qs, parent_params=parent_params)
        state.qrecs[nid] = cur_qrec
        for out_edges in cur_G.indexed_out_edges(params.name):
            for out_edge in out_edges:
                self.quantize_forwards(cur_G, state, out_edge.to_node,
                                       stop_at=stop_at,
                                       parent_params=parent_params, **kwargs)

    def quantize_forwards_fusion(self, params, state, in_qs, stop_at, scheme, **kwargs):
        for finput_params in params.subgraph.inputs():
            # store the incoming qtypes onto the fusion input nodes in the subgraph
            state.edge_recs.store_qtypes(
                finput_params,
                [in_qs[finput_params.idx]],
                parent_params=params)
            # now search down each edge
            for edge in params.subgraph.out_edges(finput_params.name):
                self.quantize_forwards(params.subgraph, state,
                                       edge.to_node, parent_params=params,
                                       stop_at=stop_at,
                                       **kwargs)
        # all fusion outputs should now have their quantization set
        outs_by_idx = sorted(params.subgraph.outputs(), key=lambda x: x.idx)
        # collect all the qtypes on the fusion outputs
        out_qs = [state.edge_recs.get_in_qs(params.subgraph, foutput_params, parent_params=params)[0]
                  for foutput_params in outs_by_idx]

        # all fusion inputs may have had their qtypes updated by backwards action in the fusion
        ins_by_idx = sorted(params.subgraph.inputs(), key=lambda x: x.idx)
        in_qs = [state.edge_recs.get_out_qs(finput_params, parent_params=params)[0]
                 for finput_params in ins_by_idx]

        return scheme.DEFAULT_QREC(in_qs=in_qs, out_qs=out_qs)

    def quantize_backwards(self, cur_G, state: StateClass, params,
                           parent_params=None, stop_at=None, **kwargs):
        # find in_qs
        nid = EdgeQuantization.get_nid(parent_params, params)
        # if inside a fusion then pass the fusion subgraph
        in_qs = state.edge_recs.get_in_qs(
            cur_G if parent_params is None else parent_params.subgraph,
            params,
            parent_params=parent_params)
        # if in_qs is None then we have not visited all the edges leading to this node
        # this should not happen going backwards
        assert in_qs is not None, f'incomplete in_qs going backwards at node {params.name}'
        # get the forced outputs. we should not be going backwards if an output has not been forced
        force_out_qs = state.frozen_out_qs.get_force_out_qs(nid)
        assert force_out_qs is not None, f'going backwards without forced outputs at node {params.name}'

        # we've hit a fusion input going backwards
        # update that input on the qrec on the fusion and go backwards up that edge
        if parent_params and isinstance(params, FusionInputParameters):
            # at present this does not handle a fusion input that is bound to multiple nodes
            # this would require quantizing forward down those nodes inside the fusion
            # This only happens in expressions currently which have their own handler so are never entered
            assert len(cur_G.out_edges(params.name)
                       ) == 1, "cannot handle fusion input that needs to be quantized down"
            state.edge_recs.store_qtypes(params, force_out_qs, parent_params=parent_params)
            # don't need to force the output of this since the output of the connected nodes will be forced
            # when the forward pass on the parent fusion completes
            return

        qrec = state.qrecs.get(nid)
        assert qrec is not None, f'qrec not set going backwards at node {params.name}'
        scheme = self.get_scheme(params, qrec, **kwargs)

        quantizers = self.schemes[scheme][0]
        handler = quantizers.get(params.__class__)
        if parent_params is None and isinstance(params, FusionBase) and handler is None:
            # if we hit a fusion then recurse into its subgraph backwards if we have no handler for it
            cur_qrec = self.quantize_backwards_fusion(
                params, force_out_qs, qrec, state, scheme, in_qs, stop_at, **kwargs)
        else:
            if handler is None:
                handler = quantizers['__default__']
            kwargs['quantization_types'].add(handler.NAME)
            options = self.get_options(params, handler)
            # if a qrec is already set on the node then try to force the output
            cur_qrec = self.handle(handler,
                                   params,
                                   in_qs,
                                   force_out_qs=force_out_qs,
                                   parent_params=parent_params,
                                   qrecs=state.qrecs,
                                   backwards=True,
                                   opts=options,
                                   **kwargs)

        # if the cur_qrec is None then the quantizer could not satisfy this input with the requested output
        if cur_qrec is None:
            raise ValueError(f'cannot find solution (going back) to quantize {params.name}')
        # check that all the in_qs on the qrec match the in_qs we wanted
        in_edges = cur_G.indexed_in_edges(params.name)
        for idx, in_q in enumerate(in_qs):
            if in_q is None:
                continue
            if cur_qrec.in_qs[idx] != in_q:
                # if the in edge does not match the requested in edge then we need to go back
                in_edge = in_edges[idx]
                # force this edge to the needed qrec
                state.frozen_out_qs.set_force_out_qs(
                    parent_params, in_edge.from_node, in_edge.from_idx, cur_qrec.in_qs[idx])
                self.quantize_backwards(cur_G, state, in_edge.from_node,
                                        parent_params=parent_params,
                                        stop_at=stop_at,
                                        **kwargs)

        state.edge_recs.store_qtypes(params, cur_qrec.out_qs, parent_params=parent_params)
        state.qrecs[nid] = cur_qrec
        for idx, out_edges in enumerate(cur_G.indexed_out_edges(params.name)):
            if cur_qrec.out_qs[idx] == qrec.out_qs[idx]:
                continue
            for out_edge in out_edges:
                # stop_at contains the node that we originally started walking back from
                # if we hit it then the force must have been satisfied
                self.quantize_forwards(cur_G, state, out_edge.to_node,
                                       parent_params=parent_params,
                                       stop_at=stop_at, **kwargs)

    def quantize_backwards_fusion(self, params, force_out_qs, qrec, state, scheme, in_qs, stop_at, **kwargs):
        for foutput_params in params.subgraph.outputs():
            force_out_q = force_out_qs[foutput_params.idx]
            cur_out_q = qrec.out_qs[foutput_params.idx]
            # check if the current qtype matches the forced qtype. if it does then nothing to do
            if force_out_q is None or force_out_q == cur_out_q:
                continue
            state.frozen_out_qs.set_force_out_qs(params, foutput_params, 0, force_out_q)
            self.quantize_backwards(params.subgraph, state,
                                    foutput_params,
                                    stop_at=stop_at,
                                    parent_params=params, **kwargs)
        # all fusion outputs should now have their quantization set
        outs_by_idx = sorted(params.subgraph.outputs(), key=lambda x: x.idx)
        # collect all the qtypes on the fusion outputs
        out_qs = [state.edge_recs.get_in_qs(params.subgraph, foutput_params, parent_params=params)[0]
                  for foutput_params in outs_by_idx]
        # all fusion inputs may have had their qtypes updated by backwards action in the fusion
        ins_by_idx = sorted(params.subgraph.inputs(), key=lambda x: x.idx)
        in_qs = [state.edge_recs.get_out_qs(finput_params, parent_params=params)[0]
                 for finput_params in ins_by_idx]
        return scheme.DEFAULT_QREC(in_qs=in_qs, out_qs=out_qs)

    def dequantize(self, G: NNGraph, **kwargs):
        qrecs = G.quantization
        LOG.info("dequantizing graph parameters")
        for _, node, _, fnode in G.nodes_iterator():
            qrec = qrecs[NodeId(node, fnode)]
            anode = node if fnode is None else fnode
            scheme = self.get_scheme(anode, qrec, **kwargs)
            dequantizers = self.schemes[scheme][1]
            handler = dequantizers.get(node.__class__)
            if handler:
                handler.dequantize(anode, qrec)

    def quantize(self, G: NNGraph, start_node=None, force_out_qs=None) -> OrderedDict:
        '''quantize the graph'''
        if G.has_quantized_parameters:
            self.dequantize(G)
            G.has_quantized_parameters = False
            G.quantization = None

        # TODO when forcing one node this should retain what is set in the graph
        quantization_types = set()

        quant_kwargs = {
            'all_stats': self._activation_stats,
            'G': G,
            'quantization_types': quantization_types
        }
        start_nodes = [start_node] if start_node else G.inputs()

        state = StateClass()
        for params in start_nodes:
            self.quantize_forwards(G, state, params, **quant_kwargs)
        G.graph_identity.quantization_types = quantization_types
        return state.qrecs
