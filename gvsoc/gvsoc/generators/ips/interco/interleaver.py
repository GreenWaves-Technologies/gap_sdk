#
# Copyright (C) 2020 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import gsystree as st

class Interleaver(st.Component):
    """
    Memory interleaver

    Can be used to dispatch accesses to memory banks based on access address MSB.

    Attributes
    ----------
    nb_slaves : int
        The number of slave ports where the accesses will be dispatched.
    interleaving_bits : int
        Number of interleaving bits, i.e. the size in bits of the accesseses dispatched
        to banks.
    stage_bits : int, optional
        Number of bits used for dispatching the accesses. If it is not specified, it is
        assigned log2(nb_slaves).
    remove_offset: int, optional
        Specify an offset to be removed to the incoming access address when it is
        dispatched (default: 0).
    
    """

    def __init__(self, parent, name, nb_slaves: int, interleaving_bits: int, stage_bits: int=0, remove_offset: int=0):

        super(Interleaver, self).__init__(parent, name)

        self.add_properties({
            'vp_component': 'interco.interleaver_impl',
            'nb_slaves': nb_slaves,
            'interleaving_bits': interleaving_bits,
            'stage_bits': stage_bits,
            'remove_offset': remove_offset,
        })
