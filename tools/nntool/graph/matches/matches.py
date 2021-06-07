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
from functools import cmp_to_key

from graph.matches.matcher import Matcher, MatchGroup

from .matchers import *

LOG = logging.getLogger("nntool." + __name__)


def general_validation(match: Matcher):
    if match.DESCRIPTION is None:
        LOG.warning('matcher %s has no description', match.NAME)
    if match.NAME is None:
        raise ValueError(f'match {match.NAME} has no name')
    if '*' in match.RUN_BEFORE and '*' in match.RUN_AFTER:
        raise ValueError(
            f'match {match.NAME} has wildcard in run_before and run_after')
    return match


def get_all_subclasses(cls):
    for subclass in cls.__subclasses__():
        yield from get_all_subclasses(subclass)
        yield subclass


ALL_MATCHERS = [general_validation(match_class) for match_class in get_all_subclasses(Matcher)
                if match_class.NAME is not None]


def select_matchers(group=None):
    return [match_class for match_class in ALL_MATCHERS
            if (group is None or '*' in match_class.GROUPS or group in match_class.GROUPS)]


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
         for match_class in ALL_MATCHERS],
        key=lambda x: x[0])


def get_pow2_match_group():
    return MatchGroup(
        *select_sorted_matcher_instances(group='symmetric'),
        identity="pow2_match_group"
    )


def get_scale8_match_group():
    return MatchGroup(
        *select_sorted_matcher_instances(group='scaled'),
        identity="std_match_group"
    )


def get_fusion(name):
    if name in ["pow2_match_group"]:
        return get_pow2_match_group()
    if name in ["std_match_group", "scale8_match_group"]:
        return get_scale8_match_group()
    match_class = next((match_class for match_class in select_matchers()
                        if match_class.NAME == name), None)
    if match_class is not None:
        return match_class()
    return None
