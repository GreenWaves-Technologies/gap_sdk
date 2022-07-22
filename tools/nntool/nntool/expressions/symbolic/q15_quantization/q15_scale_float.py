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

from ..function import Function
from ..symbol import nargs, Constant, c_headers, copy_props

@nargs(1)
@c_headers('"Gap.h"')
@copy_props('to_qrec')
class Q15ScaleFloat(Function):
    def __init__(self, *args, to_qrec=None, **kwargs):
        self._to_qrec = to_qrec
        super().__init__(*args, **kwargs)

    @property
    def to_qrec(self):
        return self._to_qrec

    def _eval(self, *args, **kwargs):
        if isinstance(args[0], Constant):
            return Constant(self._to_qrec.quantize_and_clip(args[0].value))
        return Q15ScaleFloat(args[0], to_qrec=self._to_qrec)

    def _impl(self, *args, **kwargs):
        val = args[0]
        return self._to_qrec.quantize_and_clip(val)

    def _py_expr(self, *args, **kwargs):
        val = args[0]
        return self._to_qrec.quantize_and_clip_py_expr(val)

    def _c_expr(self, *args, **kwargs):
        val = args[0]
        return self._to_qrec.quantize_and_clip_c_expr(val)

    def __repr__(self) -> str:
        return f"ScaleFloat({self.contents[0]}, ->[{self._to_qrec}])"
