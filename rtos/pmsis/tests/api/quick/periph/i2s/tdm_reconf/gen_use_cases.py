#!/usr/bin/env python3

import argparse
import random

parser = argparse.ArgumentParser(description='Check TX buffer')

parser.add_argument("--duration", dest="duration", default=1000, type=int, help="I2S sampling duration in us")
parser.add_argument("--file", dest="file", default=None, help="File where to generate the use cases")
parser.add_argument("--sampling-freq", dest="sampling_freq", action="append", type=int, help="Sampling frequency")
parser.add_argument("--nb-itf", dest="nb_itf", default=None, type=int, help="Number of interfaces")
parser.add_argument("--nb-slots", dest="nb_slots", action="append", help="Number of slots. Can be specified several times, once per interface")
parser.add_argument("--muted-min", dest="muted_min", default=None, type=int, help="Minimum time in us during which a slot must be muted")
parser.add_argument("--muted-max", dest="muted_max", default=None, type=int, help="Maximum time in us during which a slot must be muted")
parser.add_argument("--unmuted-min", dest="unmuted_min", default=None, type=int, help="Minimum time in us during which a slot must be unmuted")
parser.add_argument("--unmuted-max", dest="unmuted_max", default=None, type=int, help="Maximum time in us during which a slot must be unmuted")
parser.add_argument("--width-min", dest="width_min", default=None, type=int, help="Minimum slot width")
parser.add_argument("--width-max", dest="width_max", default=None, type=int, help="Maximum slot width")
parser.add_argument("--left-align", dest="left_align", action="append", type=int, help="Specify possible left align value")
parser.add_argument("--msb-first", dest="msb_first", action="append", type=int, help="Specify possible msb first value")
parser.add_argument("--sign-extend", dest="sign_extend", action="append", type=int, help="Specify possible sign extend value")

args = parser.parse_args()


if args.duration >= 100:
    duration = 50000
elif args.duration >= 75:
    duration = 20000
elif args.duration >= 50:
    duration = 10000
elif args.duration >= 25:
    duration = 5000
else:
    duration = 2000


with open(args.file, 'w+') as fd:

    fd.write('#include "test.h"\n\n')

    for itf in range(0, args.nb_itf):

        for slot in range(0, int(args.nb_slots[itf])):
            
            fd.write('static slot_usecase_t itf%d_slot%d_usecases[] = {\n' % (itf, slot))

            time = 0
            while time < duration:
                unmuted = random.randint(args.unmuted_min, args.unmuted_max)
                muted = random.randint(args.muted_min, args.muted_max)
                width = random.randint(args.width_min, args.width_max)
                if len(args.left_align) != 0:
                    left_align = args.left_align[random.randint(0, len(args.left_align) - 1)]
                else:
                    left_align = 0
                if len(args.msb_first) != 0:
                    msb_first = args.msb_first[random.randint(0, len(args.msb_first) - 1)]
                else:
                    msb_first = 0
                if len(args.sign_extend) != 0:
                    sign_extend = args.sign_extend[random.randint(0, len(args.sign_extend) - 1)]
                else:
                    sign_extend = 0
                fd.write('    { .unmuted_time=%d, .muted_time=%d, .width=%d, .msb_first=%d, .left_align=%d, .sign_extend=%d },\n' % (unmuted, muted, width, msb_first, left_align, sign_extend))

                time += unmuted + muted

            fd.write('};\n\n')

    for itf in range(0, args.nb_itf):
        fd.write('static slot_desc_t itf%d_slot_descs[] = {\n' % itf)
        for slot in range(0, int(args.nb_slots[itf])):
            fd.write('    { .nb_usecases=sizeof(itf%d_slot%d_usecases) / sizeof(slot_usecase_t), .usecases=itf%d_slot%d_usecases },\n' % (itf, slot, itf, slot))
        fd.write('};\n\n')
            
    fd.write('static itf_desc_t itf_descs[] = {\n')
    for itf in range(0, args.nb_itf):
        fd.write('    { .sampling_freq=%d, .nb_slots=%d, .slot_descs=itf%d_slot_descs },\n' % (int(args.sampling_freq[itf]), int(args.nb_slots[itf]), itf))
    fd.write('};\n\n')

    fd.write('test_desc_t test_desc = { .nb_itf=%d, .itf_descs=itf_descs };\n' % args.nb_itf)
