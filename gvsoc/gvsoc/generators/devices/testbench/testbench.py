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

import gsystree as st
from ips.clock.clock_domain import Clock_domain


class Testbench(st.Component):
    """
    Testbench

    This instantiates a testbench which can be controlled from simulated SW or proxy to
    interact with a chip.

    Attributes
    ----------
    uart : list
        List of UART interfaces which must be connected to the testbench

    i2s : list
        List of I2S interfaces which must be connected to the testbench

    """

    def __init__(self, parent, name, uart=[], i2s=[]):
        super(Testbench, self).__init__(parent, name)

        # Testbench implementation as this component is just a wrapper
        testbench = Testbench.Testbench_implem(self, 'testbench')

        # The testbench needs its owm cloc domain to enqueue clock events
        clock = Clock_domain(self, 'clock', frequency=50000000)

        self.bind(clock, 'out', testbench, 'clock')

        # UARTS
        for i in uart:
            # Each uart needs its own clock domain to enqueue clock events
            # Their frequency will changed by the uart models
            uart_clock = Clock_domain(self, 'uart%d_clock' % i, frequency=50000000)
            uart_tx_clock = Clock_domain(self, 'uart%d_tx_clock' % i, frequency=50000000)

            self.bind(self, 'uart%d' % i, testbench, 'uart%d' % i)

            self.bind(uart_clock, 'out', testbench, 'uart%d_clock' % i)
            self.bind(testbench, 'uart%d_clock_cfg' % i, uart_clock, 'clock_in')
            
            self.bind(uart_tx_clock, 'out', testbench, 'uart%d_tx_clock' % i)
            self.bind(testbench, 'uart%d_tx_clock_cfg' % i, uart_tx_clock, 'clock_in')

        # I2S
        for i in i2s:
            self.bind(testbench, 'i2s%d' % i, self, 'i2s%d' % i)


    class Testbench_implem(st.Component):

        def __init__(self, parent, name, uart_id=0, uart_baudrate=115200):
            super(Testbench.Testbench_implem, self).__init__(parent, name)

        # Register all parameters as properties so that they can be overwritten from the command-line
            self.add_property('uart_id', uart_id)
            self.add_property('uart_baudrate', 115200)

            self.add_properties({
                'vp_component': 'devices.testbench.testbench',
                "verbose": False,
                "ctrl_type": "uart",
                "nb_gpio": 90,
                "nb_spi": 5,
                "nb_uart": 5,
                "nb_i2c": 3,
                "nb_i2s": 3,

                "spislave_boot": {
                    "enabled": False,
                    "delay_ps": "1000000000",
                    "itf": 4,
                    "frequency": 50000000,
                    "stim_file": None
                }
            })
