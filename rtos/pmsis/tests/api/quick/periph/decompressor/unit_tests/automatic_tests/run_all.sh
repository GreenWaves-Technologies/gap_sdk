#!/usr/bin/env bash

set -euo pipefail

CURWD="$(pwd)"

test_dir() {
    dir_test="${1}"
    echo "Testing ${dir_test}"
    cd ${dir_test}
    ./run.pl
    cd ${CURWD}
}

echo "========================================================="
echo "Decompressor automatic extensive tests"
echo "WARNING: May take a really long time, up to multiple days"
echo "========================================================="

sleep 5

test_dir decompress_lin_lin
test_dir compress_lin_lin
test_dir decompress_2d_lin
test_dir decompress_lin_2d
