#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description='Check TX buffer')

parser.add_argument("--file", dest="files", action="append", help="File containing the values to check")
parser.add_argument("--nb-samples", dest="nb_samples", default=None, type=int, help="File containing the values to check")
parser.add_argument("--is-interrupted", dest="is_interrupted", action="store_true", default=False, help="File containing the values to check")


args = parser.parse_args()

for filename in args.files[1:]:

    with open(args.files[0]) as ref:
        ref_lines = ref.readlines()

    with open(filename) as check:
        check_lines = check.readlines()

    index = 0
    sample_value = 0
    while True:
        if len(ref_lines) == 0 or len(check_lines) == 0:
            break

        ref_line = ref_lines.pop(0)
        check_line = check_lines.pop(0)

        ref_value = int(ref_line, 0)
        check_value = int(check_line, 0)

        if args.is_interrupted:
            if ref_value != 0:
                sample_value += 1
                expected_value = sample_value
            else:
                expected_value = 0

            if ref_value != check_value or expected_value != check_value:
                print ('Error detected (file: %s, index: %d, expected: %d, got: %d)' % (filename, index, ref_value, check_value))
                exit(-1)

            index += 1
        else:
            if check_value == 0 and index == 0:
                continue

            if check_value == 0:
                break

            sample_value += 1
            expected_value = sample_value

            if ref_value != check_value or expected_value != check_value:
                print ('Error detected (file: %s, index: %d, expected: %d, got: %d)' % (filename, index, ref_value, check_value))
                exit(-1)

            index += 1


    if index < args.nb_samples:
        print("Didn't get the right number of samples (file: %s, expected: %d, got: %d)" % (filename, args.nb_samples, index))

