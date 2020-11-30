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

from interpreter.nntool_shell_base import NNToolShellBase
from importer.tflite2.common.handler_helper import get_backend_coverage, get_backend_partial_support_detail

class HelpTFLiteCommand(NNToolShellBase):
    def help_tflite(self):
        ops_dict = get_backend_coverage()[0]
        bc_dict = get_backend_partial_support_detail()
        self.pfeedback("Supported operators and versions")
        for op in ops_dict:
            self.pfeedback("%s (%s)"%(op, ",".join(str(ver) for ver in ops_dict[op])))
            if op in bc_dict:
                self.pfeedback(bc_dict[op])
        