#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import sys
import os
import argcomplete
import argparse
import struct

from errors import FatalError, NotSupportedError
import common
import json_tools as js

import partition
import binary
import gen_partition
import traces

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
    print("Fatal error: gen_flash_image needs to be run with python version 3")
    exit(1)


def dumpByteToSlm(file, addr, value):
    file.write("@%08X %02X\n" % (addr, value))

def dumpShortToSlm(file, addr, value):
    file.write("@%08X %04X\n" % (addr, value))

def dumpWordToSlm(file, addr, value):
    file.write("@%08X %08X\n" % (addr, value))

def dumpLongToSlm(file, addr, value):
    file.write("@%08X %016X\n" % (addr, value))

def dumpLongLongToSlm(file, addr, value):
    file.write("@%08X %032X\n" % (addr, value))



class FlashImage(object):
    
    def __init__(self, sectorSize, flashType):
        
        self.sectorSize = sectorSize
        if flashType not in ('hyper', 'spi', 'mram'):
            raise FatalError(
                'Flash type %s not suported. ROM boot loader supports hyper and spi flash type' % flashType)
        self.flashType = flashType
        
        self.image = bytearray()
        
        # Partition
        self.partitionTable = None
        self.partitionTableOffset = None
        # Partition 0 : Flash header + Second Stage BootLoader or unic APP
        self.ssbl = None
    
    def getCurrentSize(self):
        return len(self.image)
    
    def __appendInt(self, value, newBlock=False, buff=None):
        if buff is None:
            #if newBlock: self.__roundToNextBlock()
            self.buff += struct.pack("I", value)
        else:
            buff += struct.pack("I", value)
            return buff

    def __appendLongInt(self, value, newBlock=False, buff=None):
        if buff is None:
            #if newBlock: self.__roundToNextBlock()
            self.buff += struct.pack("Q", value)
        else:
            buff += struct.pack("Q", value)
            return buff

    def __appendByte(self, value, newBlock=False, buff=None):
        if buff is None:
            #if newBlock: self.__roundToNextBlock()
            self.buff += struct.pack("B", value)
        else:
            buff += struct.pack("B", value)
            return buff

    def appendBootloader(self, elf, encrypt = False, aesKey = None, aesIv = None):
        self.ssbl = binary.SSBL(flashType = self.flashType, elf = elf, encrypt = encrypt, aesKey = aesKey,
                                aesIv = aesIv)
        self.image = self.ssbl.dump()
        self.partitionTableOffset = self.ssbl.partitionTableOffset
        return self.partitionTableOffset
    
    def appendPartitionTable(self, partitionTable):
        self.partitionTable = partitionTable
        
        # Check if a boot loader has been added previously, if not, generating empty boot loader
        if len(self.image) == 0:
            traces.info("Warning: Empty boot loader partition. GAP will not be able to boot from flash.")
            self.ssbl = binary.SSBL(flashType = self.flashType)
            self.image += self.ssbl.dump()
        
        self.image += self.partitionTable.to_binary()
    
    def writeRAWImage(self, outputPath):
        # Creating folders if necessary
        try:
            os.makedirs(os.path.realpath(os.path.dirname(outputPath)))
        except FileExistsError:
            pass
        
        with open(outputPath, 'wb') as file:
            file.write(self.image)
    
    def writeStimuli(self, outputPath):
        # Warning: not tested!
        # Creating folders if necessary
        try:
            os.makedirs(os.path.dirname(outputPath))
        except:
            pass
        
        with open(outputPath, 'w') as file:
            self.buff = self.image
            if self.flashType == 'mram':
                last_bytes = len(self.buff) & 0xF
                for i in range(0, 16 - last_bytes):
                    self.__appendByte(0)
                for i in range(0, len(self.buff) >> 4):
                    value = (self.buff[i * 16 + 15] << 120) + (self.buff[i * 16 + 14] << 112) + \
                            (self.buff[i * 16 + 13] << 104) + (self.buff[i * 16 + 12] << 96) + \
                            (self.buff[i * 16 + 11] << 88)  + (self.buff[i * 16 + 10] << 80) + \
                            (self.buff[i * 16 + 9]  << 72)  + (self.buff[i * 16 + 8]  << 64) + \
                            (self.buff[i * 16 + 7]  << 56)  + (self.buff[i * 16 + 6]  << 48) + \
                            (self.buff[i * 16 + 5]  << 40)  + (self.buff[i * 16 + 4]  << 32) + \
                            (self.buff[i * 16 + 3]  << 24)  + (self.buff[i * 16 + 2]  << 16) + \
                            (self.buff[i * 16 + 1]  << 8)   + (self.buff[i * 16])
                    dumpLongLongToSlm(file, i, value)
            elif self.flashType == 'hyper':
                if len(self.buff) & 1 != 0:
                    self.__appendByte(0)
                for i in range(0, len(self.buff) >> 1):
                    value = (self.buff[i * 2 + 1] << 8) + self.buff[i * 2]
                    dumpShortToSlm(file, i, value)
            else:
                for i in range(0, len(self.buff)):
                    dumpByteToSlm(file, i, self.buff[i])


