import os
import argparse
import hashlib
import struct
import re

import traces
import binary
from errors import InputError, FatalError


PARTITION_HEADER_SIZE = 32
PARTITION_TABLE_ENTRY_SIZE = 32
MAX_PARTITION_TABLE_ENTRIES = 31
MAX_PARTITION_TABLE_SIZE = MAX_PARTITION_TABLE_ENTRIES * PARTITION_TABLE_ENTRY_SIZE + PARTITION_HEADER_SIZE

MIN_PARTITION_SUBTYPE_APP_OTA = 0x10
NUM_PARTITION_SUBTYPE_APP_OTA = 2

APP_TYPE = 0x00
DATA_TYPE = 0x01

TYPES = {
	"app": APP_TYPE,
	"data": DATA_TYPE,
}

FACTORY_SUBTYPE = 0x00
APP_OTA_MIN_SUBTYPE = 0x10
APP_OTA0_SUBTYPE = APP_OTA_MIN_SUBTYPE + 0
APP_OTA1_SUBTYPE = APP_OTA_MIN_SUBTYPE + 1
TEST_SUBTYPE = 0x20
UPDATER0_SUBTYPE = 0x30
UPDATER1_SUBTYPE = 0x31

APP0_SUBTYPE = 0x40
APP1_SUBTYPE = 0x41
APP2_SUBTYPE = 0x42
APP3_SUBTYPE = 0x43
APP4_SUBTYPE = 0x44
APP5_SUBTYPE = 0x45
APP6_SUBTYPE = 0x46
APP7_SUBTYPE = 0x47
APP8_SUBTYPE = 0x48
APP9_SUBTYPE = 0x49

DATA_OTA_SUBTYPE = 0x0
RAW_SUBTYPE = 0x80
READFS_SUBTYPE = 0x81
LFS_SUBTYPE = 0x82

# Keep this map in sync with pi_partition_subtype_t enum in pi_partition.h
SUBTYPES = {
	APP_TYPE: {
		"factory": FACTORY_SUBTYPE,
		"ota0": APP_OTA0_SUBTYPE,
		"ota1": APP_OTA1_SUBTYPE,
		"test": TEST_SUBTYPE,
		"updater0": UPDATER0_SUBTYPE,
		"updater1": UPDATER1_SUBTYPE,
		"app0": APP0_SUBTYPE,
		"app1": APP1_SUBTYPE,
		"app2": APP2_SUBTYPE,
		"app3": APP3_SUBTYPE,
		"app4": APP4_SUBTYPE,
		"app5": APP5_SUBTYPE,
		"app6": APP6_SUBTYPE,
		"app7": APP7_SUBTYPE,
		"app8": APP8_SUBTYPE,
		"app9": APP9_SUBTYPE,
	},
	DATA_TYPE: {
		"ota": DATA_OTA_SUBTYPE,
		"readfs": READFS_SUBTYPE,
		"lfs": LFS_SUBTYPE,
	},
}


def isBinaryPartitionTable(table):
	return table[0:2] == PartitionTable.MAGIC_BYTES


class ValidationError(InputError):
	def __init__(self, partition, message):
		super(ValidationError, self).__init__(
			"Partition %s invalid: %s" % (partition.name, message))

class PartitionNameFilenamePairAction(argparse.Action):
	def __init__(self, option_strings, dest, nargs = 2, default = {}, **kwargs):
		if nargs is not 2:
			raise ValueError("nargs not allowed")
		super(PartitionNameFilenamePairAction, self).__init__(
			option_strings = option_strings,
			dest = dest,
			nargs = nargs,
			default = default,
			**kwargs)
	
	def __call__(self, parser, namespace, values, option_string = None):
		# Fetch the dictionary partitions which could have been created by a previous call.
		entry = getattr(namespace, self.dest)
		if entry is None:
			entry = {}
		
		try:
			entry[values[0]] = open(values[1], 'rb')
		except OSError as e:
			raise argparse.ArgumentError(self, "can't open '%s': %s" % (values[1], e))
		
		setattr(namespace, self.dest, entry)

def parse_int(v, keywords = {}, sectorSize = None):
	"""Generic parser for integer fields - int(x,0) with provision for
	k/m/K/M suffixes and 'keyword' value lookup.
	"""
	try:
		for letter, multiplier in [("k", 1024), ("m", 1024 * 1024), ("kb", 1024), ("mb", 1024 * 1024), ('sec', sectorSize)]:
			if v.lower().endswith(letter):
				return parse_int(v[:-len(letter)], keywords, sectorSize) * multiplier
		return int(v, 0)
	except ValueError:
		if len(keywords) == 0:
			raise InputError("Invalid field value %s" % v)
		try:
			return keywords[v.lower()]
		except KeyError:
			raise InputError("Value '%s' is not valid. Known keywords: %s" % (v, ", ".join(keywords)))

