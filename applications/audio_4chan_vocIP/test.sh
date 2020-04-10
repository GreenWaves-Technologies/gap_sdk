#!/bin/bash
declare -a list_word=( "dummy" "silence" "none" "yes" "no" "up" "down" "left" "right" "on" "off" "stop"  "go")   

#usage
# source test.sh ./reduced_test_set/testing_list_small_small.txt 0


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
    cd reduced_test_set
    echo "test $test Text read from file: $line"
    python3 ../convwav2txt.py $line

    echo "GAP_L2_DATA short int  DataIn[]={" > wav.h
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
	touch main.c
	make  all run platform=gvsoc > test_log
	read lab_found<<< $(grep "found max" test_log | awk '{print($3);}')
	echo "found word: $lab_found ${list_word[$lab_found]}"
	if [ ${list_word[$lab_found]} !=  ${array[0]} ]; then
	    let test_err=test_err+1
	fi
    fi
    echo "TEST COMPLETE err count: ${test_err}/${test}"
#    exit
done < "$1"
