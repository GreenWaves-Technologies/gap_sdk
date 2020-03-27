#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import sys
import os

import argcomplete
import argparse

import errors
from errors import FatalError, NotSupportedError
import common
import traces
import binary
import json_tools as js
import shlex
import subprocess

__version__ = "0.1"

PYTHON2 = sys.version_info[0] < 3  # True if on pre-Python 3
if PYTHON2:
	print("Fatal error: Gapy needs to be run with python version 3")
	exit(1)




def appendArgs(parser: argparse.ArgumentParser, partitionConfig: js.config=None) -> None:
	"""
	Append specific module arguments.

	:param parser:
	:type parser: argparse.ArgumentParser
	"""
	
	# Output
	kwargs = {'default': 'lfs.img'}
	if partitionConfig:
	    kwargs['default'] = partitionConfig.get_py("image")
	parser.add_argument('-o',
	                    dest = 'output',
	                    help = 'LittleFS partition image output',
                        **kwargs)


def operationFunc(args, config=None):
	if config.get_str('root_dir') is None:
		return

	traces.info('Generating LittleFS image')
	
	cmd = 'mklfs -b 262144 -r 4 -p 4 -s 10485760 -c %s -i %s' % (config.get_str('root_dir'), args.output)

	traces.info('Generating LittleFS images with command:')
	traces.info('  ' + cmd)

	stdout = None if traces.verbose else subprocess.PIPE

	if subprocess.run(shlex.split(cmd), stdout=stdout).returncode != 0:
		raise errors.InputError('Failed to generate LittleFS image')

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
