#!/usr/bin/env python3

import argparse
import pexpect
import os
import sys
import signal
import random

parser = argparse.ArgumentParser(description='Launch test')

parser.add_argument("--run", dest="run", default=None, help="Give run command")
parser.add_argument("--proxy", dest="proxy", default=None, help="Give proxy command")

args = parser.parse_args()

while True:

    port = random.randint(4000, 20000)

    os.environ['GV_PROXY_PORT'] = str(port)
    run = pexpect.spawn(args.run, encoding='utf-8', logfile=sys.stdout)
    match = run.expect(['Opened proxy on socket '], timeout=None)
    if match == 0:
        break

    run.expect(pexpect.EOF, timeout=None)
    port = port + 1

proxy = pexpect.spawn(args.proxy, encoding='utf-8', logfile=sys.stdout)

while True:
    match = run.expect([pexpect.EOF, pexpect.TIMEOUT], timeout=1)
    if match == 0:
        break

    proxy.expect([pexpect.EOF, pexpect.TIMEOUT], timeout=1)
    


exit(run.exitstatus)