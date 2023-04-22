#!/bin/bash
# Simple test script to make sure everything is, more or less, working.

on_exit() {
	if [ $? -eq 0 ]; then
		echo -e "\n\n.----------------------------------."
		echo        "|                                  |"
		echo        "|               :)                 |"
		echo        "|                                  |"
		echo        "|             SUCCESS!             |"
		echo        "|                                  |"
		echo -e     "'----------------------------------'\n\n"
	else
		echo -e "\n\n.----------------------------------."
		echo        "|                                  |"
		echo        "|               :(                 |"
		echo        "|                                  |"
		echo        "|              ERROR!              |"
		echo        "|                                  |"
		echo -e     "'----------------------------------'\n\n"
	fi
}
cd $(dirname $0)

trap "on_exit" EXIT

# 1 - Run clang-tidy on 32 and 64-bit configurations
clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-m32 -std=gnu99

clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-std=gnu99

# 2 - Execute test application with valgrind
BIN64=test_app_64
BIN32=test_app_32
VALGRIND_ERROR_CODE=100

VALGRIND="valgrind -s --leak-check=full --track-origins=yes\
       	--show-leak-kinds=all --error-exitcode=$VALGRIND_ERROR_CODE"

# 2.1 - for 64 bits
make clean all BIN_NAME=$BIN64
$VALGRIND ./$BIN64
if [ $? -eq $VALGRIND_ERROR_CODE ]; then
	exit 1
fi

# 2.2 - for 32 bits
make clean all CFLAGS=-m32 BIN_NAME=$BIN32
$VALGRIND ./$BIN32
if [ $? -eq $VALGRIND_ERROR_CODE ]; then
	exit 1
fi

# 3 - Check error codes
./$BIN64 -p test_suite1_ok
if [ $? -ne 0 ]; then
	exit 1
fi

./$BIN64 -p test_suite1_nok
if [ $? -eq 0 ]; then
	exit 1
fi

./$BIN64 -p test_suite1_skip
if [ $? -ne 0 ]; then
	exit 1
fi

# 4 - Check outputs
OUTPUT_FILE=log.txt
TAP_FILE=log.tap

./$BIN32 -p test_ > $OUTPUT_FILE
sed -ri "s/[0-9]+ ms//g" $OUTPUT_FILE
cmp expected-logs/$OUTPUT_FILE $OUTPUT_FILE
if [ $? -ne 0 ]; then
	exit 1
fi

./$BIN32 -p test_ -o $TAP_FILE
cmp expected-logs/$TAP_FILE $TAP_FILE
if [ $? -ne 0 ]; then
	exit 1
fi
