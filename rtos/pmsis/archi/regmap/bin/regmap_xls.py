#
# Copyright (C) 2019 GreenWaves Technologies
#

import pandas as pd
import regmap as rmap

def get_name(field):
    if field[0].isdigit():
        field = '_' + field
    return field.replace(' ', '_')

def get_description(field):
    return field.replace('\n', ' ').replace('\r', ' ')

def import_xls(regmap, path):
    xlsx = pd.ExcelFile(path)

    ipinfo = None
    ipreglist = None
    ipregmap = None
    ipfsmfmtmap = None

    for sheet_name in xlsx.sheet_names:

        if sheet_name.find('Info') == 0:
            ipinfo = xlsx.parse(sheet_name)
        elif sheet_name.find('IPREGLIST') == 0:
            ipreglist = xlsx.parse(sheet_name)
        elif sheet_name.find('IPREGMAP') == 0:
            ipregmap = xlsx.parse(sheet_name)
        elif sheet_name.find('IPFSMFMTMAP') == 0:
            ipfsmfmtmap = xlsx.parse(sheet_name)

        #if ipinfo is not None:
        #    for index, row in ipinfo.iterrows():
        #        if row[1] == "IP type":
        #            regmap.name = row[2]
        #            print ("Regmap name from xls %s" % regmap.name)

    if ipreglist is not None:
        for index, row in ipreglist.iterrows():

            if not isinstance(row['Register Name'], str):
                continue

            reset_val = row.get('Default Value')

            if reset_val is not None:
                if not isinstance(reset_val, int):
                    try:
                        reset_val = int(row['Default Value'], 0)
                    except:
                        reset_val = None

            regmap.add_register(
                rmap.Register(
                  name=str(row['Register Name']),
                  offset=int(row['Address'], 0),
                  width=row['Size'],
                  desc=str(row['Description']),
                  reset=reset_val
                )
            )
            regmap.range=str(int(row['Address'], 0))

    if ipregmap is not None:
        for index, row in ipregmap.iterrows():
            reg_name = row['Register']

            reg = regmap.get_register(reg_name)

            if row['Bit field'] == '-':
                continue

            if reg is None:
                raise Exception("Found bitfield for unknown register: " + reg_name)

            reset_val = row.get('Reset Value')

            if reset_val is not None:
                if not isinstance(reset_val, int):
                    try:
                        if isinstance(reset_val, str):
                            reset_val = int(row['Reset Value'], 0)
                        else:
                            reset_val = int(row['Reset Value'])
                    except:
                        reset_val = None

            reg.add_regfield(
                rmap.Regfield(
                    name=str(row['Bit field']),
                    width=int(row['Size']),
                    bit=int(row['Bit Position']),
                    access=str(row['Host Access Type']),
                    desc=get_description(str(row['Description'])),
                    reg_reset=reg.reset,
                    reset=reset_val
                )
            )

    # TODO this should be reworked, it should not be stored in the registers
    # if ipfsmfmtmap is not None:
        # for index, row in ipfsmfmtmap.iterrows():
            # reg_name = '%s_%s' % (row['Register name'], row['Format Name'])
# 
            # reg = regmap.get_register(reg_name)
# 
            # if reg is None:
                # reg = regmap.add_register(
                    # rmap.Register(
                    #   name=reg_name,
                    #   offset=None,
                    #   width=row['Size'],
                    #   desc=get_description(row['Description'])
                    # ))
# 
            # if row['Bit field'] == '-':
                # continue
# 
            # reg.add_regfield(
                # rmap.Regfield(
                    # name=get_name(row['Bit field']),
                    # width=int(row['Size']),
                    # bit=int(row['Bit Position']),
                    # access=str(row['Host Access Type']),
                    # desc=get_description(row['Description']),
                    # reg_reset=reg.reset
                # )
            # )
