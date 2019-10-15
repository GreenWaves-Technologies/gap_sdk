from .remove_unused_concats import RemoveUnusedConcats
from .match_gap_conv import MatchAllGapConv
from .fuse_pad import MatchFusePad

from .matcher import MatchGroup

FUSION_LIST = [
    (("fuse_pad", "Fuse pad operation to subsequent Convolution or Pool"), MatchFusePad()),
    (("unused_concats", "Remove concats that only have one input"), RemoveUnusedConcats()),
    (("fuse_gap_convs", "Fuse convolutions, pools and activations to match GAP AutoTiler operations"), MatchAllGapConv()),
]

def get_fusions():
    return [elem[0] for elem in FUSION_LIST]

def get_fusion(name):
    return next((fusion for desc, fusion in FUSION_LIST if desc[0] == name), None)

def get_std_match_group():
    return MatchGroup(
        MatchFusePad(),
        RemoveUnusedConcats(),
        MatchAllGapConv(),
        identity="std_match_group"
    )
