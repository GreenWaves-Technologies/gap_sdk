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


class Uart_checker(st.Component):

    def __init__(self, parent, name):
        super(Uart_checker, self).__init__(parent, name)

        uart_checker = Uart_checker.Uart_checker_implem(self, 'uart_checker')

        clock = Clock_domain(self, 'clock', frequency=50000000)

        self.bind(self, 'input', uart_checker, 'input')
        self.bind(clock, 'out', uart_checker, 'clock')
        self.bind(uart_checker, 'clock_cfg', clock, 'clock_in')


    class Uart_checker_implem(st.Component):

        def __init__(self, parent, name):
            super(Uart_checker.Uart_checker_implem, self).__init__(parent, name)

            self.add_property('verbose', False)
            self.add_property('baudrate', 115200)
            self.add_property('loopback', False)
            self.add_property('stdout', True)
            self.add_property('stdin', False)
            self.add_property('telnet', False)
            self.add_property('tx_file', 'tx_uart.log')
            self.add_property('vp_component', 'devices.uart.uart_checker')

