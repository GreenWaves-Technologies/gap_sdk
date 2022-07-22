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

from itertools import chain

from nntool.graph.dim import PadDim


class PadMixin(object):
    @classmethod
    def mix_pads(cls, val):
        if len(val) == 0:
            pads_start = [0, 0]
            pads_end = [0, 0]
        elif len(val) == 2:
            pads_start = [0, val[0]]
            pads_end = [0, val[1]]
        elif len(val) == 4:
            pads_start = val[:2]
            pads_end = val[2:]
        else:
            raise ValueError('unexpected pad lenght')

        return list(chain(*zip(pads_start, pads_end)))

    @classmethod
    def pad_start_with(cls, val, pad_val, dlen):
        return pad_val * (dlen - len(val)) + val

    @classmethod
    def calc_pad_dim(cls, node, expected_len):
        if "auto_pad" not in node.attrs or node.attrs["auto_pad"] == "NOTSET":
            pad_dim = PadDim(*cls.mix_pads(node.attrs.get("pads", [])))
        elif node.attrs["auto_pad"] == "VALID":
            pad_dim = PadDim.valid()
        elif node.attrs["auto_pad"] == "SAME_UPPER":
            pad_dim = PadDim.same(same_type="balanced_left")
        elif node.attrs["auto_pad"] == "SAME_LOWER":
            pad_dim = PadDim.same(same_type="balanced_right")
        else:
            raise ValueError("bad pad type")
        return pad_dim