def appendArgs(parser: argparse.ArgumentParser, flashConfig: js.config) -> None:
    """
    Append specific module arguments.

    """
    # Bootloader
    kwargs = { }
    if flashConfig:
        kwargs['default'] = flashConfig.get_str("content/boot-loader")
    parser.add_argument('--boot-loader',
                        type = argparse.FileType('rb'),
                        help = """Boot loader or unic application in elf format.
		                           In case of Gap boot from flash, this program will be loaded into memories and will be executed by the FC.""",
                        **kwargs)
    
    # Partition table
    parser.add_argument('--partition-table', '-t',
                        default = None, type = argparse.FileType('rb'), required = False,
                        dest = 'partitionTable',
                        help = """Partition table in binary or csv format.
	                    If the partition table is not mentioned, a typical partition table is generated with both filesystem partitions readfs and LittleFS.""")
    
    # Partition images
    parser.add_argument('--partition', '-p', metavar = 'partition-name partition-image',
                        default = {},
                        action = partition.PartitionNameFilenamePairAction,
                        help = 'The name of the partition and its image file.')
    
    # Flash block size
    kwargs = {}
    if flashConfig:
        kwargs['default'] = flashConfig.get_int("datasheet/block-size")
    parser.add_argument('--block-size', '-b',
                        type = common.argToInt, dest = 'blockSize',
                        required = not flashConfig,
                        help = "Erasable flash sectore size. useful for aligning partitions.",
                        **kwargs)
    
    # Flash size
    kwargs = {}
    if flashConfig:
        kwargs['default'] = flashConfig.get_int("datasheet/size")
    
    parser.add_argument('--flash-size', '-s',
                        required = not flashConfig,
                        type = common.argToInt, dest = 'flashSize',
                        help = "Flash size.",
                        **kwargs)
    
    # Flash type
    kwargs = {}
    if flashConfig:
        kwargs['default'] = flashConfig.get_str("datasheet/type")
    parser.add_argument('--flash-type',
                        required = not flashConfig,
                        choices = ('hyper', 'spi'),
                        dest = 'flashType',
                        help = "Flash type.",
                        **kwargs)
    
    # Flash stimuli format
    kwargs = { 'default': None }
    if flashConfig:
        kwargs['default'] = flashConfig.get_str("content/stimuli/format")
    parser.add_argument('--flash-stimuli-format',
                        choices = ('slm'),
                        dest = 'flashStimuliFormat',
                        help = "Flash stimuli format.",
                        **kwargs)
    
    # Flash stimuli file
    kwargs = { 'default': None }
    if flashConfig:
        kwargs['default'] = flashConfig.get_str("content/stimuli/file")
    parser.add_argument('--flash-stimuli-file',
                        choices = ('slm'),
                        dest = 'flashStimuliFile',
                        help = "Flash stimuli file.",
                        **kwargs)
    
    # Output
    kwargs = { 'default': 'flash.img' }
    if flashConfig:
        kwargs['default'] = flashConfig.get_str("content/image")
    parser.add_argument('-o',
                        dest = 'output',
                        help = 'RAW image output',
                        **kwargs)


