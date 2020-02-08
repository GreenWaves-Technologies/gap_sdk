# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from utils.sparse_list import SparseList

def test1():
    sl = SparseList()
    sl[2] = True
    assert sl[1] is None
    assert len(sl) == 3
    assert sl[2] == True
    sl[5] = False
    assert len(sl) == 6
    assert sl[5] == False
    del sl[2]
    assert len(sl) == 5
    assert sl[2] is None
    assert sl[4] == False
    tl = [v for v in sl]
    assert tl == [None, None, None, None, False]
