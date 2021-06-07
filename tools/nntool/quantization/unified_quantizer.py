# Copyright (C) 2020, 2021  GreenWaves Technologies, SAS

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
from copy import deepcopy

import numpy as np
from graph.nngraph import NNGraph
from graph.types.base import NNEdge
from graph.types.expression_fusion import ExpressionFusionParameters
from graph.types.fusions import (FusionBase, FusionInputParameters,
                                 FusionOutputParameters)
from graph.types.input_output import ConstantInputParameters
from graph.types.others import QuantizeParameters
from utils.node_id import NodeId

from quantization.handlers_helpers import (get_quantization_handlers,
                                           match_handler)
from quantization.new_qrec import QRec
from quantization.quantization_set import QuantizationSet

LOG = logging.getLogger('nntool.' + __name__)
DEBUG = LOG.info

WIDTH_TO_DTYPE = {
    8: np.int8,
    16: np.int16
}


class NoHandlerError(Exception):
    pass


class ForceQs():
    def __init__(self) -> None:
        super().__init__()
        self._force_qs = {
            'in': {},
            'out': {}
        }

    def init_force_qs(self, parent_params, params, direction):
        if parent_params:
            nid = NodeId(parent_params, fnode=params)
        else:
            nid = NodeId(params)
        out_len = len(getattr(params, f'{direction}_dims'))
        return self._force_qs[direction].setdefault(nid, [None]*out_len)

    def clear_force_qs(self, parent_params, params, direction):
        if parent_params:
            nid = NodeId(parent_params, fnode=params)
        else:
            nid = NodeId(params)
            if isinstance(params, FusionBase) and params.quantize_internals:
                for fnode in params.contained_nodes():
                    self.clear_force_qs(params, fnode, 'in')
                    self.clear_force_qs(params, fnode, 'out')
        if nid in self._force_qs[direction]:
            del self._force_qs[direction][nid]

    def get_force_out_qs(self, nid):
        return self._force_qs['out'].get(nid, None)

    def get_force_in_qs(self, nid):
        return self._force_qs['in'].get(nid, None)

    def set_force_qs(self, parent_params, params, idx, qtype, direction):
        qtypes = self.init_force_qs(parent_params, params, direction)
        qtypes[idx] = qtype
        if all(qtype is None for qtype in qtypes):
            self.clear_force_qs(parent_params, params, direction)
            return None
        return qtypes

    def set_force_out_qs(self, parent_params, params, idx, qtype):
        return self.set_force_qs(parent_params, params, idx, qtype, 'out')

    def set_force_in_qs(self, parent_params, params, idx, qtype):
        return self.set_force_qs(parent_params, params, idx, qtype, 'in')

    def override(self, nid, qtypes, direction):
        force_qs = self._force_qs[direction].get(nid, None)
        if force_qs is None:
            return qtypes
        if qtypes is None:
            return force_qs.copy()
        return [qtype if force_qs[idx] is None else force_qs[idx]
                for idx, qtype in enumerate(qtypes)]

    def override_in_qs(self, nid, in_qs):
        return self.override(nid, in_qs, 'in')

    def override_out_qs(self, nid, out_qs):
        return self.override(nid, out_qs, 'out')


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
    def __init__(self, edge_recs: EdgeQuantization = None, qrecs: dict = None, frozen_qs: ForceQs = None):
        self.edge_recs = edge_recs if edge_recs is not None else EdgeQuantization()
        self.qrecs = qrecs if qrecs is not None else QuantizationSet()
        if frozen_qs is None:
            frozen_qs = ForceQs()
        self.qrecs.frozen_qs = frozen_qs

    @property
    def frozen_qs(self) -> ForceQs:
        return self.qrecs.frozen_qs

    @classmethod
    def from_g(cls, G, frozen_qs=None):
        if not G.quantization:
            return StateClass(frozen_qs=frozen_qs)
        edge_quantization = EdgeQuantization()
        for node in G.nodes():
            if isinstance(node, FusionBase) and node.quantize_internals:
                for fnode in node.subgraph.nodes():
                    if isinstance(fnode, (FusionInputParameters, FusionOutputParameters)):
                        continue
                    qrec = G.quantization.get(NodeId(node, fnode))
                    if qrec:
                        edge_quantization.store_qtypes(
                            fnode, qrec.out_qs, parent_params=node)
            qrec = G.quantization.get(NodeId(node))
            if qrec:
                edge_quantization.store_qtypes(node, qrec.out_qs)
        return StateClass(edge_recs=edge_quantization, qrecs=G.quantization,
                          frozen_qs=frozen_qs or G.quantization.frozen_qs)


