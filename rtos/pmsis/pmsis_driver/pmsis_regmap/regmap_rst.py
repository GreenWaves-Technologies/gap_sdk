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

import pytablewriter

class Rst_file(object):
    def __init__(self, name, path):
        self.file = open(path, "w")

    def close(self):
        self.file.close()

    def get_file(self):
        return self.file


class Regfield(object):

    def get_row(self):
        if self.width == 1:
            bit = '%d' % self.bit
        else:
            bit = '%d:%d' % (self.bit + self.width - 1, self.bit)

        return [bit, self.access, self.name, self.desc]

    def dump_to_rst(self, rst):
        rst.append(self.get_row())



class Register(object):

    def dump_to_rst(self, rst):
        writer = pytablewriter.RstGridTableWriter()
        writer.table_name = self.name
        writer.header_list = ['Bit #', 'R/W', 'Name', 'Description']

        table = []
        for name, field in self.fields.items():
            field.dump_to_rst(table)

        writer.value_matrix = table

        writer.stream = rst.get_file()
        writer.write_table()



class Regmap(object):

    def dump_to_rst(self, rst):
        for name, register in self.registers.items():
            register.dump_to_rst(rst)



def dump_to_rst(regmap, name, rst_path):
    rst_file = Rst_file(name, rst_path)
    regmap.dump_to_rst(rst_file)
    rst_file.close()
