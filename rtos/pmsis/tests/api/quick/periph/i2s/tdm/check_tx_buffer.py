#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description='Check TX buffer')

parser.add_argument("--start", dest="start", default=None, type=int, help="Start value")
parser.add_argument("--incr", dest="incr", default=None, type=int, help="Incr value")
parser.add_argument("--width", dest="width", default=None, type=int, help="Incr value")
parser.add_argument("--nb-elem", dest="nb_elem", default=None, type=int, help="Number of elements")
parser.add_argument("--file", dest="file", default=None, help="File containing the values to check")

args = parser.parse_args()

incr = args.incr
current_value = args.start

is_first = True

with open(args.file) as fp:
    index = 0
    for line in fp.readlines():
        index += 1

        if index == args.nb_elem:
            break

        value = int(line, 0)

        if value == 0:
            continue

        if is_first:
            is_first = False

            fact = int((value - args.start) / args.incr)
            current_value = fact*args.incr + args.start

        if value != (current_value & ((1<<args.width) -1)):
            print ('Error at index %d, expected 0x%x, got 0x%x' % (index, current_value, value))
            exit(1)

        current_value += incr
