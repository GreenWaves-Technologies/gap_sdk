#!/usr/bin/env python3

import argparse
import pexpect
import os
import sys
import signal
import random

parser = argparse.ArgumentParser(description='Launch test')

parser.add_argument("--run", dest="run", default=None, help="Give run command")
parser.add_argument("--ocd", dest="ocd", default=None, help="Give ocd command")

args = parser.parse_args()

while True:

    port = random.randint(4000, 20000)

    os.environ['GAPY_OPENOCD_BITBANG_PORT'] = str(port)
    run = pexpect.spawn(args.run, encoding='utf-8', logfile=sys.stdout)
    match = run.expect(['Listening on', 'Address already in use'], timeout=None)
    if match == 0:
        break

    run.expect(pexpect.EOF, timeout=None)
    port = port + 1

ocd = pexpect.spawn(args.ocd, encoding='utf-8', logfile=sys.stdout)

while True:
    match = ocd.expect([pexpect.EOF, pexpect.TIMEOUT], timeout=1)
    if match == 0:
        break

    run.expect([pexpect.EOF, pexpect.TIMEOUT], timeout=1)
    


os.killpg(run.pid, signal.SIGKILL)

exit(ocd.exitstatus)