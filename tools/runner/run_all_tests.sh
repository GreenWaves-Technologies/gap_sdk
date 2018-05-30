#!/bin/bash

# PARAMETERS TIME
TIME_INTERVAL=10


if [ "$2" == "rtl" ]; then
    EXPIRE_LIMIT=500
elif [ "$2" == "gvsoc" ]; then
    EXPIRE_LIMIT=60
elif [ "$2" == "gapuino" ]; then
    EXPIRE_LIMIT=100
elif [ "$1" == "clean" ]; then
    echo "Clean the tests!"
else
    echo "Please provide platform!"
    exit 1
fi

# VARIABLEs
TATAL_TEST=0
TEST_COMPILE=0
TEST_SUCCESS=0
TEST_FAILED=0

EXPIRE_TIME=0

# PATH
TEST_COMPILE_FAILED_PATH=""
TEST_RUN_FAILED_PATH=""

REPORT_FILE_PATH="$(pwd)/report.log"

check_compile() {
    # If only test compile, no need to wait.
    if [ -z "$1" ]; then
        TEST_COMPILE=$((TEST_COMPILE+1));
    else
        # If use run command, then we need to wait the compilation.
        # The sleep time should be > the max of compilaton time
        sleep 30;
        find_result=$(find $1 -name test);

        if [ find_result != "" ]; then
            TEST_COMPILE=$((TEST_COMPILE+1));
        fi
    fi
}

check_run() {

    while true; do
        sleep $TIME_INTERVAL;

        # grep -s --no-messages
        if grep -cs "Test success"  $1/BUILD/GAP8/GCC_RISCV/transcript > /dev/null;
        then
            # Test success
            TEST_SUCCESS=$((TEST_SUCCESS+1))
            echo "Test success..."
            break;

        elif grep -cs "Test failed"  $1/BUILD/GAP8/GCC_RISCV/transcript  > /dev/null;
        then
            # Test failed
            TEST_FAILED=$((TEST_FAILED+1))
            # Record run failed path
            TEST_RUN_FAILED_PATH="$TEST_RUN_FAILED_PATH $1";
            echo "$1 Test failed..."
            break;

        else
            # Accumulate expire time, add TIME_INTERVAL seconds every time
            EXPIRE_TIME=$((EXPIRE_TIME+$TIME_INTERVAL))

            if [ $EXPIRE_TIME -gt $EXPIRE_LIMIT ]; then
                # Test failed
                TEST_FAILED=$((TEST_FAILED+1))
                # Record run failed path
                TEST_RUN_FAILED_PATH="$TEST_RUN_FAILED_PATH $1";
                echo "$1 Test failed..."
                break;
            else
                echo "Waiting test run finished, time = $EXPIRE_TIME s..."
            fi
        fi
    done

    kill $(ps aux | grep -s 'vsimk' | awk '{print $2}')
    kill $(ps aux | grep -s 'plpbridge' | awk '{print $2}')
}


