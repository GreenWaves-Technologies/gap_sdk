# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from .remove_unused_concats import RemoveUnusedConcats
from .match_gap_conv import MatchAllGapConv
from .fuse_pad import MatchFusePad
from .expand_transposes import ExpandTransposesMatcher
from .move_activation import MoveActivationsMatcher
from .match_gap_linear import MatchGapLinear
from .match_gap_pool import MatchGapPool

from .matcher import MatchGroup

ALL_MATCH_CLASSES = [MatchFusePad, RemoveUnusedConcats,
                     MoveActivationsMatcher, MatchAllGapConv, MatchGapPool,
                     MatchGapLinear, ExpandTransposesMatcher]
STD_MATCH_CLASSES = [MatchFusePad, RemoveUnusedConcats,
                     MoveActivationsMatcher, MatchAllGapConv, ExpandTransposesMatcher]

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