def operationFunc(args, flash_config=None):

    traces.info('Build flash image')
    
    flashImage = FlashImage(sectorSize = args.blockSize, flashType = args.flashType)
    
    #
    # Bootloader
    #
    endOfSSBLOffset = flashImage.appendBootloader(elf = args.boot_loader)
    traces.info("Partition boot loader size: 0x%X" % endOfSSBLOffset)
    
    #
    # Partition table
    #
    traces.info("\nGenerating partition table:")
    # Check if the partition table is lower than free space in current SSBL flash sector
    if (endOfSSBLOffset + partition.MAX_PARTITION_TABLE_SIZE) > binary.align(endOfSSBLOffset, args.blockSize):
        partitionTableOffset = binary.align(endOfSSBLOffset, args.blockSize)
        traces.info(
            "No free space to store partition table at the end of the SSBL partition, adding padding untill the next sector 0x%X" % partitionTableOffset)
        flashImage.image.padToOffset(partitionTableOffset)
    else:
        partitionTableOffset = endOfSSBLOffset
    
    traces.info("Partition table offset: 0x%X" % partitionTableOffset)
  
    if flash_config and flash_config.get('content/partitions') is not None:
        traces.info('Partition table was provided from configuration file.')
        config_partitions = {}
        table = gen_partition.PartitionTable(md5Sum = True)
        offset = partitionTableOffset + partition.MAX_PARTITION_TABLE_SIZE

        for name, partition_config in flash_config.get('content/partitions').get_items().items():
            if partition_config.get_bool('enabled'):
                path = partition_config.get_str('image')
                type_name = partition_config.get_str('type')
                size = os.path.getsize(path)
                traces.info("Creating partition (name: %s, type: %s, path: %s, offset: 0x%x, size: 0x%x" % (name, type_name, path, offset, size))

                part = partition.PartitionDefinition(
                    name = name,
                    type = partition.DATA_TYPE,
                    subtype = partition.SUBTYPES[partition.DATA_TYPE][type_name],
                    size = size,
                    offset = binary.align(offset, args.blockSize),
                    path = path)
                table.append(part)
                offset = part.offset + part.size

    else:

        if args.partitionTable:
            traces.infoWithoutNewLine('Open partition table: ')
            tableInput = args.partitionTable.read()
            if gen_partition.isBinaryPartitionTable(tableInput):
                # Binary format
                traces.info('Binary table format')
                table = gen_partition.PartitionTable.from_binary(tableInput)
            
            else:
                # CSV Format
                traces.info('CSV table format')
                
                try:
                    tableInput = tableInput.decode()
                except UnicodeDecodeError:
                    raise InputError(
                        '"%s" input file must be a CSV text file or partition table binary.' % args.partitionTable.name)
                
                traces.info('Parsing CSV input...')
                table = gen_partition.PartitionTable.from_csv(tableInput, partitionTableOffset = partitionTableOffset,
                                                            sectorSize = args.blockSize, md5Sum = True)
        
        else:
            # Auto partitioning
            traces.info('Partition table was not provided, generating generic table.')
            table = gen_partition.PartitionTable(md5Sum = True)
            offset = partitionTableOffset + partition.MAX_PARTITION_TABLE_SIZE
            
            if "readfs" in args.partition.keys():
                traces.info("Creating ReadFS partition")
                readFSSize = os.path.getsize(args.partition['readfs'].name)
                traces.info("ReadFS image size: 0x%X" % readFSSize)
                readFSPartition = partition.PartitionDefinition(
                    name = 'readfs',
                    type = partition.DATA_TYPE,
                    subtype = partition.READFS_SUBTYPE,
                    size = readFSSize,
                    offset = binary.align(offset, args.blockSize))
                table.append(readFSPartition)
                offset = readFSPartition.offset + readFSPartition.size
            
            #
            # LittleFS partition
            #
            lfsOffset = binary.align(offset, args.blockSize)
            if "lfs" in args.partition.keys():
                # LittleFS image from CLI
                traces.info("Creating LittleFS partition")
                lfsSize = os.path.getsize(args.partition['lfs'].name)
                traces.info("LittleFS image size: 0x%X" % lfsSize)
            else:
                # No LittleFS image, use of remaining free space
                lfsSize = args.flashSize - lfsOffset
                traces.info("Creating an empty LittleFS partition, using the rest of the flash space: 0x%X" % lfsSize)
            
            lfsPartition = partition.PartitionDefinition(
                name = 'lfs',
                type = partition.DATA_TYPE,
                subtype = partition.LFS_SUBTYPE,
                size = lfsSize,
                offset = binary.align(offset, args.blockSize))
            table.append(lfsPartition)
            offset = lfsPartition.offset + lfsPartition.size
        
    traces.info('Verifying table...')
    traces.info(table.to_csv(simple_formatting = False))
    table.verify(partitionTableOffset = partitionTableOffset, flashSectorSize = args.blockSize,
                 flashSize = args.flashSize)
    
    flashImage.appendPartitionTable(table)
    
    #
    # Writting partition images
    #
    traces.info("Dumping partition image::")

    if flash_config and flash_config.get('content/partitions') is not None:
        for p in table:
            if p.path:
                flashImage.image.padToOffset(p.offset)
                with open(p.path, 'rb') as p_file:
                    flashImage.image += p_file.read()

    else:
        for p in sorted(table, key = lambda x: x.offset):
            if p.name in args.partition.keys():
                traces.info("%s partition [%s]" % (p.name, args.partition[p.name].name))
                flashImage.image.padToOffset(p.offset)
                flashImage.image += args.partition[p.name].read()
            
            else:
                traces.info("%s partition [None]" % p.name)
    
    # add padding to finish on 4 bytes align
    flashImage.image.padToOffset(binary.align(flashImage.getCurrentSize(), 4))
    
    #
    # Write output
    #
    
    traces.info("\nWritting output image to %s, size %uKB." % (args.output, flashImage.getCurrentSize() / 1024))
    flashImage.writeRAWImage(args.output)

    if args.flashStimuliFormat is not None:
        file_name = args.flashStimuliFile
        traces.info("\nWritting output stimuli to %s" %( file_name))
        flashImage.writeStimuli(file_name)



