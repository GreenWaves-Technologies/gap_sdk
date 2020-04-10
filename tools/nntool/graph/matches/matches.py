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

from .remove_unused_concats import RemoveUnusedConcats
from .match_gap_conv import MatchAllGapConv
from .fuse_pad import MatchFusePad
from .expand_transposes import ExpandTransposesMatcher
from .move_activation import MoveActivationsMatcher
from .match_gap_linear import MatchGapLinear
from .match_gap_pool import MatchGapPool
from .match_external_bias import MatchExternalBias
from .matscale import FuseMatScalePair, FuseMatScale

from .matcher import MatchGroup

ALL_MATCH_CLASSES = [MatchExternalBias, MatchFusePad, RemoveUnusedConcats,
                     MoveActivationsMatcher, MatchAllGapConv, MatchGapPool,
                     MatchGapLinear, ExpandTransposesMatcher, FuseMatScalePair, FuseMatScale]
STD_MATCH_CLASSES = [MatchExternalBias, MatchFusePad, RemoveUnusedConcats,
                     MoveActivationsMatcher, MatchAllGapConv, ExpandTransposesMatcher,
                     FuseMatScalePair, FuseMatScale]

FUSION_LIST = [((match_class.NAME, match_class.DESCRIPTION), match_class())
               for match_class in ALL_MATCH_CLASSES]


def get_fusions():
    return [(match_class.NAME, match_class.DESCRIPTION) for match_class in ALL_MATCH_CLASSES]


def get_std_match_group():
    return MatchGroup(
        *[match_class() for match_class in STD_MATCH_CLASSES],
        identity="std_match_group"
    )


def get_fusion(name):
    if name == "std_match_group":
        return get_std_match_group()
    match_class = next((match_class for match_class in ALL_MATCH_CLASSES
                        if match_class.NAME == name), None)
    if match_class is not None:
        return match_class()
    return None
