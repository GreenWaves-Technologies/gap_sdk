# index ((name, (start, stop, step)), (name, (start, stop, step)), (name, None))

from functools import reduce
import math
import sys
from typing import Mapping, Sequence, Tuple

import numpy as np
from graph.types import (ConcatParameters, FusionInputParameters,
                         FusionOutputParameters, Parameters, SplitParameters,
                         StridedSliceParameters,
                         TransposeParameters)
from utils.graph import GraphView


def reverse_transpose(trans):
    return [trans.index(idx) for idx in range(len(trans))]


def do_transpose(trans, shape):
    return [shape[idx] for idx in trans]


def explore_node(subg, node, state, out_slice=None, in_slice=None):
    # out slice contains an list of outputs
    # each output contains a list of
    #   a tuple of the destination node and axis
    #   a list of the slices of the output of this node
    dest_slice_sets = []
    src_slice_sets = []
    if isinstance(node, ConcatParameters):
        # out_slice needs to be reduced to the indexes range in the output of each portion
        # in_slice needs to be reduced to the elements of the portion that make up that output
        out_slice = out_slice[0]
        in_slice = in_slice[0]
        dest_start = out_slice[node.axis][1][0]
        dest_end = out_slice[node.axis][1][1]
        dest_stride = out_slice[node.axis][1][2]
        dest_next = dest_start
        in_idx = 0
        for edge in subg.indexed_in_edges(node.name):
            in_shape = node.in_dims[edge.to_idx].shape
            in_end = in_idx + in_shape[node.axis]
            seg_dest_start = dest_start + in_idx * dest_stride
            seg_dest_end = min(dest_start + in_end * dest_stride, dest_end)
            # test if this portion of the concat is present in the output
            if dest_next > seg_dest_end or dest_next > dest_end:
                dest_slice_sets.append(None)
                in_idx = in_end
                continue
            dest_slice_set = [
                [out_slice[idx][0], out_slice[idx][1] if idx !=
                    node.axis else [seg_dest_start, seg_dest_end, dest_stride]]
                for idx, _ in enumerate(in_shape)]
            dest_slice_sets.append(dest_slice_set)
            src_seg_start = dest_next - seg_dest_start
            src_seg_end = min(seg_dest_end - seg_dest_start, in_end)
            src_slice_set = [
                [in_slice[idx][0], in_slice[idx][1] if idx !=
                    node.axis else [src_seg_start, src_seg_end, dest_stride]]
                for idx, _ in enumerate(in_shape)]
            src_slice_sets.append(src_slice_set)
            dest_next += ((src_seg_end - src_seg_start) //
                          dest_stride) * dest_stride
            in_idx = in_end
        explore_edges(src_slice_sets, dest_slice_sets, state, node, subg)
    elif isinstance(node, TransposeParameters):
        dest_slice_sets = [do_transpose(
            reverse_transpose(node.transpose), out_slice)]
        src_slice_sets = [do_transpose(
            reverse_transpose(node.transpose), in_slice)]
        explore_edges(src_slice_sets, dest_slice_sets, state, node, subg)
    elif isinstance(node, StridedSliceParameters):
        # in_slice needs to be expanded to the indexes before the slice
        out_slice = out_slice[0]
        assert len(node.act_slice) == len(out_slice)
        dest_slice_sets = [out_slice]
        src_slice_set = []
        for idx, sl in enumerate(node.act_slice):
            in_sl_name = in_slice[idx][0]
            in_sl = in_slice[idx][1]
            # express the range of the slice in terms of the input
            src_seg_start = in_sl[0] + sl[0]
            src_seg_end = src_seg_start + (in_sl[1] - in_sl[0]) * sl[2]
            src_seg_step = in_sl[2] * sl[2]
            src_slice_set.append(
                [in_sl_name, [src_seg_start, src_seg_end, src_seg_step]])
        src_slice_sets = [src_slice_set]
        explore_edges(src_slice_sets, dest_slice_sets, state, node, subg)
    elif isinstance(node, FusionOutputParameters):
        slice_set = [[(node.name, idx), [0, dim, 1]]
                     for idx, dim in enumerate(node.dims.shape)]
        src_slice_sets = [slice_set]
        dest_slice_sets = [slice_set]
        explore_edges(src_slice_sets, dest_slice_sets, state, node, subg)
    elif isinstance(node, FusionInputParameters):
        in_state = state.setdefault(node, [])
        in_state.append((in_slice, sorted(out_slice, key=lambda x: x[0])))
    else:
        raise ValueError(
            f"don't know how to handle {node.name} {node.__class__.__name__}")


