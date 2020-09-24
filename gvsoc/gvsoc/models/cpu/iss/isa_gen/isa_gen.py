#
# Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
#                    University of Bologna
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

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

instrLabels = {}
insn_isa_tags = {}
instrLabelsList = []
nb_insn = 0
nb_decoder_tree = 0

def append_insn_to_isa_tag(isa_tag, insn):
    global insn_isa_tags
    if insn_isa_tags.get(isa_tag) is None:
        insn_isa_tags[isa_tag] = []
    insn_isa_tags[isa_tag].append(insn)


def dump(isaFile, str, level=0):
    for i in range(0, level):
        isaFile.write('  ')
    isaFile.write(str)

class Const(object):
    def __init__(self, val):
        self.val = val

    def len(self):
        return 1

    #def gen(self, signed=False):
    #    return '%d' % (self.val)

    def gen_info(self, isaFile):
        dump(isaFile, '{ .type=ISS_DECODER_VALUE_TYPE_UIM, .u= { .uim= %d } }, ' % self.val)

    def gen(self, isaFile):
        pass

class Range(object):
    def __init__(self, first, width=1, shift=0):
        self.first = first
        self.width = width
        self.shift = shift

    def gen(self, isaFile, indent=0):
        dump(isaFile, '{%d, %d, %d}, ' % (self.first, self.width, self.shift))

    def gen_info(self, isaFile):
        dump(isaFile, '{ .type=ISS_DECODER_VALUE_TYPE_RANGE, .u= { .range_set= {.nb_ranges=1, .ranges={ ')
        self.gen(isaFile)
        dump(isaFile, '} } } }, ')


    def len(self):
        return 1

class Ranges(object):
    def __init__(self, fieldsList):
        self.ranges = []
        for range in fieldsList:
            if len(range) < 2: range.append(1)
            if len(range) < 3: range.append(0)
            self.ranges.append(Range(range[0], range[1], range[2]))

    def gen_info(self, isaFile, indent=0):
        dump(isaFile, '{ .type=ISS_DECODER_VALUE_TYPE_RANGE, .u= { .range_set= {.nb_ranges=%d, .ranges={ ' % (len(self.ranges)))
        for range in self.ranges:
            range.gen(isaFile)
        dump(isaFile, '} } } }, ')

    def len(self):
        return len(self.ranges)


class OpcodeField(object):
    def __init__(self, id, ranges, dumpName=True, flags=[]):
        self.ranges = ranges
        self.id = id
        self.isImm = False
        self.dumpName = dumpName
        self.flags = ['ISS_DECODER_ARG_FLAG_NONE'] + flags
        self.latency = 0

    def is_reg(self):
        return False

    def set_latency(self, latency):
        self.latency = latency

class Indirect(OpcodeField):
    def __init__(self, base, offset=None, postInc=False, preInc=False):
        self.base = base
        self.offset = offset
        self.postInc = postInc
        self.preInc = preInc
        self.flags = ['ISS_DECODER_ARG_FLAG_NONE']

    def genExtract(self, isaFile, level):
        self.base.genExtract(isaFile, level)
        if self.offset is not None:
            self.offset.genExtract(isaFile, level)

    def genTrace(self, isaFile, level):
        if self.offset is not None and self.offset.isImm:
            funcName = 'traceSetIndirectImm'
        else:
            funcName = 'traceSetIndirectReg'
        if self.postInc:
            funcName += 'PostInc'
            self.flags.append('ISS_DECODER_ARG_FLAG_POSTINC')
        if self.preInc:
            funcName += 'PreInc'
            self.flags.append('ISS_DECODER_ARG_FLAG_PREINC')
        if self.offset is not None:
            dump(isaFile, level, '  %s(pc, %s, %s);\n' % (funcName, self.base.genTraceIndirect(), self.offset.genTraceIndirect()))
        else:
            dump(isaFile, level, '  %s(pc, %s, 0);\n' % (funcName, self.base.genTraceIndirect()))

    def gen(self, isaFile, indent=0):
        if self.postInc:
            self.flags.append('ISS_DECODER_ARG_FLAG_POSTINC')
        if self.preInc:
            self.flags.append('ISS_DECODER_ARG_FLAG_PREINC')
        if self.offset.is_reg():
            dump(isaFile, '%s{\n' % (' '*indent))
            dump(isaFile, '%s  .type=ISS_DECODER_ARG_TYPE_INDIRECT_REG,\n' % (' '*indent))
            dump(isaFile, '%s  .flags=(iss_decoder_arg_flag_e)(%s),\n' % (' '*indent, ' | '.join(self.flags)))
            dump(isaFile, '%s  .u= {\n' % (' '*indent))
            dump(isaFile, '%s    .indirect_reg= {\n' % (' '*indent))
            dump(isaFile, '%s      .base_reg= ' % (' '*indent))
            self.base.gen_info(isaFile)
            dump(isaFile, '\n%s      .offset_reg= ' % (' '*indent))
            self.offset.gen_info(isaFile)
            dump(isaFile, '\n%s    },\n' % (' '*indent))
            dump(isaFile, '%s  },\n' % (' '*indent))
            dump(isaFile, '%s},\n' % (' '*indent))
        else:
            dump(isaFile, '%s{\n' % (' '*indent))
            dump(isaFile, '%s  .type=ISS_DECODER_ARG_TYPE_INDIRECT_IMM,\n' % (' '*indent))
            dump(isaFile, '%s  .flags=(iss_decoder_arg_flag_e)(%s),\n' % (' '*indent, ' | '.join(self.flags)))
            dump(isaFile, '%s  .u= {\n' % (' '*indent))
            dump(isaFile, '%s    .indirect_imm= {\n' % (' '*indent))
            dump(isaFile, '%s      .reg= ' % (' '*indent))
            self.base.gen_info(isaFile)
            dump(isaFile, '\n%s      .imm= ' % (' '*indent))
            self.offset.gen_info(isaFile)
            dump(isaFile, '\n%s    },\n' % (' '*indent))
            dump(isaFile, '%s  },\n' % (' '*indent))
            dump(isaFile, '%s},\n' % (' '*indent))