def quantization_matches(qs1, qs2):
    return all(in_q1 == in_q2 for in_q1, in_q2 in zip(qs1, qs2))


def ranges_are_valid(ranges):
    return not any(rng['min'] is None or rng['max'] is None for rng in ranges)


def build_stat(G, nid, node=None):
    qrec = G.quantization.get(nid)
    if qrec is None:
        return None
    if qrec.in_qs is None or qrec.out_qs is None:
        return None
    range_in = [{'min': qtype.min_val, 'max': qtype.max_val} if qtype and qtype.has_valid_range else {'min': None, 'max': None}
                for qtype in qrec.in_qs]
    range_out = [{'min': qtype.min_val, 'max': qtype.max_val} if qtype and qtype.has_valid_range else {'min': None, 'max': None}
                 for qtype in qrec.out_qs]
    range_in_valid = ranges_are_valid(range_in)
    range_out_valid = ranges_are_valid(range_out)
    if not range_in_valid or not range_out_valid:
        # maybe this is a quantize node with a float type one side of it
        if isinstance(node, QuantizeParameters) and (range_in_valid or range_out_valid):
            if range_in_valid:
                range_out = deepcopy(range_in)
            else:
                range_in = deepcopy(range_out)
        elif isinstance(node, ConstantInputParameters):
            range_out = [{'min': np.atleast_1d(
                node.value.min()), 'max': np.atleast_1d(node.value.max())}]
        else:
            LOG.warning(
                '%s does not have valid activation range information', nid.node_name)
    return {
        'range_in': range_in,
        'range_out': range_out
    }


def build_stats(G, current_stats):
    stats = {}
    for node in G.nodes():
        nid = NodeId(node)
        if current_stats and nid in current_stats:
            stats[nid] = current_stats[nid]
            continue
        stats[nid] = build_stat(G, nid, node=node)
        if isinstance(node, FusionBase) and node.quantize_internals:
            for fnode in node.subgraph.nodes():
                nid = NodeId(node, fnode)
                # if fusion input or output recs are not present build them
                if isinstance(fnode, FusionInputParameters) and nid not in G.quantization:
                    first_edge = node.subgraph.out_edges(fnode.name)[0]
                    edge_qrec = G.quantization.get(
                        NodeId(node, first_edge.to_node))
                    edge_qtype = edge_qrec.in_qs[first_edge.to_idx]
                    G.quantization[nid] = QRec.copy_ktype(
                        edge_qrec, in_qs=[edge_qtype], out_qs=[edge_qtype])
                elif isinstance(fnode, FusionOutputParameters) and nid not in G.quantization:
                    first_edge = node.subgraph.in_edges(fnode.name)[0]
                    edge_qrec = G.quantization.get(
                        NodeId(node, first_edge.from_node))
                    edge_qtype = edge_qrec.out_qs[first_edge.from_idx]
                    G.quantization[nid] = QRec.copy_ktype(
                        edge_qrec, in_qs=[edge_qtype], out_qs=[edge_qtype])
                stats[nid] = build_stat(G, nid)
        elif isinstance(node, ExpressionFusionParameters):
            stats[nid]['expression'] = G.quantization[nid].cache['expression']
    return stats


def reduce_start_nodes(G, nodes):
    nodes = sorted(nodes, key=lambda x: x.step_idx)
    start_nodes = []
    while nodes:
        node = nodes.pop(0)
        start_nodes.append(node)
        if not nodes:
            break
        for snode in G.dfs(node.name):
            if snode in nodes:
                nodes.remove(snode)
    return start_nodes


def str_qs(qtypes):
    if qtypes is None:
        return "None"
    return ",".join(str(qtype) for qtype in qtypes)