def explore_edges(src_slice_sets, dest_slice_sets, state, node, subg):
    state[node] = (src_slice_sets, dest_slice_sets)

    for edge in subg.indexed_in_edges(node.name):
        explore_node(subg, edge.from_node, state,
                     out_slice=[dest_slice_sets[edge.to_idx]], in_slice=[src_slice_sets[edge.to_idx]])


def eval_copy(subg):
    state = {}
    for node in subg.outputs():
        explore_node(subg, node, state)
    return {
        'inputs': tuple((node.name, state[node]) for node in subg.inputs()),
        'outputs': tuple((node.name, state[node]) for node in subg.outputs())
    }


# group copies with compatible directions on all axis on the same inputs and outputs (i.e. they all have to be the same traversal)
# or could read inputs and outputs in both directions?
# each will be a separate user kernel
#

# each part has two sets of dimensions
# the original element [node, [list of slices]] - The InputSlice - The slices include the original axis number
# the start index of this slice in the new tensors

class InputSlice():
    """ InputSlice encapsulates a single slice of an Input to the copy subgraph.
    It contains the actual slice and a pointer to the input node"""
    def __init__(self, node, tslice) -> None:
        self.node = node
        self._slice = tslice

    @classmethod
    def from_shape(cls, node, tslice):
        """ Creates an input slice from an input node. i.e. the slice is the full
        contents of the input"""
        return cls(
            node,
            tuple(tuple([axis, (0, dim, 1)])
                  for axis, dim in enumerate(tslice)))

    @property
    def rank(self) -> int:
        """ Returns the rank of the slice """
        return len(self._slice)

    @property
    def shape(self) -> int:
        return tuple(math.ceil((sl[0] - (-1 if sl[1] < 0 else sl[1]))/-sl[2])
                     if sl[2] < 0 else
                     math.ceil((sl[1] - sl[0])/sl[2]) for _, sl in self._slice)

    @property
    def original_shape(self) -> int:
        return self.node.dims.shape

    @property
    def dir_start_end(self):
        sorted_shape = sorted(self._slice, key=lambda x: x[0])
        return tuple((sl[1][2] > 0, sl[1][0], sl[1][1]) for sl in sorted_shape)

    @property
    def start_idx(self):
        return reduce(lambda s, x: s * x[0] + x[1][1][0], zip(self.original_shape, self._slice), 0)

    @property
    def end_idx(self):
        return reduce(lambda s, x: s * x[0] + (-1 if x[1][1][1] < 0 else x[1][1][1]), zip(self.original_shape, self._slice), 0)

    def transpose(self, trans) -> None:
        return InputSlice(self.node, do_transpose(trans, self._slice))

    def slice(self, slices):
        """ Slices the current slice """
        x = self.start_idx
        shape = []
        cnts = []
        for idx, sl in enumerate(slices):
            axis, cur_sl = self._slice[idx]
            # start is offset in terms of current step
            start = cur_sl[0] + sl[0] * cur_sl[2]
            # stride is multiplied by current stride
            stride = cur_sl[2] * sl[2]
            # slice is negative i.e. reversed
            if sl[2] < 0:
                if sl[1] < 0:
                    cnt = sl[0] + 1
                else:
                    cnt = sl[0] - sl[1]
                end = start - cnt * cur_sl[2]
                if end < 0:
                    end = -sys.maxsize
                    act_cnt = math.ceil((start + 1) / -stride)
                else:
                    act_cnt = math.ceil((start - end) / -stride)
            else:
                # this is the actual number of items inbetween the bounds
                cnt = sl[1] - sl[0]
                end = min(cur_sl[1], start + cnt * cur_sl[2])
                # how many items could we satisfy
                act_cnt = math.ceil((end - start) / stride)
            shape.append((axis, (start, end, stride)))
            cnts.append(act_cnt)
        return InputSlice(self.node, shape), cnts

    def __repr__(self) -> str:
        str_shape = ','.join(
            f'{axis}:[{",".join(str(dim) for dim in sl)}]' for axis, sl in self._slice)
        return f'({self.node.name}, {str_shape})'


