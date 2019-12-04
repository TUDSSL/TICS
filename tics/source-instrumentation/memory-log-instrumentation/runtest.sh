#!/bin/bash

# static configuration options
TEST_DIR=test
GEN_DIR=test/gen

# dynamic configuration options
print_tranformation_output=true

# test cases format 'test_case_name'.c (w/o extension)
# will generate GEN_'test_case_name'.c
# will compare to 'test_case_name'-instr.c
test_cases=(
    global_struct
    struct_ref
    array
    prepostincr
    compound_assign
    activity
    cuckoo
)

function usage {
    echo "Usage: $0 [-h] [-q] [-t \"TEST_NAME1 [TEST_NAME2]\"]"
    echo "  -q              do not output transformation details"
    echo "  -t              tests to run, input multiple between \"\" as a string"
    echo "  -h              print this help message"
}

# argument parsing
while getopts ":hqt:" opt; do
    case ${opt} in
        h )
            usage
            exit 1
            ;;
        q)
            print_tranformation_output=false
            ;;
        t)
            test_cases=(${OPTARG})
            ;;
        \? )
            usage
            exit 1
            ;;
        :)
            echo "Error: -${OPTARG} requires an argument."
            usage
            exit 1
    esac
done

# build tool
echo -e "\e[33mbuilding tool\e[39m"
pushd build
make
if [ $? -ne 0 ]; then
    echo "build failed."
    exit 1
fi
popd

# create output directory
mkdir -p $GEN_DIR

# command to instrument the code
toolcmd="build/memlog"

# command to diff the result with the compare
dcmd="colordiff -y --suppress-common-lines"

echo ""
for tcase in "${test_cases[@]}"; do
    echo -e "\e[4m\e[33mtest case:\e[39m\e[24m $tcase"

    convfile="$tcase.c"
    cmpfile="$tcase-instr.c"
    genfile="GEN_$tcase.c"

    # check if the required files exist"
    if [ ! -f "$TEST_DIR/$convfile" ]; then
        echo -e "\e[31mconvert file: $convfile does not exist\e[39m"
        echo "skipping test case"
        echo ""
        continue
    fi
    if [ ! -f "$TEST_DIR/$cmpfile" ]; then
        echo -e "\e[31mcompare file: $cmpfile does not exist\e[39m"
        echo "skipping test case"
        echo ""
        continue
    fi

    # run tool
    full_toolcmd="$toolcmd $TEST_DIR/$convfile --"
    if [ "$print_tranformation_output" = true ] ; then
        echo ""
        echo -e "\e[33mtransfromation log\e[39m $convfile > $genfile"
        $full_toolcmd > $GEN_DIR/$genfile
        echo ""
    else
        # discard transfromation information
        $full_toolcmd > $GEN_DIR/$genfile 2>/dev/null
    fi

    # compare
    echo -e "\e[33mcomparing\e[39m $cmpfile and $genfile"
    $dcmd $TEST_DIR/$cmpfile $GEN_DIR/$genfile

    # Check if the files are equal (test succeeded)
    test_pf=$?
    if [ $test_pf -eq 0 ]; then
        echo -e "\e[32mpassed\e[39m"
    else
        echo -e "\e[31mfailed\e[39m"
    fi
    echo ""

done

echo -e "\e[33mdone\e[39m"
