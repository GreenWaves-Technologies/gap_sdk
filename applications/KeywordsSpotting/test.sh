#!/bin/bash

# Copyright 2019 GreenWaves Technologies, SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


declare -a list_word=( "dummy" "silence" "none" "yes" "no" "up" "down" "left" "right" "on" "off" "stop"  "go")   

#directory to store the test files
#if [ ! -d reduced_test_set ]; then
#   mkdir reduced_test_set
#fi
   
make clean all
echo "skip $2 first tests"
test=0
test_err=0
while IFS='' read -r line || [[ -n "$line" ]]; do
#    cd speech_command_testset
    cd test_jcvoice
    echo "test $test Text read from file: $line"
    python3 ../convwav2txt.py $line

    echo "RT_L2_DATA short int  DataIn[]={" > wav.h
    sed "s/$/,/g" wav.txt >> wav.h
    echo "};" >> wav.h
    cp wav.h ../DataTest.h
    #mv wav.h "wav"_$test
    let test=test+1
    #rm  wav.txt
    cd ..
    IFS='/' read -r -a array <<< $line
    direct="reduced_test_set/${array[0]}"
    if [ ! -d $direct ]; then
	mkdir $direct
    fi
    #cp "speech_command_testset/$line" $direct
    if (( false )); then
#    if (( "$3" = "none" )); then
	echo "$3"
       present=0
       for item in  "yes" "no" "up" "down" "left" "right" "on" "off" "stop"  "go"
       do
	   #echo "item $item"
	   if [ ${item} == ${array[0]} ]; then
	       #echo "===>present $present $item ${array[0]}"
	       present=1
	   fi
       done
       #echo "$present"
       if (($present != 1)); then
	   array[0]="none"
       fi
    fi
    echo "${array[0]}"
	
    if (($2 <= $test))
    then
	echo "===> $2 $test"
	make clean all run > test_log
	read lab_found<<< $(grep "found max" test_log | awk '{print($3);}')
	echo "found word: $lab_found ${list_word[$lab_found]}"
	if [ ${list_word[$lab_found]} !=  ${array[0]} ]; then
	    let test_err=test_err+1
	fi
    fi
    echo "TEST COMPLETE err count: ${test_err}/${test}"
#    exit
done < "$1"
