import argparse


def appendCommonOptions(parser):
	"""
	Append common options shared by all sub commands.
	This function can be called by an other module to fetch options of Gapy.

	:param parser:
	:type parser: argparse.ArgumentParser
	"""
	
	# Chip
	# parser.add_argument('--chip',
	#                     help = 'Target chip type',
	#                     choices = ['GAP8', 'GAP8_V2'],
	#                     default = os.environ.get('TARGET_CHIP', ''))
	
	# Port
	# parser.add_argument(
	# 	'--port', '-p',
	# 	help='Serial port device',
	# 	default=os.environ.get('ESPTOOL_PORT', None))
	
	# Baud rate
	# parser.add_argument(
	# 	'--baud', '-b',
	# 	help='Serial port baud rate used when flashing/reading',
	# 	type=arg_auto_int,
	# 	default=os.environ.get('ESPTOOL_BAUD', ESPLoader.ESP_ROM_BAUD))
	
	# Quiet
	parser.add_argument('-q',
	                    action = 'store_false',
	                    dest = 'verbose',
	                    help = 'Quiet mode, print only the critical messages.')

def argToInt(value):
	""" Given a size or addresse of the type passed in args	(ie 512KB or 1MB) then return the value in bytes.
	In case of neither KB or MB was specified, 0x prefix can be used.
	"""
	for letter, multiplier in [("k", 1024), ("m", 1024 * 1024), ("kb", 1024), ("mb", 1024 * 1024), ]:
		if value.lower().endswith(letter):
			return int(value[:-len(letter)], 0) * multiplier
	else:
		return int(value, 0)


def hexify(s, uppercase = True):
	format_str = '%02X' if uppercase else '%02x'
	return ''.join(format_str % c for c in s)


class HexFormatter(object):
	"""
	Wrapper class which takes binary data in its constructor
	and returns a hex string as it's __str__ method.

	This is intended for "lazy formatting" of trace() output
	in hex format. Avoids overhead (significant on slow computers)
	of generating long hex strings even if tracing is disabled.

	Note that this doesn't save any overhead if passed as an
	argument to "%", only when passed to trace()

	If auto_split is set (default), any long line (> 16 bytes) will be
	printed as separately indented lines, with ASCII decoding at the end
	of each line.
	"""
	
	def __init__(self, binary_string, auto_split = True):
		self._s = binary_string
		self._auto_split = auto_split
	
	def __str__(self):
		if self._auto_split and len(self._s) > 16:
			result = ""
			s = self._s
			while len(s) > 0:
				line = s[:16]
				ascii_line = "".join(c if (c == ' ' or (c in string.printable and c not in string.whitespace))
				                     else '.' for c in line.decode('ascii', 'replace'))
				s = s[16:]
				result += "\n    %-16s %-16s | %s" % (hexify(line[:8], False), hexify(line[8:], False), ascii_line)
			return result
		else:
			return hexify(self._s, False)

class AddrFilenamePairAction(argparse.Action):
	""" Custom parser class for the address/filename pairs passed as arguments """
	
	def __init__(self, option_strings, dest, nargs = '+', **kwargs):
		super(AddrFilenamePairAction, self).__init__(option_strings, dest, nargs, **kwargs)
	
	def __call__(self, parser, namespace, values, option_string = None):
		# validate pair arguments
		pairs = []
		for i in range(0, len(values), 2):
			try:
				address = int(values[i], 0)
			except ValueError:
				raise argparse.ArgumentError(self, 'Address "%s" must be a number' % values[i])
			try:
				argfile = open(values[i + 1], 'rb')
			except IOError as e:
				raise argparse.ArgumentError(self, e)
			except IndexError:
				raise argparse.ArgumentError(self, 'Must be pairs of an address and the binary filename to write there')
			pairs.append((address, argfile))
		
		# Sort the addresses and check for overlapping
		end = 0
		for address, argfile in sorted(pairs):
			argfile.seek(0, 2)  # seek to end
			size = argfile.tell()
			argfile.seek(0)
			sector_start = address & ~(ESPLoader.FLASH_SECTOR_SIZE - 1)
			sector_end = ((address + size + ESPLoader.FLASH_SECTOR_SIZE - 1) & ~(ESPLoader.FLASH_SECTOR_SIZE - 1)) - 1
			if sector_start < end:
				message = 'Detected overlap at address: 0x%x for file: %s' % (address, argfile.name)
				raise argparse.ArgumentError(self, message)
			end = sector_end
		setattr(namespace, self.dest, pairs)


