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

from .equalize_sym_mult_concats import \
    EqualizeSymmetricMultiplicativeQuantivedConcats
from .expand_transposes import ExpandTransposesMatcher
from .filt_bigger_than_in import FilterBiggerThanInput
from .find_asymmetric_quantization import FindAsymmetricQuantization
from .find_hsigmoid import MatchCloseHSigmoid, MatchFarHSigmoid
from .find_missing_quantization import FindMissingQuantization
from .fuse_pad import MatchFusePad
from .insert_copies import MatchInsertCopies
from .match_external_bias import MatchExternalBias, MatchExternalBiasSQ8
from .match_gap_conv import MatchAllGapConv
from .match_gap_linear import MatchGapLinear
from .match_gap_pool import MatchGapPool
from .match_op_activation import (MatchOpActivationPow2Kernels,
                                  MatchOpActivationScaleKernels)
from .match_rnn_unpack import MatchRnnUnpack
from .matcher import MatchGroup
from .matscale import FuseMatScale, FuseMatScalePair
from .move_node_up import (MoveActivationsMatcherPow2,
                           MoveActivationsMatcherScale8,
                           MoveMaxPoolMatcherScale8)
from .propagate_softmax_sym_mult_qrec import PropagateSoftmaxSymQrec
from .remove_noops import RemoveNoOPs
from .remove_relus import RemoveRelusMatch
from .remove_unused_concats import RemoveUnusedConcats

ALL_MATCH_CLASSES = [MatchRnnUnpack, RemoveRelusMatch, RemoveNoOPs, MatchExternalBias,
                     MatchFusePad, RemoveUnusedConcats,
                     FindMissingQuantization, MatchFarHSigmoid, MatchCloseHSigmoid, MoveMaxPoolMatcherScale8,
                     MoveActivationsMatcherScale8, MoveActivationsMatcherPow2,
                     EqualizeSymmetricMultiplicativeQuantivedConcats,
                     MatchAllGapConv, MatchGapPool, MatchOpActivationScaleKernels,
                     MatchOpActivationPow2Kernels, FilterBiggerThanInput,
                     MatchGapLinear, ExpandTransposesMatcher, FindAsymmetricQuantization,
                     FuseMatScalePair, FuseMatScale, MatchInsertCopies]
POW2_MATCH_CLASSES = [RemoveRelusMatch, RemoveNoOPs, MatchExternalBias, MatchFusePad,
                      RemoveUnusedConcats, FindMissingQuantization, MatchCloseHSigmoid,
                      MoveActivationsMatcherPow2, ExpandTransposesMatcher, MatchAllGapConv, MatchGapLinear,
                      EqualizeSymmetricMultiplicativeQuantivedConcats, FilterBiggerThanInput,
                      MatchInsertCopies]
SCALE8_MATCH_CLASSES = [RemoveRelusMatch, RemoveNoOPs, MatchExternalBiasSQ8, MatchFusePad,
                        RemoveUnusedConcats, FindMissingQuantization,
                        MatchRnnUnpack,
                        MatchFarHSigmoid, MatchCloseHSigmoid, MoveMaxPoolMatcherScale8,
                        MoveActivationsMatcherScale8, ExpandTransposesMatcher,
                        MatchAllGapConv, MatchGapLinear, MatchOpActivationScaleKernels, PropagateSoftmaxSymQrec,
                        EqualizeSymmetricMultiplicativeQuantivedConcats, FilterBiggerThanInput,
                        MatchInsertCopies]

FUSION_LIST = [((match_class.NAME, match_class.DESCRIPTION), match_class())
               for match_class in ALL_MATCH_CLASSES]


def get_fusions():
    return [(match_class.NAME, match_class.DESCRIPTION) for match_class in ALL_MATCH_CLASSES]


def get_pow2_match_group():
    return MatchGroup(
        *[match_class() for match_class in POW2_MATCH_CLASSES],
        identity="pow2_match_group"
    )


def get_scale8_match_group():
    return MatchGroup(
        *[match_class() for match_class in SCALE8_MATCH_CLASSES],
        identity="std_match_group"
    )


def get_fusion(name):
    if name in ["pow2_match_group"]:
        return get_pow2_match_group()
    if name in ["std_match_group", "scale8_match_group"]:
        return get_scale8_match_group()
    match_class = next((match_class for match_class in ALL_MATCH_CLASSES
                        if match_class.NAME == name), None)
    if match_class is not None:
        return match_class()
    return None
