#!/bin/bash
# run this script with: ./mylocaltest.sh <OS> <platform> <test path>
# for ex: 
# ./run_jenkins_loc.sh freertos gvsoc ./example
echo "Test path: $3 with OS $1 on platform $2"
export PMSIS_OS=$1
export PLPTEST_PLATFORM=$2
#use this line if you want to show output while execution
plptest --testset=$3/testset.cfg --stdout | tee test_log_$1_$2.log
