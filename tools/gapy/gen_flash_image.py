#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import sys
import os
import argcomplete
import argparse

from errors import FatalError, NotSupportedError
import common
import partition
import binary
import gen_partition
import traces

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
	print("Fatal error: gen_flash_image needs to be run with python version 3")
	exit(1)


class FlashImage(object):
	
	def __init__(self, sectorSize, flashType):
		
		self.sectorSize = sectorSize
		if flashType not in ('hyper', 'spi'):
			raise FatalError('Flash type %s not suported. ROM boot loader supports hyper and spi flash type' % flashType)
		self.flashType = flashType
		
		self.image = bytearray()
		
		# Partition
		self.partitionTable = None
		self.partitionTableOffset = None
		# Partition 0 : Flash header + Second Stage BootLoader or unic APP
		self.ssbl = None
	
	def getCurrentSize(self):
		return len(self.image)
	
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
			if self.flashType == 'mram':
				last_bytes = len(self.buff) & 0x7
				for i in range(0, 8 - last_bytes):
					self.__appendByte(0)
				for i in range(0, len(self.buff) >> 3):
					value = (self.buff[i * 8 + 7] << 56) + (self.buff[i * 8 + 6] << 48) + (
							self.buff[i * 8 + 5] << 40) + (self.buff[i * 8 + 4] << 32) + (
							        self.buff[i * 8 + 3] << 24) + (self.buff[i * 8 + 2] << 16) + (
							        self.buff[i * 8 + 1] << 8) + self.buff[i * 8]
					dumpLongToSlm(file, i, value)
			elif self.flashType == 'hyper':
				if len(self.buff) & 1 != 0:
					self.__appendByte(0)
				for i in range(0, len(self.buff) >> 1):
					value = (self.buff[i * 2 + 1] << 8) + self.buff[i * 2]
					dumpShortToSlm(file, i, value)
			elif self.archi == 'vivosoc2' or self.archi == 'fulmine':
				if len(self.buff) % 4 != 0:
					for i in range(0, 4 - (len(self.buff) % 4)):
						self.buff.append(0)
				for i in range(0, len(self.buff), 4):
					dumpByteToSlm(file, i, self.buff[i + 3])
					dumpByteToSlm(file, i + 1, self.buff[i + 2])
					dumpByteToSlm(file, i + 2, self.buff[i + 1])
					dumpByteToSlm(file, i + 3, self.buff[i + 0])
			else:
				for i in range(0, len(self.buff)):
					dumpByteToSlm(file, i, self.buff[i])


def appendArgs(parser: argparse.ArgumentParser) -> None:
	"""
	Append specific module arguments.

	:param parser:
	:type parser: argparse.ArgumentParser
	"""
	
	# Bootloader
	parser.add_argument('--boot-loader',
	                    type = argparse.FileType('rb'), required = True,
	                    help = """Boot loader or unic application in elf format.
		                           In case of Gap boot from flash, this program will be loaded into memories and will be executed by the FC.""")
	
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
	parser.add_argument('--block-size', '-b',
	                    required = True,
	                    type = common.argToInt, dest = 'blockSize',
	                    help = "Erasable flash sectore size. useful for aligning partitions.")
	
	# Flash size
	parser.add_argument('--flash-size', '-s',
	                    required = True,
	                    type = common.argToInt, dest = 'flashSize',
	                    help = "Flash size.")
	
	# Flash type
	parser.add_argument('--flash-type',
	                    required = True,
	                    choices = ('hyper', 'spi'), dest = 'flashType',
	                    help = "Flash type.")
	
	# Output
	parser.add_argument('-o',
	                    dest = 'output', default = 'flash.img',
	                    help = 'RAW image output')


def operationFunc(args):
	traces.verbose = args.verbose
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
	table.verify(partitionTableOffset = partitionTableOffset, flashSectorSize = args.blockSize, flashSize = args.flashSize)

	flashImage.appendPartitionTable(table)
	
	#
	# Writting partition images
	#
	traces.info("Dumping partition image::")
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
	
	traces.info("\nWritting output image to %s, size %uKB." %(args.output, flashImage.getCurrentSize() / 1024))
	flashImage.writeRAWImage(args.output)


def main(custom_commandline = None):
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
		fromfile_prefix_chars = '@')
	
	common.appendCommonOptions(parser)
	appendArgs(parser)
	
	argcomplete.autocomplete(parser)
	args = parser.parse_args(custom_commandline)
	
	try:
		operationFunc(args)
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
