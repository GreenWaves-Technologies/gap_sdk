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

def gen_constant(gen, pnode, cache_node, const_type, extra_name=''):
    cname = gen.naming_convension.get_global_name(pnode.name, pnode.step_idx,
                                                  pnode, const_type)
    cname = cname + extra_name
    gen.name_cache.set(cache_node, const_type, cname)
    file_name = os.path.join(gen.opts['tensor_directory'],
                             cname+".tensor")
    return cname, file_name