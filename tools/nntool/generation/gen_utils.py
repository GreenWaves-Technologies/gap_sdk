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

import os

from utils.exception import NNToolInternelError


def at_bits(qtype):
    if qtype is None:
        return 0
    if qtype.bits not in [8, 16, 32]:
        raise NotImplementedError("unsupported number of bits")
    size = qtype.bits // 8
    if not qtype.signed:
        return -size
    return size


def at_q(qtype):
    if qtype is None:
        return 0
    return qtype.q


def at_bits_and_q(qtype):
    return "{}, {}".format(at_bits(qtype), qtype.q)

def write_empty(model_directory, model_file, comment):
    model_path = os.path.join(model_directory, model_file)
    with open(model_path, "w") as output_fp:
        output_fp.write(f"/** {comment}\n**/")

class ModelGenerationInternalError(NNToolInternelError):
    pass
