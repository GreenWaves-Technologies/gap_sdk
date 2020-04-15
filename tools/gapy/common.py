import os
import sys

import argparse
import traces

import json_tools as js

import importlib

gapyDir = os.path.dirname(os.path.realpath(__file__))
targetsDir = os.path.join(gapyDir, 'targets')
configsDir = os.path.join(gapyDir, 'configs')
gapyJsonPath = [targetsDir, configsDir]
openocdDir = os.path.join(gapyDir, 'openocd')


def get_platforms():
    platform_list = {}
    for path in js.get_paths(paths=gapyJsonPath):
        config_path = os.path.join(path, 'platforms')
        if os.path.exists(config_path):
            files = os.listdir(config_path)
            for platform_file in files:
                name = platform_file[:platform_file.index('.json')]
                if platform_list.get(name) is None:
                    platform_list[name] = os.path.join(config_path, platform_file)

    return platform_list



def appendCommonOptions(parser):
    """
    Append common options shared by all sub commands.
    This function can be called by an other module to fetch options of Gapy.

    :param parser:
    :type parser: argparse.ArgumentParser
    """
    
    # Target
    targetJsonList = os.listdir(targetsDir)
    targetList = []
    for target in targetJsonList:
        targetList.append(target[:target.index('.json')])
    parser.add_argument('--target',
                        dest = 'target',
                        default = 'None',
                        choices = targetList,
                        help = 'Gives the name of the target configuration.')
    
    #
    # Plateforms
    #
    platforms = get_platforms()
    parser.add_argument('--platform',
                        dest = 'platform',
                        default = None,
                        choices = platforms,
                        help = 'Gives the name of the platform configuration.')

    [args, otherArgs] = parser.parse_known_args()

    if args.platform is not None:
        os.environ['GAPY_PLATFORM'] = args.platform

    # Verbose
    parser.add_argument('-v',
                        action = 'store_true',
                        dest = 'verbose',
                        help = 'Verbose mode, print all messages.')

    parser.add_argument('--work-dir',
                        dest = 'work_dir',
                        default = None,
                        help = 'Specifies working directory.')

    parser.add_argument('--config-ini',
                        dest = 'ini_configs',
                        action = "append",
                        default = [],
                        help = 'Specifies additionnal ini configuration.')

    parser.add_argument('--config-opt',
                        dest = 'config_items',
                        action = "append",
                        default = [],
                        help = 'Specifies additionnal configuration items.')


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


def parseOptions(args, config):

    if config is not None:
        config.set('gapy/verbose', args.verbose)

    traces.verbose = args.verbose

    work_dir = args.work_dir
    if work_dir is None:
        work_dir = os.getcwd()

    if config is not None:
        config.set('gapy/work_dir', work_dir)


def importConfig(parser):
    argsCli = sys.argv[1:]
    if '-h' in argsCli:
        argsCli.remove('-h')
    parser.parse_known_args(argsCli)
    args, _ = parser.parse_known_args(argsCli)
    if args.target is not None:
        jsonPath = os.path.join(targetsDir, args.target + '.json')
        ini_configs = args.ini_configs

        if os.environ.get('GAPY_CONFIGS_INI') is not None:
            ini_configs += os.environ.get('GAPY_CONFIGS_INI').split()

        return js.import_config_from_file(jsonPath, find = True, interpret = True, paths = gapyJsonPath, ini_configs=ini_configs, config_items=args.config_items)
    else:
        return None


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
