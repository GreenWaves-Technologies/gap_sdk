#!/bin/bash

# PARAMETERS TIME
# TIME check interval
TIME_INTERVAL=10
# Expiration TIME variable
EXPIRE_TIME=0

if [ -z "$TARGET_CHIP"  ]; then
    TARGET_CHIP=GAP8
fi

# VARIABLEs
TOTAL_TEST=0
TEST_COMPILE=0
TEST_SUCCESS=0
TEST_FAILED=0

# Report PATH
REPORT_FILE_PATH="$(pwd)/report.log"

check_previous_test() {
    # Pass previous jenkins test

    if [ -f  $REPORT_FILE_PATH ]; then
            if [ "$1" == "run" ]; then
                PREVIOUS_TEST=$(grep -c RUN $REPORT_FILE_PATH)
            elif [ "$1" == "compile" ]; then
                PREVIOUS_TEST=$(grep -c COMPILE $REPORT_FILE_PATH)
            elif [ "$1" == "compile_and_run" ]; then
                PREVIOUS_TEST=$(grep -c RUN $REPORT_FILE_PATH)
            fi
            else
        PREVIOUS_TEST=""
    fi

    TOTAL_TEST=$((TOTAL_TEST+1))

    if [ "$PREVIOUS_TEST" != "" ]; then
        if [ $TOTAL_TEST -le $PREVIOUS_TEST ]; then
            if [ "$1" == "list" ]; then
                echo "$(dirname $t) : PASS";
            fi
            continue;
        fi
    fi
}

check_compile() {

    if [ $1 -eq 0 ]; then
        echo "$(dirname $t) : COMPILE SUCCESS" >> $REPORT_FILE_PATH

        # Record compile success path
        TEST_COMPILE=$((TEST_COMPILE+1));
    else
        # Record compile failed path
        echo "$(dirname $t) : COMPILE FAILED" >> $REPORT_FILE_PATH
    fi
}

check_run() {

    while true; do
        sleep $TIME_INTERVAL;

        # grep -s --no-messages
        if grep -cs "Test success"  $1/BUILD/$TARGET_CHIP/GCC_RISCV/transcript > /dev/null;
        then
            # Test success
            TEST_SUCCESS=$((TEST_SUCCESS+1))
            # Record run success path
            echo "$1 : RUN SUCCESS" >> $REPORT_FILE_PATH

            echo "Test success..."
            break;

        elif grep -cs "Test failed"  $1/BUILD/$TARGET_CHIP/GCC_RISCV/transcript  > /dev/null;
        then
            # Test failed
            TEST_FAILED=$((TEST_FAILED+1))
            # Record run failed path
            echo "$1 : RUN FAILED" >> $REPORT_FILE_PATH

            echo "$1 Test failed..."
            break;

        else
            # Accumulate expire time, add TIME_INTERVAL seconds every time
            EXPIRE_TIME=$((EXPIRE_TIME+$TIME_INTERVAL))

            if [ $EXPIRE_TIME -gt $TIMEOUT ]; then
                # Test failed
                TEST_FAILED=$((TEST_FAILED+1))
                # Record run failed path
                echo "$1 : RUN FAILED" >> $REPORT_FILE_PATH

                echo "$1 Test failed..."
                break;
            else
                echo "Waiting test run finished, time = $EXPIRE_TIME s..."
            fi
        fi
    done

    kill $(ps aux | grep -s 'vsimk' | awk '{print $2}')     2>/dev/null
    kill $(ps aux | grep -s 'plpbridge' | awk '{print $2}') 2>/dev/null
}


