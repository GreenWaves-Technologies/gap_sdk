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

import math
import texttable
from importer.onnx.common.handler_helper import get_opset_status
from interpreter.nntool_shell_base import NNToolShellBase

class HelpOnnxCommand(NNToolShellBase):
    def help_onnx(self):
        ops, counts_by_domain = get_opset_status()
        table = texttable.Texttable()
        table.set_cols_align(['l', 'l', 'l', 'l'])
        table.set_max_width(120)
        table.set_cols_width([15, 20, 15, 60])
        table.add_rows(ops)
        self.pfeedback("Supported operators and versions")
        self.pfeedback(table.draw()+'\n')
        self.pfeedback(f"{counts_by_domain[''][0]}/{counts_by_domain[''][1]} operators {math.ceil(100*counts_by_domain[''][0]/counts_by_domain[''][1])}% coverage")
