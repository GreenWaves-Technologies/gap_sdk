#
# Copyright (C) 2018 ETH Zurich, University of Bologna
# and GreenWaves Technologies
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

# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)


from prettytable import PrettyTable


class Regfield(object):

    def dump_to_table(self, table):
        if self.width == 1:
            bit = '%d' % self.bit
        else:
            bit = '%d:%d' % (self.bit + self.width - 1, self.bit)

        row = ['', '', '', ''] + [bit, self.access, self.name, self.desc]

        table.add_row(row)



class Register(object):

    def dump_to_table(self, table, dump_regfields=True):
        row = [self.get_full_name(), '0x%x' % self.get_offset(), self.width, self.desc]
        if dump_regfields:
            row += ['', '', '', '']

        table.add_row(row)

        if dump_regfields:
            for name, field in self.fields.items():
                field.dump_to_table(table)



class Regmap(object):

    def dump_to_table(self, dump_regfields=True):
        rows = ['Name', 'Offset', 'Width', 'Description']
        if dump_regfields:
            rows += ['Field bit #', 'Field R/W', 'Field name', 'Field description']

        table = PrettyTable(rows)
        table.align = 'l'

        for name, register in self.registers.items():
            register.dump_to_table(table, dump_regfields=dump_regfields)

        print (table)
