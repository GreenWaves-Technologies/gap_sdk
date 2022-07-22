#!/usr/bin/env bash

set -euo pipefail

BUILDDIR=$1

NUM_LINES=705
EXP_FILE="expected.txt"
RES_FILE="${BUILDDIR}/outfile.txt"

diff <(head -n ${NUM_LINES} ${EXP_FILE}) <(head -n ${NUM_LINES} ${RES_FILE})