for t in $(find "$(pwd)" -name testset.ini);
do
    if [ $1 == "clean" ]; then
        cd  $(dirname $t) && make clean;
    elif [ $2 == "rtl" ]; then
        if grep -cs "rtl" $t > /dev/null;
        then
            TATAL_TEST=$((TATAL_TEST+1))

            # Reset expire time for each test
            EXPIRE_TIME=0

            # Compilation check or run check
            if [ $1 == "compile" ]; then
                # Do compilation, then check the result of compilation
                cd  $(dirname $t) && make clean all;

                # Test the return value - 0 is success, others are failed
                if [ $? -eq 0 ]; then
                    check_compile;
                else
                    TEST_COMPILE_FAILED_PATH="$TEST_COMPILE_FAILED_PATH $(dirname $t)";
                fi

            elif [ $1 == "run" ]; then
                # Do run, then check the result of run
                cd  $(dirname $t) && make run & check_run $(dirname $t);

            elif [ $1 == "compile_and_run" ]; then
                # Do compilation and run, then check the result of compilation and check the result of run
                cd  $(dirname $t) && make clean all run & check_compile $(dirname $t) & check_run $(dirname $t);

            elif [ $1 == "list" ]; then
                # List all the test, according the testset.ini
                # echo "Finding test : checking whether there is a <testset.ini> file";
                echo $(dirname $t);

            else
                echo "Nothing to do, see usage !!!"
            fi
        fi
    elif [ $2 == "gvsoc" ]; then
        if grep -cs "gvsoc" $t > /dev/null;
        then
            TATAL_TEST=$((TATAL_TEST+1))

            # Reset expire time for each test
            EXPIRE_TIME=0

            # Compilation check or run check
            if [ $1 == "compile" ]; then
                # Do compilation, then check the result of compilation
                cd  $(dirname $t) && gvsoc=true make clean all;

                # Test the return value - 0 is success, others are failed
                if [ $? -eq 0 ]; then
                    check_compile;
                else
                    TEST_COMPILE_FAILED_PATH="$TEST_COMPILE_FAILED_PATH $(dirname $t)";
                fi

            elif [ $1 == "run" ]; then
                # Do run, then check the result of run
                cd  $(dirname $t) && gvsoc=true make run | tee ./BUILD/GAP8/GCC_RISCV/transcript & check_run $(dirname $t);

            elif [ $1 == "compile_and_run" ]; then
                # Do compilation and run, then check the result of compilation and check the result of run
                cd  $(dirname $t) && gvsoc=true make clean all && gvsoc=true make run | tee ./BUILD/GAP8/GCC_RISCV/transcript & check_compile $(dirname $t) & check_run $(dirname $t);

            elif [ $1 == "list" ]; then
                # List all the test, according the testset.ini
                # echo "Finding test : checking whether there is a <testset.ini> file";
                echo $(dirname $t);

            else
                echo "Nothing to do, see usage !!!"
            fi
        fi
    elif [ $2 == "gapuino" ]; then
        if grep -cs "gapuino" $t > /dev/null;
        then
            TATAL_TEST=$((TATAL_TEST+1))

            # Reset expire time for each test
            EXPIRE_TIME=0

            # Compilation check or run check
            if [ $1 == "compile" ]; then
                # Do compilation, then check the result of compilation
                cd  $(dirname $t) && gapuino=true make clean all;

                # Test the return value - 0 is success, others are failed
                if [ $? -eq 0 ]; then
                    check_compile;
                else
                    TEST_COMPILE_FAILED_PATH="$TEST_COMPILE_FAILED_PATH $(dirname $t)";
                fi

            elif [ $1 == "run" ]; then
                # Do run, then check the result of run
                cd  $(dirname $t) && make run  | tee ./BUILD/GAP8/GCC_RISCV/transcript & check_run $(dirname $t);

            elif [ $1 == "compile_and_run" ]; then
                # Do compilation and run, then check the result of compilation and check the result of run
                cd  $(dirname $t) && make clean all && (make run | tee ./BUILD/GAP8/GCC_RISCV/transcript) & check_compile $(dirname $t) & check_run $(dirname $t);

            elif [ $1 == "list" ]; then
                # List all the test, according the testset.ini
                # echo "Finding test : checking whether there is a <testset.ini> file";
                echo $(dirname $t);

            else
                echo "Nothing to do, see usage !!!"
            fi
        fi
    fi

done

echo "############################################"         | tee -a $REPORT_FILE_PATH
echo "Test total           number : $TATAL_TEST"            | tee -a $REPORT_FILE_PATH
echo "Test copmile success number : $TEST_COMPILE"          | tee -a $REPORT_FILE_PATH
echo "Test success         number : $TEST_SUCCESS"          | tee -a $REPORT_FILE_PATH
echo "Test failed          number : $TEST_FAILED"           | tee -a $REPORT_FILE_PATH
echo "############################################"         | tee -a $REPORT_FILE_PATH
echo "Test copmile failed path : $TEST_COMPILE_FAILED_PATH" | tee -a $REPORT_FILE_PATH
echo "Test run failed path : $TEST_RUN_FAILED_PATH"         | tee -a $REPORT_FILE_PATH