class SliceElement():
    """ SliceElement maps an input slice into a slot in a composed tensor """
    def __init__(self, start: Tuple[int], end: Tuple[int], tensor_slice: InputSlice) -> None:
        self.start = start
        self.end = end
        self.tensor_slice = tensor_slice

    @property
    def rank(self):
        return len(self.start)

    def __repr__(self) -> str:
        return f'({",".join(f"({start},{end})" for start, end in zip(self.start, self.end))}, {self.tensor_slice})'


def overlap(x1, x2, stride, y1, y2):
    if stride < 0:
        # negative indicates that the element is at index 0 inclusive
        temp = 0 if x2 < 0 else x2
        x2 = x1 + 1
        x1 = temp
    return x1 < y2 and y1 < x2


class SlicedTensor():
    """ SlicedTensor is a collection of SliceElements that forms a complete tensor """
    def __init__(self, elements: Sequence[SliceElement]) -> None:
        # all elements must have the same rank
        assert all(
            elem.tensor_slice.rank == elements[0].tensor_slice.rank for elem in elements[1::])
        self.elements = elements

    @classmethod
    def from_input_slice(cls, init):
        return SlicedTensor([SliceElement(tuple([0] * init.rank), init.shape, init)])

    @property
    def shape(self):
        return [max(ends) for ends in zip([elem.end for elem in self.elements])][0]

    def transpose(self, trans) -> None:
        return SlicedTensor([SliceElement(do_transpose(trans, elem.start), do_transpose(trans, elem.end), elem.tensor_slice.transpose(trans))
                             for elem in self.elements])

    @classmethod
    def concat(cls, *args, axis=0):
        # this just adds all the sliced tensors together down an axis
        res, cur_idx = [], 0
        for tensor in args:
            for elem in tensor.elements:
                res.append(
                    SliceElement(
                        tuple(dim if axis != idx else cur_idx
                              for idx, dim in enumerate(elem.start)),
                        tuple(dim if axis != idx else cur_idx + elem.tensor_slice.shape[idx]
                              for idx, dim in enumerate(elem.end)),
                        elem.tensor_slice))
            cur_idx += tensor.shape[axis]
        return SlicedTensor(res)

    def slice(self, slices):
        new_elements = []
        starts = [0] * len(slices)
        for elem in self.elements:
            if any(not overlap(sl[0], sl[1], sl[2], start, end) for sl, start, end in zip(slices, elem.start, elem.end)):
                continue
            # here the slice is inside the range of the segment but we need to adjust it to match the start index
            this_slice = []
            for sl, start, end in zip(slices, elem.start, elem.end):
                # reverse slice on this dimension
                if sl[2] < 0:
                    # is the end of the reverse slice inside this segment
                    if sl[0] < end:
                        sl_start = min(sl[0] - start, end - 1)
                    else:
                        sl_start = sl[0] - start
                        sl_start += sl_start % -sl[2]

                    if sl[1] < 0:
                        sl_end = -sys.maxsize
                    else:
                        sl_end = sl[1] - start
                        if sl_end < 0:
                            sl_end = -sys.maxsize
                    this_slice.append((sl_start, sl_end, sl[2]))
                else:
                    if sl[0] >= start:
                        this_slice.append(
                            (sl[0] - start, sl[1] - start, sl[2]))
                    else:
                        offset = start - sl[0]
                        offset += offset % sl[2]
                        this_slice.append(
                            (offset - start, sl[1] - start, sl[2]))

            new_elem, cnts = elem.tensor_slice.slice(this_slice)
            ends = starts + np.array(cnts)
            new_elements.append(SliceElement(
                list(starts), list(ends), new_elem))
            starts = ends
        return SlicedTensor(new_elements)

    def split(self, slices):
        """ Split is a series of slices with multiple outputs """
        return [self.slice(sl) for sl in slices]