class SignedImm(OpcodeField):
    def __init__(self, id, ranges, isSigned=True):
        self.ranges = ranges
        self.id = id
        self.isSigned = isSigned
        self.isImm = True

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->sim[%d] = %s;\n' % (self.id, self.ranges.gen(signed=self.isSigned)))

    def genTrace(self, isaFile, level):
        dump(isaFile, level, '  traceSetSimm(pc, pc->sim[%d]);\n' % (self.id))

    def genTraceIndirect(self):
        return 'pc->sim[%d]' % (self.id)

    def gen_info(self, isaFile):
        dump(isaFile, '{ .is_signed=%d, .id=%d, .info= ' % (self.isSigned, self.id))
        self.ranges.gen_info(isaFile)
        dump(isaFile, '}, ')

    def gen(self, isaFile, indent=0):
        dump(isaFile, '%s{\n' % (' '*indent))
        dump(isaFile, '%s  .type=ISS_DECODER_ARG_TYPE_SIMM,\n' % (' '*indent))
        dump(isaFile, '%s  .flags=ISS_DECODER_ARG_FLAG_NONE,\n' % (' '*indent))
        dump(isaFile, '%s  .u= {\n' % (' '*indent))
        dump(isaFile, '%s    .simm= ' % (' '*indent))
        self.gen_info(isaFile)
        dump(isaFile, '\n%s  },\n' % (' '*indent))
        dump(isaFile, '%s},\n' % (' '*indent))

class UnsignedImm(OpcodeField):
    def __init__(self, id, ranges, isSigned=False):
        self.ranges = ranges
        self.id = id
        self.isSigned = isSigned
        self.isImm = True

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->uim[%d] = %s;\n' % (self.id, self.ranges.gen(signed=self.isSigned)))

    def genTrace(self, isaFile, level):
        dump(isaFile, level, '  traceSetUimm(pc, pc->uim[%d]);\n' % (self.id))

    def genTraceIndirect(self):
        return 'pc->uim[%d]' % (self.id)

    def gen_info(self, isaFile):
        dump(isaFile, '{ .is_signed=%d, .id=%d, .info= ' % (self.isSigned, self.id))
        self.ranges.gen_info(isaFile)
        dump(isaFile, '}, ')

    def gen(self, isaFile, indent=0):
        dump(isaFile, '%s{\n' % (' '*indent))
        dump(isaFile, '%s  .type=ISS_DECODER_ARG_TYPE_UIMM,\n' % (' '*indent))
        dump(isaFile, '%s  .flags=ISS_DECODER_ARG_FLAG_NONE,\n' % (' '*indent))
        dump(isaFile, '%s  .u= {\n' % (' '*indent))
        dump(isaFile, '%s    .uimm= ' % (' '*indent))
        self.gen_info(isaFile)
        dump(isaFile, '\n%s  },\n' % (' '*indent))
        dump(isaFile, '%s},\n' % (' '*indent))

