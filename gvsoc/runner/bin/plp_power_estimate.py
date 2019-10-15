#!/usr/bin/python

import re, os
import argparse
import os.path


parser = argparse.ArgumentParser(description='Generate power report')

parser.add_argument("--vcd", dest="vcd", default=None,
                    help='Path to VCD file')

parser.add_argument("--out", dest="out", default=os.getcwd(),
                    help='Path to report folder')

parser.add_argument("--platform", dest="platform", default=None,
                    help='Path to platform')

args = parser.parse_args()
   
if args.vcd == None:
    raise Exception('Path to VCD file must be specified through --vcd')

if args.platform == None:
    raise Exception('Path to simulation platform must be specified through --platform')

os.environ['VCD_FILE'] = args.vcd
os.environ['REPORT_PATH'] = args.out
os.environ['PULP_PLATFORM_PATH'] = args.platform
os.environ['PLS_CLUSTER_PATH'] = os.path.join(args.platform, 'power')
os.environ['PLS_SOC_PATH'] = os.path.join(args.platform, 'power')

print ('Power analysis')
print ('  VCD file: ' + args.vcd)
print ('  Report folder: ' + args.out)
print ('  Platform install: ' + args.platform)
print ('')

def readlines_reverse(qfile):
    qfile.seek(0, os.SEEK_END)
    position = qfile.tell()
    line = ''
    while position >= 0:
        qfile.seek(position)
        next_char = qfile.read(1)
        if next_char == "\n":
            yield line[::-1]
            line = ''
        else:
            line += next_char
        position -= 1
    yield line[::-1]


filename = args.vcd
vcd_file = open(filename, 'r')

start_time = -1
end_time = -1

print ('Looking for VCD start and stop time')

for line in vcd_file:
    if re.search("^#", line):
        time = int(line[1:])
        if time > 10:
            start_time = time
            break

print ("  Found start time: {0}".format(start_time))

# now look for end time
for line in readlines_reverse(vcd_file):
    if re.search("^#", line):
        time = int(line[1:])
        end_time = time
        break

print ("  Found end time: {0}".format(end_time))

vcd_file.close()

os.environ['VCD_START'] = str(int(start_time/1000) + 20)
os.environ['VCD_STOP'] = str(int(end_time/1000) - 20)

cmd = 'primetime-2012.12 pt_shell -f %s/power/scripts/run_power_est.tcl' % (args.platform)
print ('')
print ('Launching PrimeTime analysis with command: ' + cmd)
if os.system(cmd) != 0: raise Exception('Failed to execute PrimeTime analysis')
