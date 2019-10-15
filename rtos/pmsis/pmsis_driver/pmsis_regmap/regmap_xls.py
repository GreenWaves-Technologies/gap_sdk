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

import pandas as pd
import regmap as rmap
import cmdmap as cmap

def import_xls(regmap, cmdmap, path):
    xlsx = pd.ExcelFile(path)

    ipreglist = None
    ipregmap = None
    ipcmdlist = None
    ipcmdmap = None

    for sheet_name in xlsx.sheet_names:

        if sheet_name.find('IPREGLIST') == 0:
            ipreglist = xlsx.parse(sheet_name)
        elif sheet_name.find('IPREGMAP') == 0:
            ipregmap = xlsx.parse(sheet_name)
        elif sheet_name.find('IPINDIRECTREGLIST') == 0:
            ipindirectreglist = xlsx.parse(sheet_name)
        elif sheet_name.find('IPINDIRECTREGMAP') == 0:
            ipindirectregmap = xlsx.parse(sheet_name)
        elif sheet_name.find('IPUDMACMDLIST') == 0:
            ipcmdlist = xlsx.parse(sheet_name)
        elif sheet_name.find('IPUDMACMDMAP') == 0:
            ipcmdmap = xlsx.parse(sheet_name)

    if ipreglist is not None:
        for index, row in ipreglist.iterrows():
            regmap.add_register(
                rmap.Register(
                  name=row['Register Name'],
                  offset=int(row['Address'], 0),
                  width=row['Size'],
                  desc=row['Description']
                )
            )

    if ipregmap is not None:
        for index, row in ipregmap.iterrows():
            reg = regmap.get_register(row['Register'])

            if row['Bit field'] == '-':
                continue

            reg.add_regfield(
                rmap.Regfield(
                    name=row['Bit field'],
                    width=row['Size'],
                    bit=row['Bit Position'],
                    access=row['Host Access Type'],
                    desc=row['Description'],
                    reg_reset=reg.reset
                )
            )

    if ipcmdlist is not None:
        for index, row in ipcmdlist.iterrows():
             cmdmap.add_cmd(
                cmap.Cmd(
                  name=row['Command name'],
                  code=int(row['Command field'],0),
                  width=row['Size'],
                  desc=row['Description']
                )
            )

    if ipcmdmap is not None:
        for index, row in ipcmdmap.iterrows():
            cmd = cmdmap.get_command(row['Command name'])

            if row['Bit position'] == '-':
                continue

            cmd.add_cmdfield(
                cmap.Cmdfield(
                    name=row['Parameter bit field'],
                    width=row['Size'],
                    offset=int(row['Bit position']),
                    value=row['Field value'],
                    desc=row['Description'],
                )
            )
