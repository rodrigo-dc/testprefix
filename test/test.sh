#!/bin/bash -e
# Simple test script to make sure everything is, more or less, working.

cd $(dirname $0)

# 1 - Run clang-tidy on 32 and 64-bit configurations
clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-m32 -std=gnu99

clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-std=gnu99

# 2 - Execute test application
BIN64=test_app_64
BIN32=test_app_32

VALGRIND="valgrind -s --leak-check=full --track-origins=yes\
       	--show-leak-kinds=all --error-exitcode=1"

PREFIX=test_suite1_with_setup

# 2.1 - for 64 bits
rm *.txt *.tap || true
make clean all BIN_NAME=$BIN64
$VALGRIND ./$BIN64 > ${BIN64}.txt
$VALGRIND ./$BIN64 -p $PREFIX > ${BIN64}_prefix.txt
$VALGRIND ./$BIN64 -o ${BIN64}.tap
# Remove time information
sed -ri "s/[0-9]+ ms//g" ${BIN64}.txt
sed -ri "s/[0-9]+ ms//g" ${BIN64}_prefix.txt
# Check the logs
cmp expected-logs/test_app.txt ${BIN64}.txt
cmp expected-logs/test_app_prefix.txt ${BIN64}_prefix.txt

# 2.2 - for 32 bits
make clean all CFLAGS=-m32 BIN_NAME=$BIN32
./$BIN32 > ${BIN32}.txt
./$BIN32 -p $PREFIX > ${BIN32}_prefix.txt
./$BIN32 -o ${BIN32}.tap
# Remove time information
sed -ri "s/[0-9]+ ms//g" ${BIN32}.txt
sed -ri "s/[0-9]+ ms//g" ${BIN32}_prefix.txt
# Check the logs
cmp expected-logs/test_app.txt ${BIN32}.txt
cmp expected-logs/test_app_prefix.txt ${BIN32}_prefix.txt

echo -e "\n\n.----------------------------------."
echo        "|                                  |"
echo        "|               :)                 |"
echo        "|                                  |"
echo        "|  APPARENTLY, NOTHING IS BROKEN!  |"
echo        "|                                  |"
echo -e     "'----------------------------------'\n\n"