class OutReg(OpcodeField):
    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->outReg[%d] = %s;\n' % (self.id, self.ranges.gen()))

    def genTrace(self, isaFile, level):
        dump(isaFile, level, '  traceSetReg(pc, pc->outReg[%d], 1, %d);\n' % (self.id, self.dumpName))

    def is_out(self):
        return True

    def is_reg(self):
        return True

    def gen_info(self, isaFile):
        dump(isaFile, '{ .id=%d, .flags=(iss_decoder_arg_flag_e)(%s), .dump_name=%d, .latency=%d, .info= ' % (self.id, ' | '.join(self.flags), 1 if self.dumpName else 0, self.latency))
        self.ranges.gen_info(isaFile)
        dump(isaFile, '}, ')

    def gen(self, isaFile, indent=0):
        dump(isaFile, '%s{\n' % (' '*indent))
        dump(isaFile, '%s  .type=ISS_DECODER_ARG_TYPE_OUT_REG,\n' % (' '*indent))
        dump(isaFile, '%s  .flags=(iss_decoder_arg_flag_e)(%s),\n' % (' '*indent, ' | '.join(self.flags)))
        dump(isaFile, '%s  .u= {\n' % (' '*indent))
        dump(isaFile, '%s    .reg= ' % (' '*indent))
        self.gen_info(isaFile)
        dump(isaFile, '\n%s  },\n' % (' '*indent))
        dump(isaFile, '%s},\n' % (' '*indent))

