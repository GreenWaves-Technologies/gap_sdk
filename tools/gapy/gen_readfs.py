#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import sys
import os

import argcomplete
import argparse

from errors import FatalError, NotSupportedError
import common
import traces
import binary
import json_tools as js

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
	print("Fatal error: Gapy needs to be run with python version 3")
	exit(1)


class Comp(object):
	
	def __init__(self, dirpath, name, incDirInName = False):
		self.path = os.path.join(dirpath, name)
		
		if incDirInName:
			self.name = self.path
		else:
			self.name = name
		
		self.size = os.path.getsize(self.path)
	
	def dump(self):
		print(self.name)
		print('Path: ' + self.path)
		print('Partition addr: ' + str(self.partitionAddr))
		print('Size: ' + str(self.size))
		print('')


class ReadFS(binary.BlockBuffer):
	def __init__(self):
		self.compList = []
	
	def appendComponent(self, comp):
		self.compList.append(comp)
	
	def generate(self, outputPath):
		# Compute the header size
		headerSize = 12  # Header size and number of components
		for comp in self.compList:
			headerSize += 12  # Partition address, size and path length
			headerSize += len(comp.name) + 1  # Path
		
		offset = headerSize
		
		# Now set the flash address for each component
		if len(self.compList) > 0:
			traces.info('Adding component')
			traces.info('')
			traces.info('Name                           Size       Partition offset')
		else:
			traces.info("*** Warning: No component was specified! ***")
		for comp in self.compList:
			comp.partitionAddr = (offset + 3) & ~3
			traces.info('%-30s 0x%-15x 0x%x' % (comp.name, comp.size, comp.partitionAddr))
			offset = comp.partitionAddr + comp.size
		
		#
		# Now create the raw image as a byte array
		#
		# First header size
		self.appendLongInt(headerSize)
		
		# Number of components
		self.appendInt(len(self.compList))
		
		# Then for each component
		for comp in self.compList:
			# The partition address
			self.appendInt(comp.partitionAddr)
			
			# Binary size
			self.appendInt(comp.size)
			
			# The path length
			self.appendInt(len(comp.name) + 1)
			
			# And the path
			self += comp.name.encode('utf-8')
			self.appendByte(0)
		
		# Then dump all components
		for comp in self.compList:
			with open(comp.path, 'rb') as file:
				self.padToOffset(comp.partitionAddr)
				self += file.read()
		
		#
		# Write output
		#
		traces.newLine()
		traces.info("Write ReadFS image -> %s" % outputPath)
		traces.info("Total size: 0x%x bytes" %len(self))
		try:
			
			os.makedirs(os.path.realpath(os.path.dirname(outputPath)))
		except FileExistsError:
			pass
		
		with open(outputPath, 'wb') as file:
			file.write(self)


def getCompsFromDir(path, rec = False, incDirInName = True):
	files = []
	for file in os.listdir(path):
		fullPath = os.path.join(path, file)
		if os.path.isfile(fullPath):
			files.append(Comp(path, file, incDirInName))
		elif rec and os.path.isdir(fullPath):
			files.extend(getCompsFromDir(fullPath, rec = True, incDirInName = incDirInName))
	return files


def appendArgs(parser: argparse.ArgumentParser, partitionConfig: js.config=None) -> None:
	"""
	Append specific module arguments.

	:param parser:
	:type parser: argparse.ArgumentParser
	"""
	
	#
	# Input files
	#
	parser.add_argument("--comp-dir", dest = "compDir",
	                    action = "append", nargs = "*", default = [],
	                    help = "Component directory")
	parser.add_argument("--comp-dir-rec", dest = "compDirRec",
	                    action = "append", nargs = "*", default = [],
	                    help = "Recursive component directory")
	kwargs = {'default': []}
	if partitionConfig:
	    kwargs['default'] = [partitionConfig.get_py("files")]
	parser.add_argument("--comp", dest = "comp",
	                    action = "append", nargs = "*",
	                    help = "Component list",
                        **kwargs)
	
	parser.add_argument('--include-path', action = 'store_true', dest = 'incDirInName',
	                    help = 'Include folders path in component name')
	
	# Output
	kwargs = {'default': 'readfs.rfs'}
	if partitionConfig:
	    kwargs['default'] = partitionConfig.get_py("image")
	parser.add_argument('-o',
	                    dest = 'output',
	                    help = 'ReadFS partition image output',
                        **kwargs)


def operationFunc(args, config=None):

	if (len(args.comp) == 0 or (len(args.comp) == 1 and len(args.comp[0]) == 0)) and len(args.compDir) == 0 and len(args.compDirRec) == 0:
		return

	traces.info('Generating ReadFS image')

	# Concatenate input listsd
	compList = []
	for l in args.comp:
		compList += l
	
	compDirList = []
	for l in args.compDir:
		compDirList += l
	
	compDirRecList = []
	for l in args.compDirRec:
		compDirRecList += l
	
	#
	# Build ReadFS image
	#
	
	readFS = ReadFS()
	
	for comp in compList:
		is_enabled = True
		readFS.appendComponent(Comp(os.path.dirname(comp), os.path.basename(comp), args.incDirInName))
	
	for compDir in compDirList:
		for comp in getCompsFromDir(compDir, incDirInName = args.incDirInName):
			readFS.appendComponent(comp)
			is_enabled = True
	
	for compDir in compDirRecList:
		for comp in getCompsFromDir(compDir, rec = True, incDirInName = args.incDirInName):
			readFS.appendComponent(comp)
			is_enabled = True
			
	readFS.generate(args.output)

	if config is not None:
		config.set('enabled', True)


def main(custom_commandline = None, config = None, partition_config = None):
	"""
	Main function for build Flash image

	custom_commandline - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments
	as strings. Arguments and their values need to be added as individual items to the list e.g. "-b 115200" thus
	becomes ['-b', '115200'].
	"""
	
	parser = argparse.ArgumentParser(
		description = 'Build a Gap flash image from binary files - v%s' % __version__,
		prog = 'build_flash_image',
		fromfile_prefix_chars = '@')
	
	common.appendCommonOptions(parser)
	appendArgs(parser, partition_config)
	
	argcomplete.autocomplete(parser)

	# If no argument has been passed and a config has been passed, force arg_parse to not take arguments from sys.argv
	# as this would parse arguments for the calling script
	if custom_commandline is None and (config is not None or flash_config is not None):
		custom_commandline = []

	args = parser.parse_args(custom_commandline)
	
	try:
		operationFunc(args, config=partition_config)
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