def main(custom_commandline = None, config = None, flash_config = None):
    """
    Main
    function
    for build Flash image
    
    custom_commandline - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments
    as strings.Arguments and their values need to be added as individual items to the list e.g."-b 115200" thus
    becomes['-b', '115200'].
    """
    
    parser = argparse.ArgumentParser(
        description = 'Build a Gap flash image from binary files, partition table and partition images - v%s' % __version__,
        prog = 'gen_flash_image',
        fromfile_prefix_chars = '@',
        formatter_class = argparse.ArgumentDefaultsHelpFormatter)
    
    common.appendCommonOptions(parser)
    
    if config is None:
        config = common.importConfig(parser)
    
    if flash_config is None:
        flash_config = config.get(config.get_str("runner/boot/device"))

    appendArgs(parser, flash_config)
    
    argcomplete.autocomplete(parser)

    # If no argument has been passed and a config has been passed, force arg_parse to not take arguments from sys.argv
    # as this would parse arguments for the calling script
    if custom_commandline is None and (config is not None or flash_config is not None):
        custom_commandline = []
    
    args = parser.parse_args(custom_commandline)
    
    
    try:
        operationFunc(args, flash_config)
    finally:
        try:  # Clean up AddrFilenamePairAction files
            for address, argfile in args.addr_filename:
                argfile.close()
        except AttributeError:
            pass


if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print('\nA fatal error occurred: %s' % e)
        sys.exit(2)
