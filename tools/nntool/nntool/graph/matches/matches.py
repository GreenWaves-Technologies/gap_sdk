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

# pylint: disable=wildcard-import,unused-wildcard-import

import logging

from nntool.graph.matches.matcher import Matcher, MatchGroup, match_name, description
from nntool.utils.subclasses import get_all_subclasses

from .matchers import *

LOG = logging.getLogger(__name__)

ALL_MATCHERS = {}


def select_matchers(group=None):
    return [match_class for match_class in ALL_MATCHERS.values()
            if ('*' in match_class.GROUPS or group in match_class.GROUPS)]


def order_matchers(matchers):
    first_matchers = [match for match in matchers if '*' in match.RUN_BEFORE]
    last_matchers = [match for match in matchers if '*' in match.RUN_AFTER]
    rest = [match for match in matchers if match not in first_matchers +
            last_matchers]
    rest_sorted = []
    while rest:
        matcher = rest.pop(0)
        pos = 0
        for omatch in rest_sorted:
            if matcher.NAME in omatch.RUN_AFTER:
                break
            if omatch.NAME in matcher.RUN_BEFORE:
                break
            pos += 1
        rest_sorted.insert(pos, matcher)
    return first_matchers + rest_sorted + last_matchers


def select_sorted_matchers(group=None):
    return order_matchers(select_matchers(group=group))


def select_sorted_matcher_instances(group=None):
    return [matcher() for matcher in select_sorted_matchers(group=group)]


def get_fusions():
    return sorted(
        [(match_class.NAME, match_class.DESCRIPTION)
         for match_class in ALL_MATCHERS.values()],
        key=lambda x: x[0])


@match_name("pow2_match_group")
@description("a selection of matches that are relevant for POW2 quantized graphs")
class POW2MatchGroup(MatchGroup):
    def __init__(self):
        super().__init__(*select_sorted_matcher_instances(group='symmetric'),
                         identity='pow2_match_group')


@match_name("scaled_match_group")
@description("a selection of matches that are relevant for scaled quantized graphs")
class ScaledMatchGroup(MatchGroup):
    def __init__(self):
        super().__init__(*select_sorted_matcher_instances(group='scaled'),
                         identity='scaled_match_group')

@match_name("float_match_group")
@description("a selection of matches that are relevant for scaled quantized graphs")
class FloatMatchGroup(MatchGroup):
    def __init__(self):
        super().__init__(*select_sorted_matcher_instances(group='scaled'),
                         identity='float_match_group')


def get_pow2_match_group():
    return POW2MatchGroup()


def get_scale8_match_group():
    return ScaledMatchGroup()


def get_matches(*match_names, identity="custom"):
    not_found = set(match_names) - set(ALL_MATCHERS)
    if not_found:
        raise ValueError(f'matches {" ".join(not_found)} not found')
    return MatchGroup(*[ALL_MATCHERS[name]() for name in match_names], identity=identity)


def get_fusion(name):
    if name in ["pow2_match_group"]:
        return get_pow2_match_group()
    if name in ["std_match_group", "scale8_match_group"]:
        return get_scale8_match_group()
    if name in ALL_MATCHERS:
        return ALL_MATCHERS[name]()
    return None


ALL_MATCHERS.update({match_class.NAME: match_class for match_class in get_all_subclasses(Matcher)
                     if match_class.NAME is not None})
