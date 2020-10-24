#
# Copyright (C) 2019 GreenWaves Technologies
#

import xml.etree.ElementTree as ET
from xml.dom.minidom import *
from collections import OrderedDict
import re

########################################
# Maping between XML namespace prefix and URI
# Move spirit from 1685-2009 to 1.2 because of spirit2regbank
NS_MAP = {
        'spirit': 'http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009',
        'xsi' : 'http://www.w3.org/2001/XMLSchema-instance',
        'xsi:schemaLocation' : 'http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009 http://www.accellera.org/images/xmlschema/spirit/1685-2009/index.xsd',
        }

########################################
for (prefix,uri) in NS_MAP.items():
    ET.register_namespace(prefix,uri)

########################################
# Create Qname useful for schema parsing
def spirit(tag):
    return str(ET.QName(NS_MAP['spirit'], tag))

########################################
def vendorExtensions(tag):
    return str(ET.QName(NS_MAP['vendorExtensions'], tag))

########################################
def indent(elem, level=0):
    if elem == None:
        return
    i = "\n" + level* "\t"
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "\t"
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

########################################
def to_s(xml):
    txt = indent(xml)
    return ET.tostring(xml).decode('utf-8')

########################################
def addSubEl(parentEl, childname, childtext):
    child = ET.SubElement(parentEl, spirit(childname))
    child.text = childtext

########################################
def addSubEl2givenEl(parentEl, childname, grdchildEl):
    for childEl in parentEl.iter(spirit(childname)):
        childEl.append(grdchildEl)

########################################
def addSubEl2givenElbyname(parentEl, childname, grdchildname, grdchildtext):
    grdchildEl = ET.Element(spirit(grdchildname))
    grdchildEl.text = grdchildtext
    addSubEl2givenEl(parentEl, childname, grdchildEl)

########################################
class Regfield(object):
    
    # IPXACT hierarchy for field section (list not exhaustive)
    ipxact__names_text = OrderedDict()
    ipxact__names_text["name"] = ""
    ipxact__names_text["bitOffset"] = ""
    ipxact__names_text["bitWidth"] = ""
    ipxact__names_text["access"] = ""
    
    def printFieldInfo(self):
        print("field: name="+self.name+" width="+str(self.width)+"\n")
    
    def genXmlSection(self):
        # Create field section
        fieldEl = ET.Element(spirit("field"))
        for name, text in self.ipxact__names_text.items():
            subelement = ET.Element(spirit(name))
            if name == "name":
                subelement.text = self.name
            elif name == "bitOffset":
                subelement.text = str(int(self.bit))
            elif name == "bitWidth":
                subelement.text = str(int(self.width))
            elif name == "access":
                a = self.access.replace('/','').replace('-','').lower()
                if a == 'rw':
                    subelement.text = 'read-write'
                elif a == 'r':
                    subelement.text = 'read-only'
                elif a == 'w':
                    subelement.text = 'write-only'
                else:
                    raise RuntimeError('register \'{}\' has unknown access \'{}\''.format(self.name, self.access))
            else:
                subelement.text = text
            fieldEl.append(subelement)
        return fieldEl


class Register(object):
    
    # IPXACT hierarchy for register section (list not exhaustive)
    ipxact__names_text = OrderedDict()
    ipxact__names_text["name"] = ""
    ipxact__names_text["addressOffset"] = ""
    ipxact__names_text["size"] = ""
    ipxact__names_text["reset"] = ""
    
    def printFieldInfo(self):
        for name, field in self.fields.items():
            field.printFieldInfo()
    
    def genXmlSection(self):
        # Create register section
        regEl = ET.Element(spirit("register"))
        for name, text in self.ipxact__names_text.items():
            subelement = ET.Element(spirit(name))
            if name == "name":
                subelement.text = self.name
            elif name == "addressOffset":
                subelement.text = str(int(self.offset)//4) # to be Word aligned
            elif name == "reset":
                addSubEl(subelement, "value", str(self.reset))
                addSubEl(subelement, "mask", "0xffffffff")
            elif name == "size":
                subelement.text = str(self.width)
            else:
                subelement.text = text
            regEl.append(subelement)
        # Populate with its fields sections
        for name, field in self.fields.items():
            regEl.append(field.genXmlSection())
        return regEl


class Regmap(object):
    
    # IPXACT hierarchy for addressBlock section (hook-up to regmap, list not exhaustive)
    ipxact__names_text = OrderedDict()
    ipxact__names_text["name"] = ""
    ipxact__names_text["baseAddress"] = "0x0"
    ipxact__names_text["typeIdentifier"] = "register"
    ipxact__names_text["range"] = ""
    ipxact__names_text["width"] = "32"
    
    def browse_regmap(self):
        for name, register in self.registers.items():
            print("reg_name="+name+"\n")
            for name, field in register.fields.items():
                print("field: name="+field.name+" width="+str(field.width)+"\n")
    
    def genXmlSection(self):
        # Create addressBlock section
        regmapEl = ET.Element(spirit("addressBlock"))
        for name, text in self.ipxact__names_text.items():
            subelement = ET.Element(spirit(name))
            if name == "name":
                subelement.text = self.name+"_blk0"
            elif name == "addressOffset":
                subelement.text = self.offset
            elif name == "range":
                if self.range is None:
                    self.range = 0

                    for register in self.registers.values():
                        addr = register.offset + int((register.width+7)/8)
                        if addr > self.range:
                            self.range = addr

                subelement.text = str(self.range)
            else:
                subelement.text = text
            regmapEl.append(subelement)
        # Populate with its registers sections
        for name, register in self.registers.items():
            regmapEl.append(register.genXmlSection())
        return regmapEl



########################################
def dump_to_ipxact(regmap, filename):
    
    ipxact__component_names_text = OrderedDict()
    ipxact__component_names_text["vendor"] = "spiritconsortium.org"
    ipxact__component_names_text["library"] = "GWT"
    ipxact__component_names_text["name"] = "regmap.name"
    ipxact__component_names_text["version"] = "1.4"
    
    ipxact__memorymap_names_text = OrderedDict()
    ipxact__memorymap_names_text["name"] = regmap.name+"_mmap0"
    
    filename = open(filename, 'w')
    filename.write("<?xml version=\"1.0\"?>\n")
    
    # Create root element & component hier
    rootEl = ET.Element(spirit("component"))

    for name, text in ipxact__component_names_text.items():
        addSubEl(rootEl, name, text)
    
    # Create mmap hier
    mmapEl = ET.Element(spirit("memoryMaps"))
    addSubEl(mmapEl, "memoryMap", "")
    for name, text in ipxact__memorymap_names_text.items():
        addSubEl2givenElbyname(mmapEl, "memoryMap", name, text)
    
    # Add regmap section
    addSubEl2givenEl(rootEl, "component", mmapEl)
    addSubEl2givenEl(rootEl, "memoryMap", regmap.genXmlSection())

    filename.write(to_s(rootEl))
    filename.close()



