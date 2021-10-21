#
# Copyright (C) 2021 GreenWaves Technologies, SAS
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

from gap.gap9.gap9_evk import Gap9_evk
from devices.i2c.i2c_bus import I2c_bus
from devices.i2c.eeprom import Eeprom
from devices.i2c.corruptor import Corruptor
import gsystree as st

nb_gap9 = 1

# Declare a board derived from gap9 board
class My_board(st.Component):

    def __init__(self, parent, name):
        super(My_board, self).__init__(parent, name)

        i2c_bus = I2c_bus(self, 'i2c_bus')

        for i in range(0, nb_gap9):
            board = Gap9_evk(self, 'board' + str(i))
            self.bind(board, 'i2c0', i2c_bus, 'input')

        i2c_periphs = []
        for i in range(0, 10):
            address = 0x20 + 5*i
            eeprom = Eeprom(self, 'eeprom' + str(i), address=address)
            i2c_periphs.append(eeprom)

        #i2c_periphs.append(Corruptor(self, 'corruptor'))

        for i2c_periph in i2c_periphs:
            self.bind(i2c_periph, 'i2c', i2c_bus, 'input')
