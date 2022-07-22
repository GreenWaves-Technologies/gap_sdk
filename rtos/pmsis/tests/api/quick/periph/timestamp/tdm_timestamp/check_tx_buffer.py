#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description='Check TX buffer')

parser.add_argument("--start", dest="start", default=None, type=int, help="Start value")
parser.add_argument("--incr", dest="incr", default=None, type=int, help="Incr value")
parser.add_argument("--end", dest="end", default=None, type=int, help="End value")
parser.add_argument("--word-size", dest="word_size", default=None, type=int, help="Word size")
parser.add_argument("--slot-size-0", dest="slot_size_0", default=None, type=int, help="Slot size")
parser.add_argument("--slot-size-1", dest="slot_size_1", default=None, type=int, help="Slot size")
parser.add_argument("--format", dest="format", default=None, type=int, help="Format")
parser.add_argument("--nb-elem", dest="nb_elem", default=None, type=int, help="Number of elements")
parser.add_argument("--slot-id", dest="slot", default=None, type=int, help="Slot")
parser.add_argument("--file", dest="file", default=None, help="File containing the values to check")
parser.add_argument("--mute", dest="mute", action="store_true", help="Skip zero samples in the middle since channel was muted")

args = parser.parse_args()

incr = args.incr

is_first = True

slot_size = args.slot_size_0 if args.slot < 8 else args.slot_size_1

if slot_size is not None:
    args.end = args.end & ((1<<slot_size) - 1)
    args.start = args.start & ((1<<slot_size) - 1)

    if args.incr >= args.end:
        args.incr = 0


hit_first_value = False

sign_extend = args.format in [ 5, 6]

with open(args.file) as fp:
    index = 0
    for line in fp.readlines():
        index += 1

        if index == args.nb_elem:
            break

        value = int(line, 0)

        if (not hit_first_value or args.mute) and value == 0:
            continue

        hit_first_value = True

        if is_first or args.mute:
            is_first = False

            if args.incr != 0:
                fact = int((value - args.start) / args.incr)
                current_value = fact*args.incr + args.start
            else:
                current_value = args.start

        if sign_extend and ((current_value >> (slot_size - 1)) & 1) == 1:
            current_value |= ((1 << (args.word_size - slot_size)) - 1) << slot_size
            

        if value != current_value:
            print ('Error at index %d, expected 0x%x, got 0x%x' % (index, current_value, value))
            exit(1)


        if args.end - (current_value & ((1 << slot_size) - 1)) <= incr:
            current_value = args.start
        else:
            current_value += incr


if not hit_first_value:
    exit(1)