for t in $(find -L "$PWD" -type d -name "BUILD" -prune -o -type f -name testset.ini -print);
do
    if [ $1 == "clean" ]; then
        cd  $(dirname $t) && make clean;
    elif [ $2 == "rtl" ]; then
        if grep -cs "rtl" $t > /dev/null;
        then
            # Check previous unfinished jenkins test
            check_previous_test $1;

            # Reset expire time for each test
            EXPIRE_TIME=0

            cd  $(dirname $t)

            # Get timeout for each test
            TIMEOUT=$(grep -r timeout | grep -o -E '[0-9]+')

            # Compilation check or run check
            if [ $1 == "compile" ]; then
                # Do compilation, then check the result of compilation
                make clean all platform=rtl;

                # Test the return value - 0 is success, others are failed
                check_compile $? ;

            elif [ $1 == "run" ]; then
                # Do compilation, then check the result of compilation
                make all platform=rtl;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

                # Do run, then check the result of run
                make run platform=rtl& check_run $(dirname $t);

            elif [ $1 == "compile_and_run" ]; then
                # Do compilation and run, then check the result of compilation and check the result of run
                make clean all platform=rtl;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

                # Check the result of run
                make run platform=rtl& check_run $(dirname $t);

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
            # Check previous unfinished jenkins test
            check_previous_test $1;

            # Reset expire time for each test
            EXPIRE_TIME=0

            cd  $(dirname $t)

            # Get timeout for each test
            TIMEOUT=60

            # Compilation check or run check
            if [ $1 == "compile" ]; then
                # Do compilation, then check the result of compilation
                make clean all platform=gvsoc;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

            elif [ $1 == "run" ]; then
                # Do compilation, then check the result of compilation
                make all platform=gvsoc;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

                # Do run, then check the result of run
                make run platform=gvsoc| tee ./BUILD/$TARGET_CHIP/GCC_RISCV/transcript & check_run $(dirname $t);

            elif [ $1 == "compile_and_run" ]; then
                # Do compilation and run, then check the result of compilation and check the result of run
                make clean all platform=gvsoc;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

                # Check the result of run
                (make run platform=gvsoc| tee ./BUILD/$TARGET_CHIP/GCC_RISCV/transcript) & check_run $(dirname $t);

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
            # Check previous unfinished jenkins test
            check_previous_test $1;

            # Reset expire time for each test
            EXPIRE_TIME=0

            cd  $(dirname $t)

            # Get timeout for each test
            TIMEOUT=60

            # Compilation check or run check
            if [ $1 == "compile" ]; then
                # Do compilation, then check the result of compilation
                gapuino=true make clean all;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

            elif [ $1 == "run" ]; then
                # Do compilation, then check the result of compilation
                gapuino=true make all;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

                # Do run, then check the result of run
                gapuino=true make run | tee ./BUILD/$TARGET_CHIP/GCC_RISCV/transcript & check_run $(dirname $t);

            elif [ $1 == "compile_and_run" ]; then
                # Do compilation and run, then check the result of compilation and check the result of run
                gapuino=true make clean all;

                # Test the return value - 0 is success, others are failed
                check_compile $?;

                # Check the result of run
                (gapuino=true make run | tee ./BUILD/$TARGET_CHIP/GCC_RISCV/transcript) & check_run $(dirname $t);

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

if [ -f  $REPORT_FILE_PATH ]; then
    TEST_COMPILE=$(grep -c "COMPILE SUCCESS" $REPORT_FILE_PATH)
    TEST_SUCCESS=$(grep -c "RUN SUCCESS" $REPORT_FILE_PATH)
    TEST_FAILED=$(grep -c "RUN FAILED" $REPORT_FILE_PATH)
fi

echo "############################################"         | tee -a $REPORT_FILE_PATH
echo "Test total           number : $TOTAL_TEST"            | tee -a $REPORT_FILE_PATH
echo "Test compile success number : $TEST_COMPILE"          | tee -a $REPORT_FILE_PATH
echo "Test success         number : $TEST_SUCCESS"          | tee -a $REPORT_FILE_PATH
echo "Test failed          number : $TEST_FAILED"           | tee -a $REPORT_FILE_PATH
echo "############################################"         | tee -a $REPORT_FILE_PATH
