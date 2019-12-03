from .remove_unused_concats import RemoveUnusedConcats
from .match_gap_conv import MatchAllGapConv
from .fuse_pad import MatchFusePad

from .matcher import MatchGroup

ALL_MATCH_CLASSES = [MatchFusePad, RemoveUnusedConcats, MatchAllGapConv]
STD_MATCH_CLASSES = [MatchFusePad, RemoveUnusedConcats, MatchAllGapConv]

FUSION_LIST = [((match_class.NAME, match_class.DESCRIPTION), match_class())\
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
    match_class = next((match_class for match_class in ALL_MATCH_CLASSES\
        if match_class.NAME == name), None)
    if match_class is not None:
        return match_class()
    return None
