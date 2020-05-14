#!/bin/bash
declare -a list_word=( "dummy" "silence" "none" "yes" "no" "up" "down" "left" "right" "on" "off" "stop"  "go")   

rm -r test_log   
#make clean all KWS_BITS=16 
echo "skip $2 first tests"
test=0
test_err=0

while read line
do
    IFS=' ' read -r -a array <<< $line
    let test=test+1
    cp "${array[9]}" ./in_feat.txt
    IFS='_' read -r -a fields <<< ${array[9]}
    IFS='.' read -r -a expected <<< ${fields[3]}
    echo " expected ===> ${expected[0]}"
	
    if (($2 <= $test))
    then
#	echo "===> $2 $test"
	touch kws.c
	make  all run KWS_BITS=16 platform=gvsoc > test_log
	read lab_found<<< $(grep "Recognized:" test_log | awk '{print($2);}')
	echo "found : $lab_found (${list_word[$lab_found]})"
	if [ $lab_found !=  ${expected[0]} ]; then
	    let test_err=test_err+1
	fi
    fi
    echo "TEST COMPLETE err count: ${test_err}/${test}"
done < <(ls -ls "$1"/*.txt)