def walk_down(
        subg: GraphView,
        node: Parameters,
        node_slices: Mapping[Parameters, Sequence[SlicedTensor]]):

    # edges not created
    if isinstance(node, FusionInputParameters):
        inp_slice = InputSlice.from_shape(node, node.dims.shape)
        dim_slices = node_slices[node] = [
            SlicedTensor([
                SliceElement(tuple([0] * inp_slice.rank), inp_slice.shape, inp_slice)])]
    else:
        dim_slices = node_slices.get(node)
        if dim_slices is None:
            return
        # all edges not created
        if len(dim_slices) < subg.num_in_edges(node.name):
            return
        # all edges not created
        if any(val is None for val in dim_slices):
            return
        if isinstance(node, TransposeParameters) and node.transpose:
            dim_slices[0] = dim_slices[0].transpose(node.transpose)
        elif isinstance(node, ConcatParameters):
            dim_slices = [SlicedTensor.concat(*dim_slices, axis=node.axis)]
        elif isinstance(node, SplitParameters):
            dim_slices = dim_slices[0].split(node.act_slices)
        elif isinstance(node, StridedSliceParameters):
            dim_slices = [dim_slices[0].slice(node.act_slice)]

    # set output edges
    for edge_set in subg.indexed_out_edges(node.name):
        for edge in edge_set:
            dest_slices = node_slices.setdefault(
                edge.to_node, [None] * (edge.to_idx + 1))
            if len(dest_slices) < edge.to_idx + 1:
                dest_slices = dest_slices + \
                    ([None] * ((edge.to_idx + 1) - len(dest_slices)))
                node_slices[edge.to_node] = dest_slices
            dest_slices[edge.to_idx] = dim_slices[edge.from_idx]

    # explore graph
    for edge_set in subg.indexed_out_edges(node.name):
        for edge in edge_set:
            walk_down(subg, edge.to_node, node_slices)


class CopySet():
    def __init__(self, elements: Sequence[Tuple[Parameters, SlicedTensor]]) -> None:
        self.elements = elements

    @property
    def output_slices(self):
        groups = []
        cur_group = []
        first_index = None
        for out_elem in self.elements:
            sorted_elements = sorted(
                out_elem[1].elements, key=lambda x: tuple(zip(x.start, x.end)))
            for sl_elem in sorted_elements:
                if first_index is None:
                    first_index = sl_elem.start[0]
                if first_index != sl_elem.start[0]:
                    groups.append(tuple(cur_group))
                    cur_group = []
                cur_group.append(sl_elem)
            if cur_group:
                groups.append(tuple(cur_group))
                cur_group = []
        return groups

    @property
    def inputs(self):
        return sorted([
            sl_elem.tensor_slice for out_elem in self.elements
            for sl_elem in out_elem[1].elements],
            key=lambda x: tuple([x.node.name, x.dir_start_end]))

    @property
    def input_slices(self):
        groups = []
        cur_group = []
        first_index = None
        for out_elem in self.elements:
            sorted_elements = sorted(
                out_elem[1].elements, key=lambda x: x.start)
            for sl_elem in sorted_elements:
                if first_index is None:
                    first_index = sl_elem.start[0]
                if first_index != sl_elem.start[0]:
                    groups.append(tuple(cur_group))
                    cur_group = []
                cur_group.append(sl_elem)
            if cur_group:
                groups.append(tuple(cur_group))
                cur_group = []
        return groups


def new_eval_copy(subg: GraphView) -> CopySet:
    node_slices = {}
    for inp in subg.inputs():
        walk_down(subg, inp, node_slices)
    return CopySet([(node, node_slices[node][0]) for node in subg.outputs()])
