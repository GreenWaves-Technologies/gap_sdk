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
import re

import numpy as np

from nntool.graph.types import (ConstantInputNode, InputNode,
                         OutputNode, TransposeNode, ReshapeNode, ConcatNode, SplitNode)
from nntool.graph.types.fusions import PaddedAddFusionNode

LOG = logging.getLogger(__name__)


def find_next_dim(shape, next_idx):
    for i in range(len(shape) - 1, -1, -1):
        if shape[i] != next_idx[i]:
            return i
    return None


def at_tensor_loader(filename, exception_on_error=False):
    with open(filename, "r") as pfile:
        return at_tensor_loader_int(pfile, exception_on_error=exception_on_error)


def at_tensor_loader_int(fp, exception_on_error=False):
    re_head = re.compile(
        r'^Node: (?P<node_name>[a-zA-Z_][a-zA-Z0-9_]*)'
        r', Argument: (?P<arg_name>[a-zA-Z_][a-zA-Z0-9_]*)'
        r', Dim: (?P<dims>\d+)'
        r', \[(?P<d0>\d+)\]\[(?P<d1>\d+)\]\[(?P<d2>\d+)\]\[(?P<d3>\d+)\]\[(?P<d4>\d+)\]'
        r' ItemSize: (?P<item_size>\d+)$')
    re_cont = re.compile(
        r'^D(?P<dim>\d+): (?P<dim_idx>\d+)$')
    re_last2 = re.compile(
        r'^D(?P<dim_pen>\d+): (?P<dim_pen_start>\d+)'
        r' - D(?P<dim_last>\d+):(?P<dim_last_start>\d+)\.\.(?P<dim_last_end>\d+)$')
    re_last1 = re.compile(
        r'^D(?P<dim_last>\d+):(?P<dim_last_start>\d+)\.\.(?P<dim_last_end>\d+)$')

    def do_warning(node_tensors, arg_name, mess, *args):
        node_tensors[arg_name] = mess % tuple(args)
        LOG.warning(node_tensors[arg_name])
        if exception_on_error:
            raise ValueError(node_tensors[arg_name])

    state = 'start'
    tensors = {}
    line_num = -1
    for line in fp:
        line_num += 1
        if state == 'start':
            match = re_head.search(line)
            if not match:
                continue
            header = match.group('node_name', 'arg_name', 'dims', 'd0',
                                 'd1', 'd2', 'd3', 'd4', 'item_size')
            shape = [int(i) for i in [header[3], header[4], header[5],
                                      header[6], header[7]]]
            dims = int(header[2])
            shape = shape[(5-dims):]
            next_idx = [-1] * dims
            node_name = header[0]
            node_tensors = tensors.get(node_name)
            if not node_tensors:
                node_tensors = {}
                tensors[node_name] = node_tensors
            arg_name = header[1]
            item_size = int(header[8])
            dims_read = 0
            value = []
            state = 'read_dims'
        elif state == 'read_dims' and dims >= 2 and (dims - dims_read) == 2:
            match = re_last2.search(line)
            if not match:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - expecting last dim', line_num)
                state = 'start'
                continue
            header = [int(elem) for elem in match.group('dim_pen', 'dim_pen_start', 'dim_last',
                                                        'dim_last_start', 'dim_last_end')]
            if header[0] != dims_read or header[2] != dims_read + 1:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - wrong dim', line_num)
                state = 'start'
                continue
            next_idx[header[0]] = header[1] + 1
            next_idx[header[2]] = header[4]
            cur_block_len = header[4] - header[3]
            cur_block_read = 0
            state = 'read_data'
        elif state == 'read_dims' and dims == 1 and dims_read == 0:
            match = re_last1.search(line)
            if not match:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - expecting last dim', line_num)
                state = 'start'
                continue
            header = [int(elem) for elem in match.group(
                'dim_last', 'dim_last_start', 'dim_last_end')]
            if header[0] != dims_read:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - wrong dim', line_num)
                state = 'start'
                continue
            next_idx[header[0]] = header[2]
            cur_block_len = header[2] - header[1]
            cur_block_read = 0
            state = 'read_data'
        elif state == 'read_dims':
            match = re_cont.search(line)
            if not match:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - expecting dim', line_num)
                state = 'start'
                continue
            header = [int(elem) for elem in match.group('dim', 'dim_idx')]
            if header[0] != dims_read:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - wrong dim', line_num)
                state = 'start'
                continue
            next_idx[header[0]] = header[1] + 1
            dims_read += 1
        elif state == 'read_data':
            read = 0
            try:
                for i in line.split():
                    value.append(float(i))
                    read += 1
            except ValueError:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - read error in state %s', line_num, state)
                state = 'start'
                continue

            cur_block_read += read

            if cur_block_read == cur_block_len:
                state = 'read_dims'
                dims_read = find_next_dim(shape, next_idx)
                if dims_read is None:
                    state = 'start'
                    if arg_name in node_tensors.keys():
                        node_tensors[arg_name] = np.concatenate((node_tensors[arg_name],
                                                                 np.array(value).reshape(shape)), axis=0)
                    else:
                        node_tensors[arg_name] = np.array(value).reshape(shape)
                else:
                    state = 'read_dims'
            elif cur_block_read > cur_block_len:
                do_warning(node_tensors, arg_name,
                           '[%s] bad_tensor - wrong number of elements in data block', line_num)
                state = 'start'
                continue
    return tensors