class PartitionDefinition(object):
	MAGIC_BYTES = b"\xBA\x01"
	
	# dictionary maps flag name (as used in CSV flags list, property name)
	# to bit set in flags words in binary format
	FLAGS = {
		"encrypted": 0
	}
	
	# add subtypes for the 16 OTA slot values ("ota_XX, etc.")
	for ota_slot in range(NUM_PARTITION_SUBTYPE_APP_OTA):
		SUBTYPES[TYPES["app"]]["ota_%d" % ota_slot] = MIN_PARTITION_SUBTYPE_APP_OTA + ota_slot
	
	def __init__(self, name = '', type = None, subtype = None, offset = None, size = None, encrypted = False, path = None):
		self.name = name
		self.type = type
		self.subtype = subtype
		self.offset = offset
		self.size = size
		self.encrypted = encrypted
		self.path = path
	
	@classmethod
	def from_csv(cls, line, line_no, sectorSize):
		""" Parse a line from the CSV """
		line_w_defaults = line + ",,,,"  # lazy way to support default fields
		fields = [f.strip() for f in line_w_defaults.split(",")]
		
		res = PartitionDefinition()
		res.line_no = line_no
		res.name = fields[0]
		res.type = res.parse_type(fields[1])
		res.subtype = res.parse_subtype(fields[2])
		res.offset = res.parse_address(fields[3], sectorSize = sectorSize)
		res.size = res.parse_address(fields[4], sectorSize = sectorSize)
		if res.size is None:
			raise InputError("Size field can't be empty")
		
		flags = fields[5].split(":")
		for flag in flags:
			if flag in cls.FLAGS:
				setattr(res, flag, True)
			elif len(flag) > 0:
				raise InputError("CSV flag column contains unknown flag '%s'" % (flag))
		
		return res
	
	def __eq__(self, other):
		return self.name == other.name and self.type == other.type \
		       and self.subtype == other.subtype and self.offset == other.offset \
		       and self.size == other.size
	
	def __repr__(self):
		def maybe_hex(x):
			return "0x%x" % x if x is not None else "None"
		
		return "PartitionDefinition('%s', 0x%x, 0x%x, %s, %s)" % (self.name, self.type, self.subtype or 0,
		                                                          maybe_hex(self.offset), maybe_hex(self.size))
	
	def __str__(self):
		return "Part '%s' %d/%d @ 0x%x size 0x%x" % (
			self.name, self.type, self.subtype, self.offset or -1, self.size or -1)
	
	def __cmp__(self, other):
		return self.offset - other.offset
	
	def __lt__(self, other):
		return self.offset < other.offset
	
	def __gt__(self, other):
		return self.offset > other.offset
	
	def __le__(self, other):
		return self.offset <= other.offset
	
	def __ge__(self, other):
		return self.offset >= other.offset
	
	def parse_type(self, strval):
		if strval == "":
			raise InputError("Field 'type' can't be left empty.")
		return parse_int(strval, TYPES)
	
	def parse_subtype(self, strval):
		if strval == "":
			return 0  # default
		return parse_int(strval, SUBTYPES.get(self.type, {}))
	
	def parse_address(self, strval, sectorSize):
		if strval == "":
			return None  # PartitionTable will fill in default
		return parse_int(strval, sectorSize = sectorSize)
	
	def verify(self, align):
		# Minimal align check
		if align is None:
			align = 4
		
		if self.type is None:
			raise ValidationError(self, "Type field is not set")
		if self.subtype is None:
			raise ValidationError(self, "Subtype field is not set")
		if self.offset is None:
			raise ValidationError(self, "Offset field is not set")
		if self.size is None:
			raise ValidationError(self, "Size field is not set")
		
		if self.offset % align:
			raise ValidationError(self, "Offset 0x%x is not aligned to 0x%x" % (self.offset, align))
		
		if self.name in TYPES and TYPES.get(self.name, "") != self.type:
			traces.critical(
				"WARNING: Partition has name '%s' which is a partition type, but does not match this partition's "
				"type (0x%x). Mistake in partition table?" % (self.name, self.type))
		all_subtype_names = []
		for names in (t.keys() for t in SUBTYPES.values()):
			all_subtype_names += names
		if self.name in all_subtype_names and SUBTYPES.get(self.type, {}).get(self.name, "") != self.subtype:
			traces.critical("WARNING: Partition has name '%s' which is a partition subtype, but this partition has "
			                "non-matching type 0x%x and subtype 0x%x. Mistake in partition table?" % (
				                self.name, self.type, self.subtype))
	
	STRUCT_FORMAT = b"<2sBBLL16sL"
	
	@classmethod
	def from_binary(cls, b):
		if len(b) != 32:
			raise InputError("Partition definition length must be exactly 32 bytes. Got %d bytes." % len(b))
		res = cls()
		(magic, res.type, res.subtype, res.offset,
		 res.size, res.name, flags) = struct.unpack(cls.STRUCT_FORMAT, b)
		if b"\x00" in res.name:  # strip null byte padding from name string
			res.name = res.name[:res.name.index(b"\x00")]
		res.name = res.name.decode()
		if magic != cls.MAGIC_BYTES:
			raise InputError("Invalid magic bytes (%r) for partition definition" % magic)
		for flag, bit in cls.FLAGS.items():
			if flags & (1 << bit):
				setattr(res, flag, True)
				flags &= ~(1 << bit)
		if flags != 0:
			traces.critical("WARNING: Partition definition had unknown flag(s) 0x%08x. Newer binary format?" % flags)
		return res
	
	def get_flags_list(self):
		return [flag for flag in self.FLAGS.keys() if getattr(self, flag)]
	
	def to_binary(self):
		flags = sum((1 << self.FLAGS[flag]) for flag in self.get_flags_list())
		return struct.pack(self.STRUCT_FORMAT,
		                   self.MAGIC_BYTES,
		                   self.type, self.subtype,
		                   self.offset, self.size,
		                   self.name.encode(),
		                   flags)
	
	def to_csv(self, simple_formatting = False):
		def addr_format(a, include_sizes):
			if not simple_formatting and include_sizes:
				for (val, suffix) in [(0x100000, "M"), (0x400, "K")]:
					if a % val == 0:
						return "%d%s" % (a // val, suffix)
			return "0x%x" % a
		
		def lookup_keyword(t, keywords):
			for k, v in keywords.items():
				if simple_formatting is False and t == v:
					return k
			return "%d" % t
		
		def generate_text_flags():
			""" colon-delimited list of flags """
			return ":".join(self.get_flags_list())
		
		return ",".join([self.name,
		                 lookup_keyword(self.type, TYPES),
		                 lookup_keyword(self.subtype, SUBTYPES.get(self.type, {})),
		                 addr_format(self.offset, False),
		                 addr_format(self.size, True),
		                 generate_text_flags()])


class PartitionTable(list):
	MAGIC_BYTES = b"\xBA\x02"
	format_version = 1
	
	#
	# Header
	# Magic number : 2 bytes
	# Partition table version : 1 byte
	# Number of entries : 1 byte
	# CRC flag : 1 byte
	# Pad : 11 bytes
	# MD5 of entries : 16 bytes
	HEADER_FORMAT = b"<2sBBB11x16s"
	
	def __init__(self, md5Sum = True):
		super(PartitionTable, self).__init__(self)
		self.md5Sum = md5Sum
	
	@classmethod
	def from_csv(cls, csv_contents, partitionTableOffset = None, sectorSize = None, md5Sum = True):
		res = PartitionTable(md5Sum)
		
		lines = csv_contents.splitlines()
		
		def expand_vars(f):
			f = os.path.expandvars(f)
			m = re.match(r'(?<!\\)\$([A-Za-z_][A-Za-z0-9_]*)', f)
			if m:
				raise InputError("unknown variable '%s'" % m.group(1))
			return f
		
		for line_no in range(len(lines)):
			line = expand_vars(lines[line_no]).strip()
			if line.startswith("#") or len(line) == 0:
				continue
			try:
				res.append(PartitionDefinition.from_csv(line, line_no + 1, sectorSize))
			except InputError as e:
				raise InputError("Error at line %d: %s" % (line_no + 1, e))
			except Exception:
				raise InputError("Unexpected error parsing CSV line %d: %s" % (line_no + 1, line))
		
		# To fill automaticaly partition offset fields, partition table offset and sector size must be know.
		if (not (partitionTableOffset and sectorSize)):
			for e in res:
				if (e.offset is None):
					raise InputError("Flash offset of the `%s' partition is empty in the CSV input file.\n"
					                 "So that the gen_partition script automaticaly calculates this offset, you must specify partition table offset (--offset) "
					                 "and flash sector size (--flash-sector-size) from the command line." % e.name)
			return res
		
		# fix up missing offsets
		last_end = partitionTableOffset + res.tableSize()  # first offset after partition table
		last_end = binary.align(last_end, sectorSize)
		for e in res:
			if e.offset is not None and e.offset < last_end:
				if e == res[0]:
					raise InputError("CSV Error: First partition offset 0x%x overlaps end of partition table 0x%x"
					                 % (e.offset, last_end))
				else:
					raise InputError(
						"CSV Error: Partitions overlap. Partition at line %d sets offset 0x%x. Previous partition ends 0x%x"
						% (e.line_no, e.offset, last_end))
			
			if e.offset is None:
				if last_end % sectorSize != 0:
					last_end = binary.align(last_end, sectorSize)
				e.offset = last_end
			
			last_end = e.offset + e.size
		
		return res
	
	def __getitem__(self, item):
		""" Allow partition table access via name as well as by
		numeric index. """
		if isinstance(item, str):
			for x in self:
				if x.name == item:
					return x
			raise ValueError("No partition entry named '%s'" % item)
		else:
			return super(PartitionTable, self).__getitem__(item)
	
	def find_by_type(self, ptype, subtype):
		""" Return a partition by type & subtype, returns
		None if not found """
		# convert ptype & subtypes names (if supplied this way) to integer values
		try:
			ptype = TYPES[ptype]
		except KeyError:
			try:
				ptype = int(ptype, 0)
			except TypeError:
				pass
		try:
			subtype = SUBTYPES[int(ptype)][subtype]
		except KeyError:
			try:
				ptype = int(ptype, 0)
			except TypeError:
				pass
		
		for p in self:
			if p.type == ptype and p.subtype == subtype:
				return p
		return None
	
	def find_by_name(self, name):
		for p in self:
			if p.name == name:
				return p
		return None
	
	def verify(self, partitionTableOffset = None, flashSectorSize = None, flashSize = None):
		# verify each partition individually
		for p in self:
			p.verify(flashSectorSize)
		# check on duplicate name
		names = [p.name for p in self]
		duplicates = set(n for n in names if names.count(n) > 1)
		
		# print sorted duplicate partitions by name
		if len(duplicates) != 0:
			print("A list of partitions that have the same name:")
			for p in sorted(self, key = lambda x: x.name):
				if len(duplicates.intersection([p.name])) != 0:
					print("%s" % (p.to_csv()))
			raise InputError("Partition names must be unique")
		
		# check for overlaps
		last = None
		for p in sorted(self, key = lambda x: x.offset):
			if p.offset < partitionTableOffset + self.tableSize():
				raise InputError(
					"%s partition offset 0x%x is below the end of partition table at 0x%x" % (
						p.name, p.offset, partitionTableOffset + self.tableSize()))
			if last is not None and p.offset < last.offset + last.size:
				raise InputError(
					"Partition at 0x%x overlaps 0x%x-0x%x" % (p.offset, last.offset, last.offset + last.size - 1))
			last = p
		
		if (flashSize and flashSize < self.flashSize()):
			raise InputError(
				"Partitions occupy %.1fMB of flash which does not fit in configured flash size of %dMB." %
				(self.flashSize() / 1024.0 / 1024.0, flashSize / 1024 ** 2))
	
	def tableSize(self):
		return len(self) * PARTITION_TABLE_ENTRY_SIZE + PARTITION_HEADER_SIZE
	
	def flashSize(self):
		""" Return the size that partitions will occupy in flash
			(ie the offset the last partition ends at)
		"""
		try:
			last = sorted(self, reverse = True)[0]
		except IndexError:
			return 0  # empty table!
		return last.offset + last.size
	
	@classmethod
	def from_binary(cls, b):
		md5 = hashlib.md5()
		if len(b) % 32 != 0:
			raise InputError("Partition table length must be a multiple of 32 bytes")
		
		result = cls()
		magic, version, nbrOfEntries, result.md5Sum, hash = struct.unpack_from(PartitionTable.HEADER_FORMAT, b)
		
		if magic != PartitionTable.MAGIC_BYTES:
			raise InputError("Magic number of the partition table header don't match!")
		if version != PartitionTable.format_version:
			raise InputError("Format version of the partition table header missmatch!")
		
		for o in range(32, 32 + nbrOfEntries * 32, 32):
			data = b[o:o + 32]
			md5.update(data)
			result.append(PartitionDefinition.from_binary(data))
		
		if result.md5Sum and hash != md5.digest():
			raise InputError("MD5 checksums don't match! (computed: 0x%s, parsed: 0x%s)" % (
				md5.hexdigest(), binascii.hexlify(hash)))
		return result
	
	def to_binary(self):
		entries = b"".join(e.to_binary() for e in self)
		
		if self.md5Sum:
			hash = hashlib.md5(entries).digest()
		else:
			hash = b"\xFF" * 16
		
		result = binary.BlockBuffer()
		result += struct.pack(self.HEADER_FORMAT,
		                      self.MAGIC_BYTES,
		                      self.format_version,
		                      len(self),
		                      self.md5Sum,
		                      hash)
		
		result += entries
		return result
	
	def to_csv(self, simple_formatting = False):
		rows = ["# GAP Partition Table",
		        "# Name, Type, SubType, Offset, Size, Flags"]
		rows += [x.to_csv(simple_formatting) for x in self]
		return "\n".join(rows) + "\n"
