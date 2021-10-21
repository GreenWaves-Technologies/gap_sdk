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

import logging
from functools import reduce
from operator import attrgetter

from graph.matches.matchers.remove_copies import RemoveCopies
from graph.matches.matchers.remove_unnecessary_quantize_operators import RemoveUnnecessaryQuantizeOperators
from graph.types import (FusionBase, FusionInputParameters,
                         FusionOutputParameters, QuantizeParameters)
from graph.types.base import NNEdge
from quantization.handlers_helpers import (get_quantization_handlers,
                                           match_handler)
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.quantization_set import QuantizationSet
from quantization.quantizer.qrec_to_stats import set_stats
from quantization.verify_quantization import verify_quantization
from utils.graph import GraphView
from utils.node_id import NodeId
from utils.transactional_dict import TransactionalDict

LOG = logging.getLogger('nntool.' + __name__)
# DEBUG = LOG.info


def debug(msg):
    LOG.debug(msg)


def info(msg):
    LOG.info(msg)


def no_nones(x):
    return tuple(item for item in x if item is not None)


class CantContinueError(Exception):
    pass


class MissingStatsError(Exception):
    pass


class NewQuantizer():
    def __init__(self, G: GraphView, reset_all=False) -> None:
        self._G = G
        if G.quantization and not reset_all:
            self.set_stats(current_stats=G.quantization.stats)
            self._options = G.quantization.options
            if self._G.quantization.scheme_priority:
                self._schemes = self._G.quantization.scheme_priority
            else:
                self._schemes = list(self._G.quantization.schemes_present)
        else:
            self._stats = {}
            self._options = {}
            self._schemes = []
        self._postprocess = None
        self._handlers = get_quantization_handlers()
        self._qtypes = None
        self._qset = None

    @classmethod
    def from_quantized_graph(cls, G):
        if not G.quantization:
            raise ValueError('expecting quantized graph')
        return NewQuantizer(G)

    @property
    def options(self):
        return self._options

    @options.setter
    def options(self, val):
        self._options = val

    def set_options(self, **kwargs):
        self._options.update(kwargs)

    @property
    def schemes(self):
        return self._schemes

    @schemes.setter
    def schemes(self, val):
        self._schemes = val

    @property
    def qset(self):
        return self._qset

    @staticmethod
    def quantization_matches(qs1, qs2):
        return all(in_q1 == in_q2 for in_q1, in_q2 in zip(qs1, qs2))

    def report_edge(self, edge, msg, level):
        qtype = self._qtypes.get(edge, False)
        if not edge:
            level(
                f'{msg} {edge.from_node.name}:{edge.from_idx} -> '
                f'{edge.to_node.name}:{edge.to_idx} NOT SET')
        elif isinstance(qtype, tuple):
            level(
                f'{msg} {edge.from_node.name}:{edge.from_idx} -> '
                f'{edge.to_node.name}:{edge.to_idx} {qtype[2]} {qtype[0]} -> {qtype[1]}')
        else:
            level(
                f'{msg} {edge.from_node.name}:{edge.from_idx} -> '
                f'{edge.to_node.name}:{edge.to_idx} {qtype}')

    def report_resolve(self, edge, direction):
        self.report_edge(edge, f'RESOLVE {direction}', info)

    def report_conflict(self, edge):
        self.report_edge(edge, 'CONFLICT', info)

    def report_pass(self, edge):
        self.report_edge(edge, 'PASS', info)

    def report_quantizer(self, edge):
        self.report_edge(edge, 'insert quantizer', info)

    @staticmethod
    def report_qrec(phase, node, qrec):
        in_qs = ','.join(
            str(in_q) if in_q is not None else 'N/A' for in_q in qrec.in_qs)
        out_qs = ','.join(
            str(out_q) if out_q is not None else 'N/A' for out_q in qrec.out_qs)
        debug(f'{phase} {node.name} in:{in_qs} out:{out_qs}')

    def get_qtype_forced(self, edge, direction):
        # The force always takes the internal element so if there is a conflict it takes the
        # side internal to the fusion
        if edge is None:
            return None
        qtype = self._qtypes.get(edge, False)
        if qtype and isinstance(qtype, tuple):
            if direction == 'down':
                return qtype[1]
            else:
                return qtype[0]
        return qtype

    def get_qtype(self, edge, direction):
        if edge is None:
            return None
        qtype = self._qtypes.get(edge, False)
        if qtype and isinstance(qtype, tuple):
            if direction == 'down':  # if resolution direction is down and we are going down
                # then up element else down element
                return qtype[0 if qtype[2] == 'down' else 1]
            else:  # if resolution direction is up and we are going up then down element else up element
                return qtype[1 if qtype[2] == 'up' else 0]
        return qtype

    def get_conflict(self, edge, direction):
        if edge is None:
            return None
        qtype = self._qtypes.get(edge, None)
        if qtype and isinstance(qtype, tuple):
            if direction == 'down':  # if resolution direction is down and we are going down
                # then up element else down element
                return qtype[0] if qtype[2] == 'down' else None
            else:  # if resolution direction is up and we are going up then down element else up element
                return qtype[1] if qtype[2] == 'up' else None

    def get_conflict_down(self, edge):
        return self.get_conflict(edge, 'down')

    def get_conflict_up(self, edge):
        return self.get_conflict(edge, 'up')

    def get_qtype_up(self, edge):
        return self.get_qtype(edge, 'up')

    def get_qtype_down(self, edge):
        return self.get_qtype(edge, 'down')

    def get_qtype_forced_up(self, edge):
        return self.get_qtype_forced(edge, 'up')

    def get_qtype_forced_down(self, edge):
        return self.get_qtype_forced(edge, 'down')

    def set_qtype(self, edge, qtype, direction, resolution=None):
        assert qtype is None or isinstance(qtype, QType)
        cur_qtype = self._qtypes.get(edge, False)
        if not cur_qtype:
            self._qtypes[edge] = qtype
        elif isinstance(cur_qtype, tuple):
            # tuple containing edge_up_qtype, edge_down_qtype, resolution direction (up/down) i.e. the direction to proceed to try to resolve this
            if direction == 'down':
                if cur_qtype[1] == qtype:  # qtype resolves since it matches the down side
                    self._qtypes[edge] = qtype
                else:  # no resolution - just update the new wtype
                    self._qtypes[edge] = (qtype, cur_qtype[1], cur_qtype[2])
            else:
                if cur_qtype[0] == qtype:  # qtype resolves since it matches the up side
                    self._qtypes[edge] = qtype
                else:  # no resolution - just update the new wtype
                    self._qtypes[edge] = (cur_qtype[0], qtype, cur_qtype[2])
        elif cur_qtype != qtype:
            # resolution can be set when setting fusion inputs and outputs that need to resolve
            # internally
            if resolution is None:
                resolution = direction
            if direction == 'down':
                # force resolution down by default
                self._qtypes[edge] = (qtype, cur_qtype, resolution)
            else:
                # force resolution up by default
                self._qtypes[edge] = (cur_qtype, qtype, resolution)
        # qtype matches so do nothing

    def set_qtype_up(self, edge, qtype, resolution=None):
        return self.set_qtype(edge, qtype, 'up', resolution=resolution)

    def set_qtype_down(self, edge, qtype, resolution=None):
        return self.set_qtype(edge, qtype, 'down', resolution=resolution)

    def get_conflict_edges(self):
        return [edge for edge, qtypes in self._qtypes.items() if isinstance(qtypes, tuple)]

    def is_conflict(self, edge):
        return isinstance(self._qtypes.get(edge, False), tuple)

    def conflict_count(self):
        return sum(1 if self.is_conflict(edge) else 0 for edge in self._qtypes)

    def get_inqtypes_up(self, G, node):
        return [self.get_conflict_up(edge) for edge in G.indexed_in_edges(node)]

    def get_inqtypes_down(self, G, node):
        return [self.get_qtype_down(edge) for edge in G.indexed_in_edges(node)]

    @staticmethod
    def select_qtype(qtypes):
        qtypes = [qtype for qtype in qtypes if qtype is not None]
        if not qtypes:
            return None
        if len(qtypes) == 1 or all(qtypes[0] == qtype for qtype in qtypes[1:]):
            qtype = qtypes[0]
            assert isinstance(qtype, QType)
            return qtype
        float_qtype = [qtype for qtype in qtypes if qtype.is_floating]
        if float_qtype:
            qtype = sorted(float_qtype, key=lambda x: x.bits)[-1]
            assert isinstance(qtype, QType)
            return qtype
        max_bits = sorted((qtype for qtype in qtypes if qtype),
                          key=lambda x: x.bits)
        max_bits = [qtype for qtype in qtypes[:-1:]
                    if qtype.bits == max_bits[-1].bits] + [max_bits[-1]]
        if len(max_bits) == 1:
            return max_bits[0]
        max_val = max(qtype.max_val for qtype in qtypes)
        min_val = max(qtype.min_val for qtype in qtypes)
        qtype = next((qtype for qtype in qtypes if qtype.min >=
                      min_val and qtype.max <= max_val), None)
        if qtype:
            return qtype
        raise CantContinueError()

    @staticmethod
    def select_qtype_fusion(qtypes):
        qtypes = [qtype for qtype in qtypes if qtype is not None]
        if not qtypes:
            return None
        if len(qtypes) == 1 or all(qtypes[0] == qtype for qtype in qtypes[1:]):
            return qtypes[0]
        raise CantContinueError()

    def get_outqtypes_up(self, G, node):
        # this function copes with the conflict on output edges which is the most complicated scenario since
        # there can be multiple competing forces. This only handles the cases that we have seen in real models
        # or been able to emulate in synthetic models.
        qtypes = []
        for cur_qtypes, forced_qtypes in [zip(*[(self.get_qtype_forced_up(edge), self.get_conflict_up(edge))
                                                for edge in edge_bundle])
                                          for edge_bundle in G.indexed_out_edges(node)]:
            forced_qtypes = no_nones(forced_qtypes)
            if not forced_qtypes:
                qtypes.append(None)
                continue
            if len(cur_qtypes) == 1:
                if len(forced_qtypes) == 1:
                    qtypes.append(forced_qtypes[0])
                    continue
            else:
                # more than one output edge
                if any(qtype.is_floating for qtype in forced_qtypes):
                    return sorted(forced_qtypes, key=attrgetter('bits'))[-1]
                uniq_cur_qtypes = reduce(
                    lambda s, x: s if x in s else s + [x], cur_qtypes, [])
                if len(uniq_cur_qtypes) == 1:
                    uniq_cur_qtype = uniq_cur_qtypes[0]
                    if len(cur_qtypes) > len(forced_qtypes):
                        qtypes.append(uniq_cur_qtype)
                        continue
                    else:
                        # all outputs are forced. we want to keep the one that best represents
                        # the output so we calculate the maximum overlapping range
                        # TODO - what about 16 bit versus 8 bit - if the range overlap is similar then lower scale should be taken into account
                        range_diffs = sorted([(qtype, min(qtype.max, uniq_cur_qtype.max_val) - max(
                            qtype.min, uniq_cur_qtype.min_val)) for qtype in forced_qtypes], key=lambda x: x[1])
                        qtypes.append(range_diffs[-1][0])
                        continue
            cur_qtypes = ",".join(str(qtype) for qtype in cur_qtypes)
            forced_qtypes = ",".join(str(qtype) for qtype in forced_qtypes)
            raise NotImplementedError(
                f'unexpected quantization conflict seen cur {cur_qtypes} forced {forced_qtypes} - please contact GreenWaves')
        return qtypes

    def get_outqtypes_up_fusion(self, G, node):
        return [self.select_qtype_fusion([self.get_conflict_up(edge) for edge in edge_bundle]) for edge_bundle in G.indexed_out_edges(node)]

    def get_fusion_in_outs(self, int_G, ins_by_idx, outs_by_idx):
        # get the in outs forced to the actual evaluated ones with no conflict evaluation
        # remove all conflicts on fusion inputs and outputs
        in_qtypes = []
        for in_node in ins_by_idx:
            edge = int_G.out_edges(in_node)[0]
            in_qtypes.append(self.get_qtype_forced_down(edge))
            for edge in int_G.out_edges(in_node):
                self._qtypes[edge] = in_qtypes[-1]
        out_qtypes = []
        for out_node in outs_by_idx:
            edge = int_G.in_edges(out_node)[0]
            out_qtypes.append(self.get_qtype_forced_up(edge))
            self._qtypes[edge] = out_qtypes[-1]
        return in_qtypes, out_qtypes

    def set_stats(self, current_stats=None):
        self._stats = set_stats(self._G, current_stats=current_stats)

    def get_options(self, nid, handler=None):
        if handler is not None:
            opts = handler.get_default_options()
            opts.update({k: v for k, v in self._options.items()
                        if k in opts})
            node_options = self._options.get(nid, {})
            opts.update({k: v for k, v in node_options.items()
                        if k in opts})
        else:
            opts = {k: v for k, v in self._options.items() if not isinstance(k, NodeId)}
            opts.update(self._options.get(nid, {}))
        return opts

    def reset(self):
        self._qtypes = None
        self._qset = None

    def get_scheme_priority(self, node):
        opts = self._options.get(node, None)
        if opts and 'scheme' in opts:
            scheme_priority = [opts['scheme']]
        else:
            scheme_priority = self._schemes
        return [scheme.upper() for scheme in scheme_priority]

    def _choose_quantizer(self, cur_G, node, in_qs, out_qs, stats, opts, scheme_priority, **kwargs):
        return match_handler(cur_G, self._handlers, node, scheme_priority, opts,
                             in_qs_constraint=in_qs, out_qs_constraint=out_qs, **kwargs)

    def _call_quantizer(self, cur_G, handler, node, in_qs, out_qs, stats, opts, fusion,
                        direction='down', set_out_qs=False):
        if stats is None and handler.NEEDS_STATS:
            raise MissingStatsError(
                f'handler {handler.__class__.__name__} requires activation stats to be present')
        kwargs = self.setup_kwargs(
            cur_G, out_qs, opts, fusion, set_out_qs, direction)
        return handler.quantize(node, in_qs, stats, **kwargs)

    def _call_get_in_qs_from_stats(self, cur_G, direction, handler, node, in_qs, out_qs, stats, opts, fusion, set_out_qs=False):
        kwargs = self.setup_kwargs(
            cur_G, out_qs, opts, fusion, set_out_qs, direction=direction)
        return handler.get_in_qs_from_stats(node, stats, in_qs, **kwargs)

    def setup_kwargs(self, cur_G, out_qs, opts, fusion, set_out_qs, direction):
        kwargs = {
            'G': self._G,
            'opts': opts,
            'all_stats': self._stats,
            'fusion': fusion,
            'cur_G': cur_G,
            'graph_update': self._postprocess
        }
        if set_out_qs:
            kwargs['out_qs'] = out_qs
        else:
            kwargs['force_out_qs'] = out_qs
        if direction == 'up':
            kwargs['backwards'] = True
        return kwargs

    def _set_fusion_in_edges(self, parent_node, in_qtypes=None):
        if in_qtypes is None:
            in_qtypes = self.get_inqtypes_down(self._G, parent_node)
        int_G = parent_node.subgraph
        ins_by_idx = sorted(int_G.inputs(), key=lambda x: x.idx)
        out_nodes = set()
        for in_node in ins_by_idx:
            for edge in int_G.out_edges(in_node):
                self.set_qtype_down(edge, in_qtypes[in_node.idx])
                out_nodes.add(edge.to_node)
        return ins_by_idx, out_nodes

    def _set_fusion_out_edges(self, parent_node, out_qtypes=None):
        int_G = parent_node.subgraph
        if out_qtypes is None:
            for out_idx, out_edge_bundle in enumerate(self._G.indexed_out_edges(parent_node)):
                qtype = None
                for edge in out_edge_bundle:
                    if qtype is None:
                        qtype = self.get_qtype_up(edge)
                    elif qtype != self.get_qtype_up(edge):
                        raise CantContinueError()
                out_qtypes[out_idx] = qtype
        outs_by_idx = sorted(int_G.outputs(), key=lambda x: x.idx)
        in_nodes = set()
        for out_node in outs_by_idx:
            in_edge = int_G.in_edges(out_node)[0]
            self.set_qtype_up(in_edge, out_qtypes[out_node.idx])
            in_nodes.add(in_edge.from_node)
        return outs_by_idx, in_nodes

    def _bfs_pass_fusion(self, parent_node, in_qtypes, qset, visited):
        int_G = parent_node.subgraph
        self._qtypes.checkpoint()
        ins_by_idx, out_nodes = self._set_fusion_in_edges(
            parent_node, in_qtypes=in_qtypes)

        try:
            for int_node in out_nodes:
                self._bfs_pass(int_G, int_node, qset,
                               visited, parent=parent_node)
            self._qtypes.commit()
        except CantContinueError:
            self._qtypes.rollback()
            raise ValueError(
                f'Unable to quantize fusion node {parent_node.name}')

        outs_by_idx = sorted(int_G.outputs(), key=lambda x: x.idx)
        in_qtypes, out_qtypes = self.get_fusion_in_outs(
            int_G, ins_by_idx, outs_by_idx)
        assert self.no_fusion_in_outs()

        nid = NodeId(parent_node)
        stat = self._stats.get(nid, None)
        opts = self.get_options(nid)
        scheme_priority = self.get_scheme_priority(nid)
        return self._resolve_qrec(self._G, 'down', parent_node, stat, opts, scheme_priority, in_qtypes, out_qs=out_qtypes, set_out_qs=True)

    def _bfs_pass(self, cur_G, node, qset, visited, parent=None):
        in_edges = cur_G.indexed_in_edges(node)
        in_qtypes = [self.get_qtype_down(in_edge) for in_edge in in_edges]
        if any(in_qtype is False for in_qtype in in_qtypes):
            return
        if node in visited:
            return
        visited.append(node)

        nid = NodeId(node) if parent is None else NodeId(parent, fnode=node)
        pnid = NodeId(node) if parent is None else NodeId(parent)
        if isinstance(node, FusionBase) and node.quantize_internals:
            qrec = self._bfs_pass_fusion(node, in_qtypes, qset, visited)
        else:
            stat = self._stats.get(nid, None)
            opts = self.get_options(pnid)
            scheme_priority = self.get_scheme_priority(pnid)
            passed_in_qtypes = in_qtypes

            qrec = self._resolve_qrec(
                cur_G, 'down', node, stat, opts, scheme_priority, passed_in_qtypes, fusion=parent)

        self.report_qrec('bfs', node, qrec)
        qset[nid] = qrec

        for idx, in_edge in enumerate(in_edges):
            if in_edge is None:
                continue
            self.set_qtype_up(in_edge, qrec.in_qs[idx])
            if self.is_conflict(in_edge):
                self.report_conflict(in_edge)
            if not isinstance(in_edge.from_node, FusionInputParameters):
                if parent:
                    if self.is_conflict(in_edge):
                        qrecs = self.elimination_pass_up(
                            cur_G, in_edge, qrec.in_qs[idx], [node], fusion=parent)
                        for nid, newqrec in qrecs.items():
                            qset[nid] = newqrec
                        continue

        for edge_bundle in cur_G.indexed_out_edges(node):
            for edge in edge_bundle:
                self.set_qtype_down(edge, qrec.out_qs[edge.from_idx])
                if self.is_conflict(edge):
                    if parent:
                        if isinstance(edge.to_node, FusionOutputParameters):
                            raise CantContinueError()
                    self.report_conflict(edge)

        for edge_bundle in cur_G.indexed_out_edges(node):
            for edge in edge_bundle:
                if isinstance(edge.to_node, FusionOutputParameters):
                    continue
                self._bfs_pass(cur_G, edge.to_node, qset,
                               visited, parent=parent)

    def _resolve_qrec(self, cur_G, direction, node, stat, opts, scheme_priority,
                      in_qs, out_qs=None, fusion=None, set_out_qs=False):
        qrec = None
        set_in_qs = in_qs
        while qrec is None:
            handler = self._choose_quantizer(cur_G,
                                             node, set_in_qs, out_qs, stat, opts, scheme_priority)
            if handler is not None:
                pnid = NodeId(node) if fusion is None else NodeId(fusion)
                handler_opts = self.get_options(pnid, handler)
                cur_in_qs = set_in_qs
                if cur_in_qs is None:
                    cur_in_qs = self._call_get_in_qs_from_stats(
                        cur_G,
                        direction,
                        handler,
                        node, in_qs, out_qs,
                        stat, handler_opts, fusion, set_out_qs=set_out_qs)
                qrec = self._call_quantizer(cur_G,
                                            handler, node, cur_in_qs, out_qs, stat, handler_opts, fusion, direction=direction, set_out_qs=set_out_qs)
            if qrec is None:
                if out_qs is not None:
                    out_qs = None
                elif set_in_qs is None:
                    raise ValueError(
                        f'No quantizer found for node {node.name} with options {opts}')
                else:
                    set_in_qs = None
        return qrec

    def bfs_pass(self, only_inserted=False):
        """Execute breadth first quantization pass identifying all quantization conflicts.

        Args:
            schemes (list[str]): List of quantization schemes to use in order of priority
        """
        if not self._schemes:
            raise ValueError('no quantization schemes set')
        self._qtypes = TransactionalDict()
        self._qset = QuantizationSet()
        self.remove_quantizers(only_inserted=only_inserted)
        visited = []
        for node in self._G.inputs():
            self._bfs_pass(self._G, node, self._qset, visited)

    def elimination_pass_up(self, cur_G, edge, qtype, visited, fusion=None):
        if edge.from_node in visited:
            return {}
        node = edge.from_node
        debug(f'eliminate up {node.name}:{edge.from_idx}')

        qrecs = {}
        qrec = self.evaluate(cur_G, node, 'up', qrecs, fusion=fusion)
        self.report_qrec('up', node, qrec)
        # check if we resolved this edge conflict

        was_conflict = self.is_conflict(edge)
        self.set_qtype_down(edge, qrec.out_qs[edge.from_idx])
        if self.is_conflict(edge):
            if fusion:
                raise CantContinueError()
            if not was_conflict:
                self.report_conflict(edge)
        else:
            if was_conflict:
                self.report_resolve(edge, 'up')

        self.continue_down(cur_G, qrecs, [node] + visited,
                           node, qrec, exclude_edge=edge, fusion=fusion)
        self.continue_up(cur_G, qrecs, [node] +
                         visited, node, qrec, fusion=fusion)
        return qrecs

    def elimination_pass_down(self, cur_G, edge, qtype, visited, fusion=None):
        if edge.to_node in visited:
            return {}
        node = edge.to_node
        debug(f'eliminate down {node.name}:{edge.to_idx}')

        qrecs = {}
        qrec = self.evaluate(cur_G, node, 'down', qrecs, fusion=fusion)
        self.report_qrec('down', node, qrec)

        was_conflict = self.is_conflict(edge)
        self.set_qtype_up(edge, qrec.in_qs[edge.to_idx])
        if self.is_conflict(edge):
            if fusion:
                raise CantContinueError()
            if not was_conflict:
                self.report_conflict(edge)
        else:
            if was_conflict:
                self.report_resolve(edge, 'down')

        self.continue_up(cur_G, qrecs, [node] + visited,
                         node, qrec, exclude_edge=edge, fusion=fusion)
        self.continue_down(
            cur_G, qrecs, [node] + visited, node, qrec, fusion=fusion)
        return qrecs

    def no_fusion_in_outs(self):
        for edge in self.get_conflict_edges():
            if isinstance(edge.from_node, FusionInputParameters) or isinstance(edge.to_node, FusionOutputParameters):
                return False
        return True

    def elimination_fusion_pass_down(self, parent_node, qrecs, in_qs, out_qs):
        int_G = parent_node.subgraph
        self._qtypes.checkpoint()
        ins_by_idx, _ = self._set_fusion_in_edges(parent_node, in_qtypes=in_qs)
        outs_by_idx, _ = self._set_fusion_out_edges(
            parent_node, out_qtypes=out_qs)
        in_qtypes = [self.get_qtype_down(int_G.out_edges(in_node)[0])
                     for in_node in ins_by_idx]
        visited = []
        try:
            for fusion_input in int_G.inputs():
                for out_edge in int_G.out_edges(fusion_input):
                    qrecs.update(self.elimination_pass_down(
                        int_G, out_edge, in_qtypes[fusion_input.idx], visited, fusion=parent_node))
            self._qtypes.commit()
        except CantContinueError:
            self._qtypes.rollback()
            return self._qset[NodeId(parent_node)]

        in_qtypes, out_qtypes = self.get_fusion_in_outs(
            int_G, ins_by_idx, outs_by_idx)

        for edge in self._G.indexed_in_edges(parent_node):
            was_conflict = self.is_conflict(
                edge) if edge in self._qtypes else False
            self.set_qtype_up(edge, in_qtypes[edge.to_idx])
            if self.is_conflict(edge):
                if not was_conflict:
                    self.report_conflict(edge)
            else:
                if was_conflict:
                    self.report_resolve(edge, 'down')

        assert self.no_fusion_in_outs()

        nid = NodeId(parent_node)
        stat = self._stats.get(nid, None)
        opts = self.get_options(nid)
        scheme_priority = self.get_scheme_priority(nid)
        return self._resolve_qrec(self._G, 'down', parent_node, stat, opts, scheme_priority, in_qtypes, out_qs=out_qtypes, set_out_qs=True)

    def elimination_fusion_pass_up(self, parent_node, qrecs, in_qs, out_qs):
        int_G = parent_node.subgraph
        self._qtypes.checkpoint()
        # when entering a fusion we set the top side of the inputs to the required in_qs
        # and the bottom side of the outputs to the required out_qs
        # at the end of the resolution of the fusion we take the bottom side of the top and the
        # top side of the bottom (if either are in conflict). This will reflect the internal state of the
        # fusion
        ins_by_idx, _ = self._set_fusion_in_edges(parent_node, in_qtypes=in_qs)
        outs_by_idx, _ = self._set_fusion_out_edges(
            parent_node, out_qtypes=out_qs)
        out_qtypes = [self.get_conflict_up(int_G.in_edges(
            out_node)[0]) for out_node in outs_by_idx]
        if not out_qtypes or all(qtype is None for qtype in out_qtypes):
            self._qtypes.rollback()
            debug(f"{parent_node} output has not been forced. Abandoning.")
            return self._qset[NodeId(parent_node)]

        visited = []
        try:
            for fusion_output in int_G.outputs():
                in_edge = int_G.in_edges(fusion_output)[0]
                qrecs.update(self.elimination_pass_up(
                    int_G, in_edge, out_qtypes[fusion_output.idx], visited, fusion=parent_node))
            self._qtypes.commit()
        except CantContinueError:
            self._qtypes.rollback()
            qrec = self._qset[NodeId(parent_node)]
            return qrec

        in_qtypes, out_qtypes = self.get_fusion_in_outs(
            int_G, ins_by_idx, outs_by_idx)

        for edge_bundle in self._G.indexed_out_edges(parent_node):
            for edge in edge_bundle:
                was_conflict = self.is_conflict(
                    edge) if edge in self._qtypes else False
                self.set_qtype_up(edge, out_qtypes[edge.from_idx])
                if self.is_conflict(edge):
                    if not was_conflict:
                        self.report_conflict(edge)
                else:
                    if was_conflict:
                        self.report_resolve(edge, 'up')

        assert self.no_fusion_in_outs()

        nid = NodeId(parent_node)
        stat = self._stats.get(nid, None)
        opts = self.get_options(nid)
        scheme_priority = self.get_scheme_priority(nid)
        qrec = self._resolve_qrec(self._G, 'up', parent_node, stat, opts,
                                  scheme_priority, in_qtypes, out_qs=out_qtypes, set_out_qs=True)
        return qrec

    def evaluate(self, cur_G, node, direction, qrecs, fusion=None):
        in_qs = self.get_inqtypes_down(cur_G, node)
        if fusion:
            out_qs = self.get_outqtypes_up_fusion(cur_G, node)
        else:
            out_qs = self.get_outqtypes_up(cur_G, node)
        nid = NodeId(node) if fusion is None else NodeId(fusion, fnode=node)
        pnid = NodeId(node) if fusion is None else NodeId(fusion)
        stat = self._stats.get(nid, None)
        opts = self.get_options(pnid)
        scheme_priority = self.get_scheme_priority(pnid)
        if isinstance(node, FusionBase) and node.quantize_internals:
            try:
                if direction == "up":
                    qrec = self.elimination_fusion_pass_up(
                        node, qrecs, in_qs, out_qs)
                else:
                    qrec = self.elimination_fusion_pass_down(
                        node, qrecs, in_qs, out_qs)
                # qrec on fusion
            except CantContinueError:
                # no change
                qrec = self._qset[nid]
        else:
            qrec = self._resolve_qrec(cur_G, direction,
                                      node, stat, opts, scheme_priority, in_qs, out_qs, fusion=fusion)
        qrecs[nid] = qrec
        return qrec

    def continue_down(self, cur_G, qrecs, visited, node, qrec, exclude_edge=None, fusion=None):
        # check all down edges except edge. proceed down any that don't match
        for idx, edge_bundle in enumerate(cur_G.indexed_out_edges(node)):
            out_qtype = qrec.out_qs[idx]
            for out_edge in edge_bundle:
                if out_edge == exclude_edge:
                    continue
                was_conflict = self.is_conflict(out_edge)
                self.set_qtype_down(out_edge, out_qtype)
                if self.is_conflict(out_edge):
                    if not was_conflict:
                        self.report_conflict(out_edge)
                else:
                    if was_conflict:
                        self.report_resolve(out_edge, 'cont down')

        for idx, edge_bundle in enumerate(cur_G.indexed_out_edges(node)):
            for out_edge in edge_bundle:
                if isinstance(out_edge.to_node, FusionOutputParameters):
                    continue
                if not self.is_conflict(out_edge):
                    continue
                qrecs.update(self.elimination_pass_down(cur_G,
                                                        out_edge, self.get_qtype_down(out_edge), visited + [node], fusion=fusion))

    def continue_up(self, cur_G, qrecs, visited, node, qrec, exclude_edge=None, fusion=None):
        # check in_qs for match problems and continue up

        for idx, in_edge in enumerate(cur_G.indexed_in_edges(node)):
            if in_edge is None or in_edge == exclude_edge:
                continue
            was_conflict = self.is_conflict(in_edge)
            self.set_qtype_up(in_edge, qrec.in_qs[idx])
            if self.is_conflict(in_edge):
                if not was_conflict:
                    self.report_conflict(in_edge)
            else:
                if was_conflict:
                    self.report_resolve(in_edge, 'cont up')
                continue

        for idx, in_edge in enumerate(cur_G.indexed_in_edges(node)):
            if in_edge is None:
                continue
            if isinstance(in_edge.from_node, FusionInputParameters):
                continue
            if in_edge == exclude_edge or not self.is_conflict(in_edge):
                continue
            qrecs.update(self.elimination_pass_up(cur_G,
                                                  in_edge, self.get_qtype_up(in_edge), visited + [node], fusion=fusion))

    def update_qrecs(self, qrecs):
        for k, v in qrecs.items():
            self._qset[k] = v

    def elimination_pass(self):
        # continue until we can eliminate no more conflicts
        found_results = True
        while found_results:
            found_results = False
            edges = self.get_conflict_edges()
            while edges:
                edge = edges.pop(0)
                if not self.is_conflict(edge):
                    continue
                self._qtypes.checkpoint()

                before_len = self.conflict_count()
                conflict = self._qtypes[edge]
                self.report_pass(edge)

                if conflict[2] == 'up':
                    qrecs = self.elimination_pass_up(self._G,
                                                     edge, conflict[1], [edge.to_node])
                else:
                    qrecs = self.elimination_pass_down(self._G,
                                                       edge, conflict[0], [edge.from_node])
                after_len = self.conflict_count()
                if after_len < before_len:
                    info(
                        f"SOLUTION ACCEPTED Quantizers before {before_len} after {after_len}")
                    self._qtypes.commit()
                    self.update_qrecs(qrecs)
                    found_results = True
                else:
                    info(
                        f"SOLUTION REJECTED Quantizers before {before_len} after {after_len}")
                    self._qtypes.rollback()

    def insert_quantizers(self):
        for edge in self.get_conflict_edges():
            from_qtype, to_qtype, conflict_dir = self._qtypes[edge]
            self.report_quantizer(edge)

            qnode = QuantizeParameters(
                self._G.unique_name(f'{edge.to_node.name}_qin{edge.to_idx}'),
                from_qtype=from_qtype,
                to_qtype=to_qtype)
            self._qset[NodeId(qnode)] = QRec(
                in_qs=[from_qtype], out_qs=[to_qtype], ktype="*")
            self._qtypes[edge] = from_qtype
            self._stats[NodeId(qnode)] = {
                'range_in': [{'min': from_qtype.min_val, 'max': from_qtype.max_val}],
                'range_out': [{'min': to_qtype.min_val, 'max': to_qtype.max_val}]
            }
            self._G.insert_node_at_edge(qnode, edge, edge_class=NNEdge)
            for out_edge in self._G.out_edges(qnode):
                self._qtypes[out_edge] = to_qtype
        RemoveCopies().match(self._G)

    def remove_quantizers(self, only_inserted=False):
        for node in self._G.nodes(node_classes=QuantizeParameters):
            if only_inserted and not node.inserted_by_quantizer:
                continue
            self._G.remove_and_reconnect(node, edge_class=NNEdge)
            nid = NodeId(node)
            if self._qset and nid in self._qset:
                del self._qset[nid]
            if self._stats and nid in self._stats:
                del self._stats[nid]

    def quantize(self):
        if not self._schemes:
            raise ValueError('no quantization schemes set')
        self._postprocess = {'requires_adjust': False}
        self.bfs_pass()
        self._G.quantization = self.qset
        self.elimination_pass()
        self.insert_quantizers()
        RemoveUnnecessaryQuantizeOperators().match(self._G)
        self._G.add_dimensions()
        if self._postprocess['requires_adjust']:
            self._G.adjust_order()
        self._G.quantization.scheme_priority = self._schemes
        self._G.quantization.schemes_present |= set(self._schemes)
        self._G.quantization.stats = self._stats
        self._G.quantization.options = self._options
        problems = verify_quantization(self._G)
        if problems:
            LOG.warning('quantization has errors')
            for problem in problems:
                LOG.warning(problem)
            raise ValueError('quantization did not complete successfully')
        return self._G.quantization