def add_result(result, idx, val, shape=None, dtype=None, node=None):
    while len(result) < idx + 1:
        result.append(None)
    try:
        if shape:
            val = val.reshape(shape)
        result[idx] = val.astype(dtype)
    except:
        result[idx] = val.astype(dtype)
        LOG.warning(
            f"Cannot reshape tensor {idx} for layer {node.name} loaded anyway with shape {val.shape}")


def add_node_output(G, node, result, tensor, dtype=None):
    for edge in G.out_edges(node.name):
        add_result(result[edge.to_node.step_idx], edge.to_idx,
                   tensor, node.out_dims[0].shape, node=node, dtype=dtype)
    add_result(result[node.step_idx], 0, tensor,
               node.out_dims[0].shape, node=node, dtype=dtype)


def look_for_output(G, edge):
    while not isinstance(edge.to_node, OutputNode):
        if edge.to_node.not_generated:
            edges = G.out_edges(edge.to_node)
            if len(edges) == 1:
                edge = edges[0]
                continue
        return None
    return edge


def at_map_tensors(G, tensors):
    re_snum = re.compile(
        r'^S(?P<step>\d+)_')
    re_snum_outnum = re.compile(
        r"s([0-9]+)_output(?:_([0-9]+))?")
    steps = G.graph_state.steps
    result = [[None] for _ in steps]
    constant_nodes = {node.name.lower(): node for node in G.nodes(
        node_classes=ConstantInputNode)}
    input_nodes = {node.name.lower(): node
                   for node in G.input_nodes()}
    output_nodes = {node.name.lower(): node
                    for node in G.outputs()}
    for cname, tset in tensors.items():
        match = re_snum.search(cname)
        if not match:
            raise ValueError("tensor name in unexpected format")
        step_idx = int(match.group('step'))
        node = steps[step_idx]['node']
        inputs = {edge.from_node.name.lower(): edge
                  for edge in G.in_edges(node.name)}
        outputs = {edge.to_node.name.lower(): edge
                   for edge in G.out_edges(node.name)}
        if G.quantization:
            in_qs = G.quantization[node.name].in_qs
            out_qs = G.quantization[node.name].out_qs
        for tname, tensor in tset.items():
            tname = tname.lower()
            snum_match = re_snum_outnum.search(tname)
            # tensor comes from an input
            if tname.startswith('input') and tname in inputs:
                input_edge = inputs[tname]
                input_node = input_edge.from_node
                idx = input_edge.to_idx
                # set the input output to this tensor
                add_result(result[input_node.step_idx], 0,
                           tensor, node.in_dims[idx].shape, in_qs[idx].dtype, node)
            elif tname.startswith('output') and tname in outputs:
                out_edge = outputs[tname]
                out_node = out_edge.to_node
                idx = out_edge.from_idx
                # set the output out to this tensor
                add_result(result[out_node.step_idx], 0,
                           tensor, node.out_dims[idx].shape, out_qs[idx].dtype, node)
                # set this node output
                add_result(result[node.step_idx], idx,
                           tensor, node.out_dims[idx].shape, out_qs[idx].dtype, node)
            elif snum_match:
                match_step_idx = int(snum_match.group(1))
                if snum_match.group(2):
                    match_out_idx = int(snum_match.group(2))
                else:
                    match_out_idx = 0
                if step_idx == match_step_idx:
                    if isinstance(node, PaddedAddFusionNode):
                        out_q_dtype = out_qs[0].dtype
                        if result[step_idx][0] is None:
                            result[step_idx][0] = np.zeros(
                                node.out_dims[0].shape)
                        pad_top = node.padding_dim()[0][0]
                        pad_bot = node.padding_dim()[0][1]
                        if "Top" in cname:
                            # here if tensor is the pad top part
                            result[step_idx][0][:pad_top, :,
                                                :] = tensor.astype(out_q_dtype)
                        elif "Bot" in cname:
                            # here if tensor is pad bot part
                            result[step_idx][0][-pad_bot:, :,
                                                :] = tensor.astype(out_q_dtype)
                        else:
                            # here the body
                            result[step_idx][0][pad_top:-pad_bot,
                                                :, :] = tensor.astype(out_q_dtype)
                    else:
                        add_result(result[step_idx],
                                   match_out_idx,
                                   tensor,
                                   node.out_dims[match_out_idx].shape,
                                   out_qs[match_out_idx].dtype,
                                   node)
                else:
                    out_node = G[match_step_idx]
                    res_rec = result[match_step_idx]
                    if len(res_rec) <= match_out_idx or res_rec[match_out_idx] is None:
                        out_node_qrec = G.quantization[out_node.name]
                        add_result(res_rec,
                                   match_out_idx,
                                   tensor,
                                   out_node.out_dims[match_out_idx].shape,
                                   out_node_qrec.out_qs[match_out_idx].dtype,
                                   out_node)
            elif tname in constant_nodes:
                tname_node = constant_nodes[tname]
                nid = tname_node.name
                qtype = G.quantization[nid].out_qs[0] if G.quantization and nid in G.quantization else None
                if qtype and qtype.attr.ne16_decode:
                    tensor = ne16_decode(G, qtype, tensor)
                elif qtype and qtype.attr.resize:
                    tensor = np.resize(tensor.reshape(
                        qtype.attr.resize[1]), qtype.attr.resize[0])
                # tensor is a combination of multiple values Hstacked
                if qtype and qtype.attr.concatenated_nodes:
                    concatenated_nodes = [G[node_name]
                                          for node_name in qtype.attr.concatenated_nodes]
                    h_lens = [tname_node.out_dims[0].shape[-1]] + \
                        [n.out_dims[0].shape[-1] for n in concatenated_nodes]
                    tensor = tensor.reshape(
                        tname_node.out_dims[0].shape[:-1] + [sum(h_lens)])
                    elem_sum = 0
                    for idx, elem in enumerate(h_lens):
                        elem_sum += elem
                        h_lens[idx] = elem_sum
                    nodes_to_add = zip(
                        [tname_node] + concatenated_nodes, np.hsplit(tensor, np.array(h_lens)))
                else:
                    nodes_to_add = [(tname_node, tensor)]
                for node_pair in nodes_to_add:
                    add_node_output(
                        G, node_pair[0], result, node_pair[1], dtype=qtype.dtype)
            elif tname in output_nodes:
                add_result_for_node(G, result, output_nodes[tname], tensor)
            elif tname in input_nodes:
                add_result_for_node(G, result, input_nodes[tname], tensor)

    # Calculate stacked tensor split outputs
    for node in G.nodes(node_classes=SplitNode):
        if node.axis != 0:
            continue
        in_edge = G.in_edges(node)[0]
        in_tensor = result[in_edge.from_node.step_idx][0]
        if in_tensor is None:
            res_node = result[node.step_idx]
            if len(res_node) != len(G.indexed_out_edges(node)) or any(res is None
                                                                      for res in res_node):
                continue
            in_node = in_edge.from_node
            in_idx = in_edge.from_idx
            in_qrec = G.quantization[in_node.name]
            in_tensor = np.concatenate(res_node, axis=0)
            add_result(result[in_node.step_idx], in_idx,
                       in_tensor,
                       shape=in_node.out_dims[in_idx],
                       dtype=in_qrec.out_qs[in_idx].dtype)
    # Calculate stacked tensor concat outputs
    for node in G.nodes(node_classes=ConcatNode):
        if node.axis != 0:
            continue

        in_tensors = []
        for edge in G.indexed_in_edges(node):
            out_res = result[edge.from_node.step_idx]
            if edge.from_idx >= len(out_res):
                break
            tensor = out_res[edge.from_idx]
            if tensor is None:
                break
            in_tensors.append(tensor)
        else:
            out_qrec = G.quantization[node.name]
            add_result(result[node.step_idx],
                       0,
                       np.concatenate(in_tensors, axis=0),
                       shape=node.out_dims[0].shape,
                       dtype=out_qrec.out_qs[0])
    # Make sure all outputs are set
    for node in G.nodes(node_classes=OutputNode):
        if result[node.step_idx][0] is not None:
            continue
        in_edge = G.in_edges(node)[0]
        in_res = result[in_edge.from_node.step_idx]
        if in_edge.from_idx < len(in_res):
            out_qrec = G.quantization[node.name]
            in_res = in_res[in_edge.from_idx]
            if in_res is not None:
                add_result(
                    result[node.step_idx],
                    0,
                    in_res,
                    shape=node.in_dims[0],
                    dtype=out_qrec.in_qs[0].dtype)

    return result


def add_result_for_node(G, result, node, tensor):
    qrec = G.quantization[node.name]
    add_result(result[node.step_idx],
               0,
               tensor,
               node.out_dims[0].shape,
               qrec.out_qs[0].dtype,
               node)


def ne16_decode(G, qtype, tensor):
    ne16_attr = qtype.attr.ne16_decode
    if ne16_attr['type'] == 'RNN':
        chan_out = ne16_attr['Ko']
        chan_in = ne16_attr['Ki']
        nb_ki = chan_in // 16 + (1 if chan_in % 16 != 0 else 0)
        chan_in_real = ne16_attr['KiReal']
        w_bits = ne16_attr['Qw']
        res = np.zeros((chan_out, chan_in_real), dtype=np.uint8)
        tensor = tensor.astype(np.int8).astype(np.uint8).reshape((-1, 2))
        for ko in range(chan_out):
            for ki in range(chan_in_real):
                kimaj = ki // 16
                kimin = ki % 16
                byte = kimin // 8
                shift = kimin % 8
                for q in range(w_bits):
                    index = ko*nb_ki*w_bits + q*nb_ki + kimaj
                    if tensor[index, byte] & (1 << shift):
                        res[ko, ki] |= 1 << q
        return res
    raise NotImplementedError()
