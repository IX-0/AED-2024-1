#!/bin/bash

function availableTests() {
    echo "Available tests:"
    echo "1 - imageBWTestAND worst cases"
    echo "2 - imageBWTestAND average cases"
    echo "3 - imageBWTestAND best cases"
}


if [[ $# != 1 ]]; then
    echo "Argument error: Wrong number of arguments"
    availableTests
    exit 1
fi

case $1 in
    1)
        testname="imageBWTestAND"
        testargs="1"
        ;;
    2)
        testname="imageBWTestAND"
        testargs="2"
        ;;
    3)
        testname="imageBWTestAND"
        testargs="3"
        ;;
    *)  
        echo "Test not found: '$1' isn't a valid test"
        availableTests
        exit 1
        ;;
esac

cd ..
make $testname || exit 1 > /dev/null
"./$testname" "$testargs" > test/out.txt
if [[ $? -ne 0 ]]; then 
    rm test/out.txt "$testname"
    exit 1
fi

cd test
matlab -batch main > /dev/null

exit 0
