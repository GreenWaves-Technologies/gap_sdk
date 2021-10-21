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

from gap.gap9.gapmod import Gapmod
from devices.testbench.testbench import Testbench
from devices.uart.uart_checker import Uart_checker


class Gap9_evk(Gapmod):
    """
    GAP9 EVK board

    Attributes
    ----------
    addon_testbench_enabled : bool, optional
        If True, this enables the testbench addon, which is a specific GVSOC addon for
        generating stimuli on the pads (default: False).
    
    """

    def __init__(self, parent, name, addon_testbench_enabled: bool=False, addon_uart_checker: bool=True):
        super(Gap9_evk, self).__init__(parent, name)

        # Register all parameters as properties so that they can be overwritten from the command-line
        self.add_property('addon_testbench_enabled', addon_testbench_enabled)
        self.add_property('addon_uart_checker', addon_uart_checker)

        gap = self.get_component('chip')

        # Addon Testbench
        if self.get_property('addon_testbench_enabled'):

            # The testbench is replacing all devices so that stimuli can be driven
            # from simulated code or from proxy
            testbench = Testbench(self, 'testbench', uart=[0, 1, 2, 3, 4], i2s=[0, 1, 2])

            self.bind(gap, 'uart0', testbench, 'uart0')
            self.bind(gap, 'uart1', testbench, 'uart1')
            self.bind(gap, 'uart2', testbench, 'uart2')
            self.bind(gap, 'uart3', testbench, 'uart3')
            self.bind(gap, 'uart4', testbench, 'uart4')

            self.bind(testbench, 'i2s0', gap, 'i2s0')
            self.bind(testbench, 'i2s1', gap, 'i2s1')
            self.bind(testbench, 'i2s2', gap, 'i2s2')

        else:
            # Addon uart checker
            if self.get_property('addon_uart_checker'):
                uart_checker = Uart_checker(self, 'uart_checker')
                self.bind(gap, 'uart0', uart_checker, 'input')

                self.bind(gap, 'i2c0', self, 'i2c0')
                self.bind(gap, 'i2c1', self, 'i2c1')
                self.bind(gap, 'i2c2', self, 'i2c2')
                self.bind(gap, 'i2c3', self, 'i2c3')

        

