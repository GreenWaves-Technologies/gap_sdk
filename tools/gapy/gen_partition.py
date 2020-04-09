#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

#
# GAP partition table generation tool
#
# Converts partition tables to/from CSV and binary formats.
#
#

import os
import sys
import argparse
import argcomplete
import errno

import traces
import common
import binary
from errors import InputError, FatalError
from partition import *

__version__ = '0.1'



def appendArgs(parser: argparse.ArgumentParser) -> None:
	"""
	Append specific module arguments.

	:param parser:
	:type parser: argparse.ArgumentParser
	"""
	
	#
	# Input files
	#
	parser.add_argument('--flash-size', '-s',
	                    dest = 'flashSize',
	                    help = 'Optional flash size limit, checks partition table fits in flash',
	                    type = common.argToInt, default = '64MB')
	
	parser.add_argument('--disable-md5sum', help = 'Disable md5 checksum for the partition table', default = False,
	                    action = 'store_false', dest = 'md5Sum')
	
	parser.add_argument('--no-verify', help = "Don't verify partition table fields",
	                    action = 'store_true')
	
	parser.add_argument('--offset', default = '512KB',
	                    type = common.argToInt, dest = 'partitionTableOffset',
	                    help = "Set offset partition table. Usefull to check overlap and to compute partition offset if it is not set in CSV file.")
	
	parser.add_argument('--flash-block-size', '-b', default = None,
	                    type = common.argToInt, dest = 'blockSize',
	                    help = "Erasable flash sectore size. Usefull to compute partition offset if it is not set in CSV file.")
	
	# parser.add_argument('--secure', help = "Require app partitions to be suitable for secure boot",
	#                     action = 'store_true')
	
	parser.add_argument('input', help = 'Path to CSV or binary file to parse.',
	                    type = argparse.FileType('rb'))
	
	parser.add_argument('output', help = 'Path to output converted binary or CSV file. Will use stdout if omitted.',
	                    nargs = '?', default = '-')


def operationFunc(args, config=None):
	
	traces.info("gen_partition tools")
	
	input = args.input.read()
	if isBinaryPartitionTable(input):
		traces.info("Parsing binary partition file input: %s..." % args.input.name)
		table = PartitionTable.from_binary(input)
	else:
		traces.info("Parsing CSV input file: %s..." % args.input.name)
		try:
			input = input.decode()
		except UnicodeDecodeError:
			raise InputError('"%s" input file must be a CSV text file or partition table binary.' % args.input.name)
		table = PartitionTable.from_csv(input, args.partitionTableOffset, args.blockSize, args.md5Sum)
	
	if not args.no_verify:
		traces.info("Verifying table...")
		table.verify(partitionTableOffset = args.partitionTableOffset,
		             flashSectorSize = args.blockSize,
		             flashSize = args.flashSize)
	
	# Make sure that the output directory is created
	output_dir = os.path.abspath(os.path.dirname(args.output))
	
	if not os.path.exists(output_dir):
		try:
			os.makedirs(output_dir)
		except OSError as exc:
			if exc.errno != errno.EEXIST:
				raise
	
	if isBinaryPartitionTable(input):
		output = table.to_csv()
		with sys.stdout if args.output == '-' else open(args.output, 'w') as f:
			f.write(output)
	else:
		output = table.to_binary()
		try:
			stdout_binary = sys.stdout.buffer  # Python 3
		except AttributeError:
			stdout_binary = sys.stdout
		with stdout_binary if args.output == '-' else open(args.output, 'wb') as f:
			f.write(output)


def main(custom_commandline = None):
	"""
	Main function for build Flash image

	custom_commandline - Optional override for default arguments parsing (that uses sys.argv), can be a list of custom arguments
	as strings. Arguments and their values need to be added as individual items to the list e.g. "-b 115200" thus
	becomes ['-b', '115200'].
	"""
	
	parser = argparse.ArgumentParser(
		description = 'GAP partition table utility - v%s' % __version__,
		prog = 'gen_partition',
		fromfile_prefix_chars = '@')
	
	common.appendCommonOptions(parser)
	appendArgs(parser)
	
	argcomplete.autocomplete(parser)
	args = parser.parse_args()
	
	operationFunc(args)




if __name__ == '__main__':
	try:
		main()
	except InputError as e:
		print('\nInput fatal error: ', e, file = sys.stderr)
		sys.exit(1)
	except FatalError as e:
		print('\nA fatal error occurred: ', e)
		sys.exit(2)