class OutFReg(OutReg):
    def __init__(self, id, ranges, dumpName=True):
        super(OutFReg, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_FREG'])

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->outReg[%d] = %s + NB_REGS;\n' % (self.id, self.ranges.gen()))

class OutReg64(OutReg):
    def __init__(self, id, ranges, dumpName=True):
        super(OutReg64, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_REG64'])

class OutRegComp(OutReg):
    def __init__(self, id, ranges, dumpName=True):
        super(OutRegComp, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_COMPRESSED'])

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->outReg[%d] = (%s) + 8;\n' % (self.id, self.ranges.gen()))

class OutFRegComp(OutReg):
    def __init__(self, id, ranges, dumpName=True):
        super(OutFRegComp, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_COMPRESSED', 'ISS_DECODER_ARG_FLAG_FREG'])

    def genExtract(self, isaFile, level):
         dump(isaFile, level, '  pc->outReg[%d] = (%s) + 8 + NB_REGS;\n' % (self.id, self.ranges.gen()))

class InReg(OpcodeField):
    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->inReg[%d] = %s;\n' % (self.id, self.ranges.gen()))

    def genTrace(self, isaFile, level):
        dump(isaFile, level, '  traceSetReg(pc, pc->inReg[%d], 0, %d);\n' % (self.id, self.dumpName))

    def genTraceIndirect(self):
        return 'pc->inReg[%d]' % (self.id)

    def is_out(self):
        return False

    def is_reg(self):
        return True

    def gen_info(self, isaFile):
        dump(isaFile, '{ .id=%d, .flags=(iss_decoder_arg_flag_e)(%s), .dump_name=%d, .latency=%d, .info= ' % (self.id, ' | '.join(self.flags), 1 if self.dumpName else 0, self.latency))
        self.ranges.gen_info(isaFile)
        dump(isaFile, '}, ')

    def gen(self, isaFile, indent=0):
        dump(isaFile, '%s{\n' % (' '*indent))
        dump(isaFile, '%s  .type=ISS_DECODER_ARG_TYPE_IN_REG,\n' % (' '*indent))
        dump(isaFile, '%s  .flags=(iss_decoder_arg_flag_e)(%s),\n' % (' '*indent, ' | '.join(self.flags)))
        dump(isaFile, '%s  .u= {\n' % (' '*indent))
        dump(isaFile, '%s    .reg= ' % (' '*indent))
        self.gen_info(isaFile)
        dump(isaFile, '\n%s  },\n' % (' '*indent))
        dump(isaFile, '%s},\n' % (' '*indent))

class InFReg(InReg):
    def __init__(self, id, ranges, dumpName=True):
        super(InFReg, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_FREG'])

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->inReg[%d] = %s + NB_REGS;\n' % (self.id, self.ranges.gen()))

class InReg64(InReg):
    def __init__(self, id, ranges, dumpName=True):
        super(InReg64, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_REG64'])

class InRegComp(InReg):
    def __init__(self, id, ranges, dumpName=True):
        super(InRegComp, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_COMPRESSED'])

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->inReg[%d] = (%s) + 8;\n' % (self.id, self.ranges.gen()))

class InFRegComp(InReg):
    def __init__(self, id, ranges, dumpName=True):
        super(InFRegComp, self).__init__(id=id, ranges=ranges, dumpName=dumpName, flags=['ISS_DECODER_ARG_FLAG_COMPRESSED', 'ISS_DECODER_ARG_FLAG_FREG'])

    def genExtract(self, isaFile, level):
        dump(isaFile, level, '  pc->inReg[%d] = (%s) + 8 + NB_REGS;\n' % (self.id, self.ranges.gen()))

class Format(object):
    def __init__(self, args):
        pass

class DecodeLeaf(object):
    def __init__(self, isaFile, instr, opcode, others=False):
        self.instr = instr
        self.level = 0
        self.isaFile = isaFile
        self.opcode = opcode
        self.others = others

    def dump(self, str):
        #for i in range(0, self.level):
        #    self.isaFile.write('  ')
        self.isaFile.write(str)

    def gen(self):
        self.dump('// %s\n' % (self.instr.getStr()))
        self.instr.gen(self.isaFile, self.opcode, self.others)
        #self.dump('static iss_decoder_item_t %s = { false, 0, { %d, 0, 0, NULL } }; \n\n' % (self.get_name(), self.firstBit))
#
#        #if self.instr.active != None: self.dump('if (!%s) goto error;\n' % (self.instr.active.replace('-', '_')))
        #self.instr.genCall(self.isaFile, self.level)

    def get_name(self):
        return self.instr.get_full_name()

class DecodeTree(object):
    def __init__(self, isaFile, instrs, mask, opcode):
        self.opcode = opcode
        self.isaFile = isaFile
        self.level = 0
        self.subtrees = {}
        self.firstBit = 0
        global nb_decoder_tree
        self.tree_id = nb_decoder_tree
        nb_decoder_tree += 1

        if len(instrs) == 0: return

        # Resize the mask to the min size of the instructions to take
        # into accpount only bits discriminating the various sizes
        # otherwise we could take invalid bits into account
        minSize = None
        for instr in instrs:
            if minSize == None or instr.len < minSize: minSize = instr.len
        currentMask = mask & ((1<<minSize)-1)

        # First eliminate the bits in the mask where no instruction has
        # a discriminating bit
        searchMask = 0
        for instr in instrs:
            for bit in range(0, minSize):
                if (currentMask & (1<<bit)) == 0: continue
                if instr.encoding[bit] in ['0', '1']: searchMask |= 1<<bit
        currentMask &= searchMask

        # Browse all instructions to find the biggest common opcode set
        searchMask = currentMask
        for instr in instrs:

            #print ('Parsing instr %s: %s' % (instr.encoding, instr.label))
            done = False
            firstBit = None
            size = instr.len

            for bit in range(0, size):
                if done or not instr.encoding[bit] in ['0', '1'] or (searchMask & (1<<bit)) == 0: #, '-']:
                    searchMask &= ~(1<<bit)
                    if firstBit != None: 
                        done = True
                    continue
                elif firstBit == None:
                    firstBit = bit

            if firstBit != None and firstBit > self.firstBit: self.firstBit = firstBit



        # In case we found no common mask, maybe we still have some discriminating
        # bits where some instructions don't care
        # Find a common mask where the opcode is either fully defined or don't care
        if searchMask == 0 and len(instrs) > 1: 
            # First find the biggest common discriminating bits
            searchMask = currentMask
            for instr in instrs:
                instrMask = 0
                for bit in range(0, size):
                    if (currentMask & (1<<bit)) == 0: continue
                    if instr.encoding[bit] in ['0', '1']:
                        instrMask |= 1<<bit
                if instrMask != 0:
                    searchMask &= instrMask
            
            for bit in range(0, size):
                if searchMask & (1<<bit): 
                    self.firstBit = bit
                    break

        if searchMask == 0 and len(instrs) > 1: 
            error = 'Error the following instructions have the same opcode:\n'
            for instr in instrs:
                error += instr.getStr() + '\n'
            raise Exception(error)

        currentMask = searchMask

        self.currentMask = currentMask
        mask = mask & ~currentMask

        # Now group them together in sub-groups depending on their
        # opcode for this mask
        groups = {}
        for instr in instrs:
            size = instr.len

            opcodeList = ['']
            isOther = False
            # Extract opcode
            for bit in range(size-1, -1, -1):
                if (currentMask & (1<<bit)) == 0: continue
                instrBit = instr.encoding[bit]
                if instrBit in ['0', '1']:
                    instrBits = [instrBit]
                else:
                    isOther = True
                    break
                newList = []
                for bit in instrBits:
                    for opcode in opcodeList:
                        newList.append(opcode + bit)
                opcodeList = newList

            # Now register the instruction for each of its possible opcode
            if not isOther:
                for opcode in opcodeList:
                    if groups.get(opcode) == None: groups[opcode] = []
                    groups[opcode].append(instr)
            else:
                if groups.get('OTHERS') == None: groups['OTHERS'] = []
                groups['OTHERS'].append(instr)

        self.opcode_width = 0
        for opcode, instrs in groups.items():
            if opcode == 'OTHERS': continue
            if len(opcode) > self.opcode_width:
                self.opcode_width = len(opcode)
            names = []
            for instr in instrs:
                names.append(instr.label)


        for opcode, instrs in groups.items():
            if len(instrs) == 1 and currentMask == 0 or opcode == 'OTHERS': 
                self.subtrees[opcode] = DecodeLeaf(isaFile, instrs[0], self.opcode, others=opcode == 'OTHERS')
            else: self.subtrees[opcode] = DecodeTree(isaFile, instrs, mask, opcode)

        self.mask = currentMask

        self.needTree = False
        for opcode, subtree in self.subtrees.items():
            if len(opcode) != 0: self.needTree = True


    def dump(self, str, diff=0):
        #for i in range(0, self.level+diff):
        #    self.isaFile.write('  ')
        self.isaFile.write(str)

    def get_name(self):
        if self.needTree:
            return 'decoder_tree_%d' % self.tree_id
        else:
            return list(self.subtrees.values())[0].get_name()


    def gen(self, is_top=False):

        if len(self.subtrees) != 0:

            if self.needTree:
                others = None
                for opcode, subtree in self.subtrees.items():
                    if opcode == 'OTHERS':
                        others = subtree
                    else:
                        subtree.gen()
                if others != None:
                    others.gen()
            else:
                subtree = list(self.subtrees.values())[0]
                subtree.gen()


            if self.needTree:
                self.dump('static iss_decoder_item_t *%s_groups[] = {' % self.get_name());
                for opcode, subtree in self.subtrees.items():
                    self.dump(' &%s,' % subtree.get_name())
             
                self.dump(' };\n')

                self.dump('%siss_decoder_item_t %s = {\n' % ('' if is_top else 'static ', self.get_name()))
                self.dump('  .is_insn=false,\n')
                self.dump('  .is_active=false,\n')
                self.dump('  .opcode_others=0,\n')
                self.dump('  .opcode=0b%s,\n' % self.opcode)
                self.dump('  .u={\n')
                self.dump('    .group={\n')
                self.dump('      .bit=%d,\n' % self.firstBit)
                self.dump('      .width=%d,\n' % self.opcode_width)
                self.dump('      .nb_groups=%d,\n' % len(self.subtrees))
                self.dump('      .groups=%s_groups\n' % self.get_name())
                self.dump('    }\n')
                self.dump('  }\n')
                self.dump('};\n')
                self.dump('\n')
            
        
class IsaSubset(object):
    def __init__(self, name, instrs, active=None, file="default", timingTable=None):
        self.name = name
        self.instrs = instrs
        self.active = active
        for instr in instrs:
            instr.isa = self
            instr.active = active
        if file == 'default': self.file = name
        else: self.file = file

        self.insn_registered = False

    def get_insns(self):
        if not self.insn_registered:
            self.insn_registered = True

            for insn in self.instrs:
                if len(insn.isa_tags) == 0:
                    append_insn_to_isa_tag(self.name, insn)

        return self.instrs

    def setPower(self, power):
        for instr in self.instrs:
            instr.setPower(power)

    def getOptions(self):
        options = {}
        for instr in self.instrs:
            options.update(instr.getOptions())

        return options
        
class IsaDecodeTree(object):

    def __init__(self, name, subsets):
        self.subsets = subsets
        self.tree = None
        self.name = name
            
    def get_insns(self):
        result = []
        
        for subset in self.subsets:
            result += subset.get_insns()

        return result

    def dumpTree(self, isa, isaFile):
        instrs = self.get_insns()

        if self.tree == None:
            self.tree = DecodeTree(isaFile, instrs, 0xffffffff, '0')

        if len(instrs) != 0:
            self.tree.gen(is_top=True)

    def dump_ref(self, isa, isaFile):
        dump(isaFile, '  {(char *)"%s", &%s},\n' % (self.name, self.tree.get_name()))


class Isa(object):
    def __init__(self, name, trees):
        self.level = 0
        self.name = name
        self.trees = trees


    def get_insns(self):
        result = []
        
        for tree in self.trees:
            result += tree.get_insns()

        return result

    def dump(self, str):
        for i in range(0, self.level):
            self.isaFile.write('  ')
        self.isaFile.write(str)

    def gen(self, isaFile, isaFileHeader):

        self.isaFile = isaFile
        self.isaFileHeader = isaFileHeader

        self.dump('#include "iss.hpp"\n')
        self.dump('\n')

        self.isaFileHeader.write('#ifndef __GV_ISA_%s__HPP_\n' % self.name)
        self.isaFileHeader.write('#define __GV_ISA_%s__HPP_\n' % self.name)

        self.isaFileHeader.write('#endif\n')
        



        for tree in self.trees:
            tree.dumpTree(self, isaFile)


        for isa_tag in insn_isa_tags.keys():
            self.dump('static iss_decoder_item_t *__iss_isa_tag_%s[] = {\n' % isa_tag)
            insn_list = []
            for insn in insn_isa_tags.get(isa_tag):
                insn_list.append('&' + insn.get_full_name())
            insn_list.append('NULL')
            self.dump('  %s\n' % ', '.join(insn_list))
            self.dump('};\n')
            self.dump('\n')

        self.dump('iss_isa_tag_t __iss_isa_tags[] = {\n')
        for isa_tag in insn_isa_tags.keys():
            self.dump('  {(char *)"%s", __iss_isa_tag_%s},\n' % (isa_tag, isa_tag))
        self.dump('  {(char *)NULL, NULL}\n')
        self.dump('};\n')
        self.dump('\n')


        self.dump('static iss_isa_t __iss_isa_list[] = {\n')
        for tree in self.trees:
            tree.dump_ref(self, isaFile)
        self.dump('};\n')
        self.dump('\n')

        self.dump('iss_isa_set_t __iss_isa_set = {\n')
        self.dump('  .nb_isa=sizeof(__iss_isa_list)/sizeof(iss_isa_t),\n')
        self.dump('  .isa_set=__iss_isa_list\n')        
        self.dump('};\n')



        return

        self.isaFileHeader.write('\n')
        self.isaFileHeader.write('extern char *instrNames[];\n')
        self.isaFileHeader.write('\n')

        self.isaFileHeader.write('#define GV_ISA_NB_INSTR %d\n' % (len(instrLabels)))
        self.isaFileHeader.write('\n')
        for id in range(0, len(instrLabelsList)):
            label = instrLabelsList[id]
            self.isaFileHeader.write('#define GV_ISA_%s %d\n' % (label.replace('.', '_').upper(), id))

        self.isaFileHeader.write('\n')
        self.isaFileHeader.write('typedef enum {\n')
        for option in self.options:
            self.isaFileHeader.write('  %s,\n' % (option.replace('-', '_')))
        self.isaFileHeader.write('\n')

        for key, value in self.optionsDict.items():
            self.isaFileHeader.write('  %s,\n' % (value[1].replace('-', '_')))

        self.isaFileHeader.write('  CPU_NB_OPTIONS,\n')
        self.isaFileHeader.write('\n')
        self.isaFileHeader.write('} cpuOptions_e;\n')
        self.isaFileHeader.write('\n')


        self.dump('#ifndef __SWIG__\n')
        self.dump('#include "%s/%s.hpp"\n' % (self.name, self.name))
        self.dump('#endif\n')
        self.dump('#include "%s"\n' % (isaFileHeader.name))
        self.dump('\n')

        for subset in self.subsets:
            if subset.file != None: self.dump('#include "%s/%s.hpp"\n' % (self.name, subset.file))
        self.dump('\n')
            
        self.dump('#ifndef __SWIG__\n')
        self.dump('#include "lib_exec.hpp"\n')
        self.dump('#endif\n')
        self.dump('\n')

        self.dump('char *instrNames[] = {\n')
        for id in range(0, len(instrLabelsList)):
            label = instrLabelsList[id]
            self.dump('  (char *)"%s",\n' % (label))
        self.dump('};\n')
        self.dump('\n')

        for table in timingTables:
            table.gen(isaFile)
        self.dump('\n')

        self.dump('timingTable_t *timeTables[] = {')
        for table in timingTables:
            self.dump('&%s, ' % table.getStructName())
        self.dump('};\n\n')

        self.dump('isaOption_t isaOptions[] = {\n')
        for option in self.options:
            self.dump('  {(char *)"%s", %s}, \n' % (option, option.replace('-', '_')))
        for key, value in self.optionsDict.items():
            self.dump('  {(char *)"%s", %s}, \n' % (value[1], value[1].replace('-', '_')))
        self.dump('  {NULL, 0}\n')
        self.dump('};\n')
        self.dump('\n')

        for subset in self.subsets:
            for instr in subset.instrs:
                instr.gen(isaFile, self.level)

        for subset in self.subsets:
            subset.dumpTree(self, isaFile)

        self.dump('int processPc(cpu_t *cpu, pc_t *pc)\n{\n')
        self.level += 1

        for subset in self.subsets:
            if subset.active != None: self.dump('if (%s && decodePc_%s(cpu, pc) == 0) goto found;\n' % (subset.active.replace('-', '_'), subset.name))
            else: self.dump('if (decodePc_%s(cpu, pc) == 0) goto found;\n' % (subset.name))
        self.dump('goto error;\n')
        self.dump('found:\n')
        self.dump('return 0;\n')
        self.dump('error:\n')
        self.dump('return 1;\n')
        self.level -= 1
        self.dump('}\n')


        self.dump('pc_t *decodePc(cpu_t *cpu, pc_t *pc)\n{\n')
        self.level += 1
        self.dump('sim_fetch_decode(cpu, pc->addr, (unsigned char *)&pc->value);\n');

        for subset in self.subsets:
            if subset.active != None: self.dump('if (getOption(cpu, %s) && decodePc_%s(cpu, pc) == 0) goto found;\n' % (subset.active.replace('-', '_'), subset.name))
            else: self.dump('if (decodePc_%s(cpu, pc) == 0) goto found;\n' % (subset.name))

        self.dump('goto error;\n')
        self.dump('found:\n')
        self.dump('if (cpu->cache->traceEn)\n')
        self.dump('{\n')
        self.dump('pc->trace.saved_exec = pc->handler;\n')
        self.dump('setHandler(pc, trace_exec);\n')
        self.dump('}\n')
        self.dump('else\n')
        self.dump('{\n')
        self.dump('pc->trace.saved_exec = NULL;\n')
        self.dump('}\n')
        self.dump('\n')
        self.dump('if (cpu->cache->powerEn)\n')
        self.dump('{\n')
        self.dump('pc->trace.power_saved_exec = pc->handler;\n')
        self.dump('setHandler(pc, power_exec);\n')
        self.dump('}\n')
        self.dump('\n')
        self.dump('if (cpu->cache->statsEn)\n')
        self.dump('{\n')
        self.dump('pc->trace.stats_saved_exec = pc->handler;\n')
        self.dump('setHandler(pc, stats_exec);\n')
        self.dump('}\n')
        self.dump('\n')
        self.dump('return handlePc(cpu, pc);\n')
        self.dump('\n')
        self.dump('error:\n')
        self.dump('triggerException(cpu, pc, EXCEPTION_ILLEGAL_INSTR);\n')
        self.dump('return pc;\n')
        self.level -= 1
        self.dump('}\n')


InstrNames = []
timingTables = []

class TimingTable(object):
    def __init__(self, name, option, default):
        self.option = option
        self.default = default
        self.name = name
        self.groups = []
        timingTables.append(self)

    def getStructName(self):
        return 'timingTable_%s' % self.name

    def regGroup(self, group):
        self.groups.append(group)

    def gen(self, isaFile):

        isaFile.write('static char *%s_names[] = { ' % (self.getStructName()))
        for group in self.groups:
            isaFile.write('(char *)"%s", ' % group.name)
        isaFile.write('};\n\n')


        isaFile.write('static timingTable_t %s = { "%s", "%s", "%s", 0, %d, %s_names, NULL, NULL };\n' % (self.getStructName(), self.name, self.option, self.default, len(self.groups), self.getStructName()))


class IsaGroup(object):
    def __init__(self, name, offload=None, timingTable=None):
        self.name = name
        self.nbGroups = 0
        self.timingTable = timingTable
        self.offload = offload

    def regGroup(self, group):
        group.id = self.nbGroups
        self.nbGroups += 1
        if self.timingTable != None: self.timingTable.regGroup(group)

    def getTiming(self):
        if self.timingTable == None: return None
        else: return self.timingTable.getStructName()

    def getOffload(self):
        if self.offload == None: return None
        return self.offload

    def getOptions(self):
        if self.offload != None: return { self.offload[0] : self.offload }
        else: return {}



class InstrGroup(object):
    def __init__(self, isaGroup, name, offload=None):
        self.isaGroup = isaGroup
        self.name = name
        self.offload = offload
        isaGroup.regGroup(self)

    def getTiming(self):
        return self.isaGroup.getTiming()

    def getOffload(self):
        if self.offload != None: return self.offload
        else: return self.isaGroup.getOffload()

    def getOptions(self):
        options = self.isaGroup.getOptions()
        if self.offload != None: options.update({self.offload[1] : self.offload })
        return options


defaultIsaGroup   = IsaGroup('ISA_GROUP_OTHER')
defaultInstrGroup = InstrGroup(defaultIsaGroup, 'INSTR_GROUP_OTHER')

class Instr(object):
    def __init__(self, label, type, encoding, decode=None, N=None, L=None, mapTo=None, power=None, group=None, fast_handler=False, tags=[], isa_tags=[]):
        global nb_insn

        for isa_tag in isa_tags:
            append_insn_to_isa_tag(isa_tag, self)

        self.insn_number = nb_insn
        self.tags = tags
        self.isa_tags = isa_tags
        self.out_reg_latencies = []
        self.latency = 0
        nb_insn += 1

        encoding = encoding[::-1].replace(' ', '')
        self.label = label
        if L != None: self.traceLabel = L
        else: self.traceLabel = label
        self.safeLabel = label.replace('.', '_')
        self.encoding = encoding
        self.decode = decode
        self.srcId = 0
        if mapTo != None: name = mapTo
        else: 
            if N == None: name = self.safeLabel
            else: name = N
        self.execFunc = '%s_exec' % (name)
        if not fast_handler:
            self.quick_execFunc = self.execFunc
        else:
            self.quick_execFunc = self.execFunc + '_fast'

        self.decodeFunc = '%s_decode_gen' % (self.safeLabel)
        index = 0
        while self.decodeFunc in InstrNames:
            self.decodeFunc = '%s_%d_decode_gen' % (self.safeLabel, index)
            index += 1
        InstrNames.append(self.decodeFunc)
        self.len = len(encoding.strip())
        self.power = power
        if group != None: self.group = group
        else: self.group = defaultInstrGroup

        self.offload = None
        if group != None: self.offload = group.getOffload()

        if instrLabels.get(self.traceLabel) == None:
            instrLabels[self.traceLabel] = len(instrLabels)
            instrLabelsList.append(self.traceLabel)
        
        self.id = instrLabels[self.traceLabel]

    def set_latency(self, latency):
        self.latency = latency

    def get_out_reg(self, reg):
        index = 0
        for arg in self.args:
            if arg.is_reg() and arg.is_out():
                if index == reg:
                    return arg
                index += 1
        return None

    def get_full_name(self):
        return ('insn_%d_%s_%s' % (self.insn_number, self.isa.name, self.getLabel())).replace('.', '_')

    def getLabel(self):
        return self.traceLabel

    def setPower(self, power):
        # Keep local power setup if it is setup to priviledge specialization
        if self.power == None: self.power = power

    def genArgsExtract(self, isaFile, level):
        for arg in self.args:
            arg.genExtract(isaFile, level)

    def getStr(self):
        return '%s: %s' % (self.encoding[::-1], self.label)

    def getSrcId(self):
        srcId = self.srcId
        self.srcId += 1
        return srcId

    def dump(self, isaFile, str, level=0):
        for i in range(0, level):
            isaFile.write('  ')
        isaFile.write(str)

    def genCall(self, isaFile, level):
        self.dump(isaFile, '%s(cpu, pc);\n' % (self.decodeFunc), level)

    def gen(self, isaFile, opcode, others=False):

        name = self.get_full_name()

        self.dump(isaFile, 'static iss_decoder_item_t %s = {\n' % (name))
        self.dump(isaFile, '  .is_insn=true,\n')
        self.dump(isaFile, '  .is_active=false,\n')
        self.dump(isaFile, '  .opcode_others=%d,\n' % (1 if others else 0))
        self.dump(isaFile, '  .opcode=0b%s,\n' % opcode)
        self.dump(isaFile, '  .u={\n')
        self.dump(isaFile, '    .insn={\n')
        self.dump(isaFile, '      .handler=%s,\n' % self.execFunc)
        self.dump(isaFile, '      .fast_handler=%s,\n' % self.quick_execFunc)
        self.dump(isaFile, '      .decode=%s,\n' % ('NULL' if self.decode is None else self.decode))
        self.dump(isaFile, '      .label=(char *)"%s",\n' % (self.getLabel()))
        self.dump(isaFile, '      .size=%d,\n' % (self.len/8))
        self.dump(isaFile, '      .nb_args=%d,\n' % (len(self.args)))
        self.dump(isaFile, '      .latency=%d,\n' % (self.latency))
        self.dump(isaFile, '      .args= {\n')
        if len(self.args) > 0:
            for arg in self.args:
                arg.gen(isaFile, indent=8)
        self.dump(isaFile, '      }\n')
        self.dump(isaFile, '    }\n')
        self.dump(isaFile, '  }\n')
        self.dump(isaFile, '};\n')
        self.dump(isaFile, '\n')


    def getOptions(self):
        if self.group != None: return self.group.getOptions()
        else: return {}