class UnifiedQuantizer():
    def __init__(self, scheme_priority, activation_stats, **kwargs):
        if isinstance(scheme_priority, str):
            scheme_priority = [scheme_priority]
        self._options = kwargs
        self._activation_stats = activation_stats
        self.handlers = get_quantization_handlers()
        self.scheme_priority = scheme_priority

    @classmethod
    def from_quantized_graph(cls, G, extra_schemes=None, **kwargs):
        if not G.quantization:
            raise ValueError(
                'calling from_quantized_graph on unquantized graph')
        stats = build_stats(G, G.quantization.stats)
        scheme_priority = G.quantization.scheme_priority or ['SQ8']
        if extra_schemes:
            for scheme in extra_schemes:
                if scheme not in scheme_priority:
                    scheme_priority.append(scheme)
        return UnifiedQuantizer(scheme_priority, stats, **kwargs)

    def get_options(self, parent_params, params, handler, **kwargs):
        force_options = kwargs.get('force_options') or {}
        params_options = self._options.get(params, {})
        handler_defaults = handler.get_default_options()
        opts = handler_defaults.copy()
        opts.update({k: v for k, v in self._options.items()
                     if k in handler_defaults})
        opts.update({k: v for k, v in params_options.items()
                     if k in handler_defaults})
        params = parent_params if parent_params is not None else params
        if params in force_options:
            opts.update(force_options[params])
        return opts

    # get a handler
    # options - overides, then defaults
    # scheme - overrides dict(params, {scheme: scheme, options: {})), qrec (existing scheme), default
    # handler - quantization options, params class default

    def handle(self, handler, params, in_qs, parent_params=None, **kwargs):
        stats = kwargs['all_stats'].get(
            EdgeQuantization.get_nid(parent_params, params))
        return handler.quantize(params, in_qs, stats, fusion=parent_params, **kwargs)

    @staticmethod
    def insert_quantize_in(G, params, idx, edge, out_qtype=None):

        # could be a name clash so be careful
        good_name = None
        name_idx = 0
        while not good_name:
            name = f'{params.name}_quant_in{idx}_{name_idx}'
            if name not in G:
                good_name = name
                break
        qparams = QuantizeParameters(good_name, to_qtype=out_qtype)
        qparams.in_dims = qparams.out_dims = [
            deepcopy(edge.from_node.out_dims[edge.from_idx])]
        G.insert_node_at_edge(qparams, edge, edge_class=NNEdge)
        LOG.info(
            'inserted quantize to %s between %s:%s and %s:%s',
            out_qtype or "new qtype", edge.from_node.name, edge.from_idx,
            edge.to_node.name, edge.to_idx)
        return qparams

    def match_forwards_handler(self, cur_G, state: StateClass, params,
                               scheme_priority, in_qs, force_out_qs,
                               parent_params=None, stop_at=None, **kwargs):
        quantizers_inserted = False
        # here we need to cope with the situation that a quantization handler has not been
        # found for the input type but we know the scheme that we should apply.
        # The only thing we need to figure out is what is the default kernel that we
        # can select and what are its input contraints
        while True:
            handler = match_handler(cur_G, self.handlers, params, scheme_priority, self._options.get(params, {}),
                                    in_qs_constraint=in_qs, out_qs_constraint=force_out_qs)
            # handler is none if no handler for scheme matching input type is found
            if handler:
                DEBUG("handler %s selected for %s(%s)", handler.__name__,
                      params.__class__.__name__, params.name)
                return in_qs, force_out_qs, handler
            if not parent_params:
                LOG.info('no match for params %s in %s out %s scheme %s',
                         params.name, QRec.qs_to_str(in_qs), QRec.qs_to_str(force_out_qs), scheme_priority)
                # see if we can find a handler without the input constraint
                handler = match_handler(cur_G, self.handlers, params, scheme_priority, self._options.get(params, {}),
                                        in_qs_constraint=None, out_qs_constraint=force_out_qs)
                if handler is None:
                    LOG.info('no match for params %s in None out %s scheme %s',
                             params.name, QRec.qs_to_str(force_out_qs), scheme_priority)
                    if force_out_qs and parent_params is None:
                        LOG.info('removing force out on params %s force_out %s',
                                 params.name, QRec.qs_to_str(force_out_qs))
                        force_out_qs = None
                        handler = match_handler(cur_G, self.handlers, params, scheme_priority, self._options.get(params, {}),
                                                in_qs_constraint=None, out_qs_constraint=force_out_qs)
                if handler is None:
                    raise NoHandlerError(
                        f'no handler found for node {params.name} - even with no constraints!')
                options = self.get_options(
                    parent_params, params, handler, **kwargs)
                stats = kwargs['all_stats'].get(
                    EdgeQuantization.get_nid(parent_params, params))
                in_qs = handler.get_in_qs_from_stats(
                    params, stats, in_qs, opts=options, **kwargs)
                return in_qs, force_out_qs, handler
            raise ValueError(f'no handler found for node {params.name}')

    def quantize_forwards(self, cur_G, state: StateClass, params,
                          parent_params=None, stop_at=None, force_change=False,
                          force_all=False, **kwargs):
        # stop_at contains a stack of nodes that we are exploring backwards from
        if stop_at is None:
            stop_at = []
        if params in stop_at:
            DEBUG('---- STOPPED AT %s', params.name)
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
        force_out_qs = state.frozen_qs.get_force_out_qs(nid)
        qrec = state.qrecs.get(nid)
        if not force_change and qrec and quantization_matches(qrec.in_qs, in_qs):
            # if there is a qrec and all inputs match the current qrec then finish
            DEBUG('%s: qrec matches input', params.name)
            return
        DEBUG(
            'forwards %s in: %s out: %s stop %s fusion %s',
            params.name,
            str_qs(in_qs),
            str_qs(force_out_qs),
            stop_at and stop_at[0].name,
            parent_params is not None)

        scheme_priority = self.get_scheme_priority(
            parent_params, params, **kwargs)

        while True:
            act_in_qs, act_force_out_qs, handler = self.match_forwards_handler(cur_G, state, params,
                                                                       scheme_priority, in_qs, force_out_qs,
                                                                       parent_params=parent_params, stop_at=stop_at, **kwargs)

            if parent_params is None and isinstance(params, FusionBase) and handler.FUSION_HANDLER:
                # if we hit a fusion then recurse into its subgraph if we have no handler for it
                # first calculate or recalculate the qrec to generate appropriate input qrecs
                cur_qrec = self.quantize_forwards_fusion(
                    params, state, act_in_qs, stop_at, handler, **kwargs)
                # if the outputs are forced then we need to quantize the fusion backwards
                if force_out_qs:
                    cur_qrec = self.quantize_backwards_fusion(params, force_out_qs, cur_qrec or qrec,
                                                              state, handler, act_in_qs, params, **kwargs)
            else:
                # call found handler
                options = self.get_options(
                    parent_params, params, handler, **kwargs)
                state.qrecs.schemes_present.add(handler.SCHEME)
                cur_qrec = self.handle(handler,
                                       params,
                                       act_in_qs,
                                       force_out_qs=act_force_out_qs,
                                       parent_params=parent_params,
                                       qrecs=state.qrecs,
                                       opts=options,
                                       cur_G=cur_G,
                                       **kwargs)

            # if the cur_qrec is None then the handler could not satisfy this input with the requested output
            if cur_qrec is None:
                # if not in a fusion we can remove the output force and try to satisfy
                # This will encourage the next layer to insert a quantizer
                if force_out_qs and parent_params is None:
                    LOG.info('handler %s failed params %s force_out %s',
                             handler.__name__, params.name, QRec.qs_to_str(force_out_qs))
                    force_out_qs = None
                    continue
                raise ValueError(
                    f'cannot find solution to quantize {params.name}')
            else:
                break
        # check that all the in_qs on the qrec match the in_qs we wanted
        in_edges = cur_G.indexed_in_edges(params.name)
        for idx, in_q in enumerate(in_qs):
            if in_q is None:
                continue
            if cur_qrec.in_qs[idx] != in_q:
                DEBUG("forwards in edge %s does not match was %s need %s",
                      idx, in_q, cur_qrec.in_qs[idx])
                # if the in edge does not match the requested in edge then we need to go back
                in_edge = in_edges[idx]
                # record that this output is forced
                state.frozen_qs.set_force_out_qs(
                    parent_params, in_edge.from_node, in_edge.from_idx, cur_qrec.in_qs[idx])
                self.quantize_backwards(cur_G, state, in_edge.from_node,
                                        stop_at=[params] + stop_at,
                                        parent_params=parent_params,
                                        **kwargs)
        DEBUG('forwards finished in edges %s', params.name)

        state.edge_recs.store_qtypes(
            params, cur_qrec.out_qs, parent_params=parent_params)
        state.qrecs[nid] = cur_qrec
        self.go_forward(cur_G, state, params,
                        parent_params=parent_params, stop_at=stop_at, force_change=force_change,
                        force_all=force_all, **kwargs)

    def go_forward(self, cur_G, state: StateClass, params,
                   parent_params=None, stop_at=None, force_change=False,
                   force_all=False, **kwargs):
        for idx, out_edges in enumerate(cur_G.indexed_out_edges(params.name)):
            DEBUG("forwards at %s on out edge %s", params.name, idx)
            for out_edge in out_edges:
                self.quantize_forwards(cur_G, state, out_edge.to_node,
                                       stop_at=stop_at,
                                       parent_params=parent_params,
                                       force_change=force_all and force_change,
                                       force_all=force_all, **kwargs)

    @staticmethod
    def copy_options_and_scheme(scheme_priority, kwargs, qparams, params):
        kwargs['force_scheme'][qparams] = scheme_priority[0]
        # force the same options as this node so we pick up things like float dtype
        force_options = kwargs['force_options'].get(params)
        if force_options:
            kwargs['force_options'][qparams] = force_options

    def insert_quantize_out(self, cur_G, params, idx, edges, in_q, out_q, state, **kwargs):
        # could be a name clash so be careful
        qparams = QuantizeParameters(
            cur_G.unique_name(f'{params.name}_quant_out{idx}'),
            from_qtype=in_q, to_qtype=out_q, inserted_by_quantizer=True)
        qparams.in_dims = qparams.out_dims = [deepcopy(params.out_dims[idx])]
        for edge in edges:
            cur_G.remove_edge(edge)
        cur_G.add_edge(NNEdge(from_node=params, from_idx=idx, to_node=qparams))
        to_nodes = []
        for edge in edges:
            cur_G.add_edge(NNEdge(from_node=qparams,
                                  to_node=edge.to_node, to_idx=edge.to_idx))
            to_nodes.append((edge.to_node, edge.to_idx))
        all_stats = kwargs['all_stats']
        edge_stats = all_stats.get(NodeId(params))
        if edge_stats is None:
            raise ValueError(f'no range information present for {params.name}')
        out_stats = edge_stats['range_out'][idx]
        all_stats[NodeId(qparams)] = {'range_in': [
            out_stats], 'range_out': [out_stats]}
        state.frozen_qs.set_force_out_qs(
            None, qparams, 0, out_q)
        LOG.info(
            'inserted quantize from %s to %s between %s:%s and %s%s',
            in_q, out_q, params.name, idx,
            ",".join(f'{node[0].name}:{node[1]}' for node in to_nodes),
            " forced out" if out_q.forced else "")
        self.quantize_forwards(cur_G, state, qparams, stop_at=list(
            node[0] for node in to_nodes), **kwargs)

    def insert_quantizers_on_outputs(self, cur_G, params, in_qs, out_qs, state, parent_params, **kwargs):
        # check each edge to see if there is a difference with the force
        # and insert or modify QuantizeParameters
        all_out_edges = cur_G.indexed_out_edges(params.name)
        for idx, out_q in enumerate(out_qs):
            if out_q is not None and out_q != in_qs[idx]:
                # insert the quantize before all the edges
                force_q = deepcopy(out_q)
                self.insert_quantize_out(
                    cur_G, params, idx, all_out_edges[idx], in_qs[idx], force_q, state, **kwargs)
                # remove the force
                state.frozen_qs.set_force_qs(
                    parent_params, params, idx, None, 'out')

    def quantize_forwards_fusion(self, params, state, in_qs, stop_at, fusion_handler, **kwargs):
        DEBUG("forwards fusion %s", params.name)

        for finput_params in params.subgraph.inputs():
            # store the incoming qtypes onto the fusion input nodes in the subgraph
            state.edge_recs.store_qtypes(
                finput_params,
                [in_qs[finput_params.idx]],
                parent_params=params)
        for finput_params in params.subgraph.inputs():
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
        options = self.get_options(
            None, params, fusion_handler, **kwargs)
        return self.handle(fusion_handler, params, in_qs, out_qs=out_qs, opts=options, **kwargs)

    def get_scheme_priority(self, parent_params, params, backwards=False, **kwargs):
        force_scheme = kwargs['force_scheme']
        if not force_scheme:
            return self.scheme_priority
        params = parent_params if parent_params else params
        if params in force_scheme:
            return [force_scheme[params]]
        return self.scheme_priority

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
        force_out_qs = state.frozen_qs.get_force_out_qs(nid)
        DEBUG(
            'backwards %s in: %s out: %s stop %s fusion %s',
            params.name,
            str_qs(in_qs),
            str_qs(force_out_qs),
            stop_at and stop_at[0].name,
            parent_params is not None)
        # we've hit a fusion input going backwards
        # update that input on the qrec on the fusion and go backwards up that edge
        if parent_params and isinstance(params, FusionInputParameters):
            # at present this does not handle a fusion input that is bound to multiple nodes
            # this would require quantizing forward down those nodes inside the fusion
            # This only happens in expressions currently which have their own handler so are never entered
            assert len(cur_G.out_edges(params.name)
                       ) == 1, "cannot handle fusion input that needs to be quantized down"
            state.edge_recs.store_qtypes(
                params, force_out_qs, parent_params=parent_params)
            # don't need to force the output of this since the output of the connected nodes will be forced
            # when the forward pass on the parent fusion completes
            return

        qrec = state.qrecs.get(nid)
        if qrec is None:
            raise ValueError(
                f'qrec not set going backwards at node {params.name}')
        scheme_priority = self.get_scheme_priority(
            parent_params, params, backwards=True, **kwargs)

        cur_qrec = None
        force_removed = None
        while True:
            handler = match_handler(cur_G, self.handlers, params, scheme_priority, self._options.get(params, {}),
                                    in_qs_constraint=in_qs, out_qs_constraint=force_out_qs)
            # handler is none if no handler for scheme matching input type is found
            if handler is None:
                LOG.info('no match for params %s in %s out %s scheme %s', params.name,
                         qrec.in_qs_str, QRec.qs_to_str(force_out_qs), scheme_priority)
                if force_out_qs and parent_params is None:
                    force_removed = force_out_qs
                    force_out_qs = None
                    break
                raise NoHandlerError(
                    f'no handler found for node {params.name}')

            DEBUG("handler %s selected for %s(%s)", handler.__name__,
                  params.__class__.__name__, params.name)

            if parent_params is None and isinstance(params, FusionBase) and handler.FUSION_HANDLER:
                # if we hit a fusion then recurse into its subgraph backwards if we have no handler for it
                try:
                    cur_qrec = self.quantize_backwards_fusion(
                        params, force_out_qs, qrec, state, handler, in_qs, stop_at, **kwargs)
                except NoHandlerError as exc:
                    if force_out_qs and parent_params is None:
                        force_removed = force_out_qs
                        force_out_qs = None
                        break
                    raise NoHandlerError(
                        f'handler error raise quantizing fusion {params.name} {exc}') from exc
            else:
                state.qrecs.schemes_present.add(handler.SCHEME)
                options = self.get_options(
                    parent_params, params, handler, **kwargs)
                # if a qrec is already set on the node then try to force the output
                cur_qrec = self.handle(handler,
                                       params,
                                       in_qs,
                                       force_out_qs=force_out_qs,
                                       parent_params=parent_params,
                                       qrecs=state.qrecs,
                                       backwards=True,
                                       opts=options,
                                       cur_G=cur_G,
                                       **kwargs)

            # if the cur_qrec is None then the quantizer could not satisfy this input with the requested output
            if cur_qrec is None:
                if force_out_qs and parent_params is None:
                    LOG.info('handler %s failed params %s force_out %s',
                             handler.__name__, params.name, QRec.qs_to_str(force_out_qs))
                    force_removed = force_out_qs
                    force_out_qs = None
                    continue
                if parent_params is not None:
                    raise NoHandlerError(
                        f'no solution found for node {params.name}')
                raise ValueError(
                    f'cannot find solution (going back) to quantize {params.name}')
            else:
                break

        # the force out was removed
        if force_removed:
            # check each edge to see if there is a difference with the force
            # and insert or modify QuantizeParameters
            self.insert_quantizers_on_outputs(
                cur_G, params, qrec.out_qs, force_removed, state, parent_params, **kwargs)
            # no point checking the in edges since we didn't change the qrec
        else:
            # check that all the in_qs on the qrec match the in_qs we wanted
            in_edges = cur_G.indexed_in_edges(params.name)
            for idx, in_q in enumerate(in_qs):
                if in_q is None:
                    continue
                if cur_qrec.in_qs[idx] != in_q:
                    DEBUG("backwards in edge %s does not match was %s need %s",
                          idx, in_q, cur_qrec.in_qs[idx])
                    # if the in edge does not match the requested in edge then we need to go back
                    in_edge = in_edges[idx]
                    # force this edge to the needed qrec
                    state.frozen_qs.set_force_out_qs(
                        parent_params, in_edge.from_node, in_edge.from_idx, cur_qrec.in_qs[idx])
                    self.quantize_backwards(cur_G, state, in_edge.from_node,
                                            parent_params=parent_params,
                                            stop_at=[params] + stop_at,
                                            **kwargs)
            DEBUG('backwards finished in_edges %s', params.name)
            state.edge_recs.store_qtypes(
                params, cur_qrec.out_qs, parent_params=parent_params)
            state.qrecs[nid] = cur_qrec

        for idx, out_edges in enumerate(cur_G.indexed_out_edges(params.name)):
            # cur_qrec will be none if a quantizer was inserted
            if cur_qrec and cur_qrec.out_qs[idx] == qrec.out_qs[idx]:
                continue
            DEBUG("backwards out edge %s does not match was %s need %s",
                  idx, qrec.out_qs[idx], cur_qrec and cur_qrec.out_qs[idx])
            for out_edge in out_edges:
                # stop_at contains the node that we originally started walking back from
                # if we hit it then the force must have been satisfied
                self.quantize_forwards(cur_G, state, out_edge.to_node,
                                       parent_params=parent_params,
                                       stop_at=stop_at, **kwargs)
        DEBUG('backwards finished out_edges %s', params.name)

    def quantize_backwards_fusion(self, params, force_out_qs, qrec, state, fusion_handler, in_qs, stop_at, **kwargs):
        DEBUG("backwards fusion %s", params.name)
        to_quantize = []
        for foutput_params in params.subgraph.outputs():
            force_out_q = force_out_qs[foutput_params.idx]
            cur_out_q = qrec.out_qs[foutput_params.idx]
            # check if the current qtype matches the forced qtype. if it does then nothing to do
            if force_out_q is None or force_out_q == cur_out_q:
                continue
            state.frozen_qs.set_force_out_qs(
                params, foutput_params, 0, force_out_q)
            to_quantize.append(foutput_params)
        for foutput_params in to_quantize:
            self.quantize_backwards(params.subgraph, state,
                                    foutput_params,
                                    stop_at=[params] + stop_at,
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
        return self.handle(fusion_handler, params, in_qs, out_qs=out_qs, **kwargs)

    def quantize(self, G: NNGraph, frozen_qs=None, start_nodes=None,
                 force_scheme=None, force_options=None) -> QuantizationSet:
        '''quantize the graph

        Parameters
        ----------
        G :
            The graph to quantize
        frozen_qs : ForceQs, optional
            Object containing a mapping from NodeIds to qtype records forcing in or out qtypes on
            edges to a specific type
        start_node: Params, optional
            Start quantizing at this node. By default starts from all input nodes.'''

        quant_kwargs = {
            'all_stats': self._activation_stats,
            'G': G,
            'force_scheme': force_scheme if force_scheme is not None else {},
            'force_options': force_options if force_options is not None else {},
            'graph_update': {'requires_adjust': False}
        }
        state = StateClass.from_g(G, frozen_qs=frozen_qs)
        state.qrecs.scheme_priority = self.scheme_priority
        state.qrecs.stats = self._activation_stats
        state.qrecs.options = self._options
        if start_nodes:
            for params in start_nodes:
                state.frozen_qs.clear_force_qs(None, params, 'out')
                state.frozen_qs.clear_force_qs(None, params, 'in')
            for params in reduce_start_nodes(G, start_nodes):
                self.quantize_forwards(
                    G, state, params, force_change=True, **quant_kwargs)
        else:
            for params in G.inputs():
                self.quantize_forwards(G, state, params, **quant_kwargs)
        G.quantization = state.qrecs
        if quant_kwargs['graph_update']['requires_adjust']:
            G.add_dimensions(quiet=True)
            G.adjust_order()
            G.add_dimensions(quiet=True)
        return state.qrecs
