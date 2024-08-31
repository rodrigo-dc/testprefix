#!/bin/bash

# Simple test script to make sure everything is, more or less, working.
# Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

on_exit() {
	if [ $? -eq 0 ]; then
		echo -e "\n\n.----------------------------------."
		echo        "|               :)                 |"
		echo        "|             SUCCESS!             |"
		echo -e     "'----------------------------------'\n\n"
	else
		echo -e "\n\n.----------------------------------."
		echo        "|               :(                 |"
		echo        "|              ERROR!              |"
		echo -e     "'----------------------------------'\n\n"
	fi
}
cd $(dirname $0)

trap "on_exit" EXIT

# List of test subdirs
SUBDIRS="trace_call global_setup_success global_setup_failure verbose failure_handler"

#
# Run clang-tidy
#

# 32-bit configuration
clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-m32 -std=gnu99
if [ $? -ne 0 ]; then
	exit 1
fi

# 64-bit configuration
clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-std=gnu99
if [ $? -ne 0 ]; then
	exit 1
fi

#
# Execute tests for each subdir
#
for s in $SUBDIRS; do
	BIN64=test_app_64
	BIN32=test_app_32
	rm $s/$BIN32
	rm $s/$BIN64
	rm $s/log.txt
	rm $s/log.tap
	VALGRIND_ERROR_CODE=100

	VALGRIND="valgrind -s --leak-check=full --track-origins=yes\
		--show-leak-kinds=all --error-exitcode=$VALGRIND_ERROR_CODE"

	# Build and run with valgrind (64 bits)
	make -f ../base.mk -C $s clean all BIN_NAME=$BIN64
	$VALGRIND ./$s/$BIN64
	if [ $? -eq $VALGRIND_ERROR_CODE ]; then
		exit 1
	fi

	# Build and run with valgrind (32 bits)
	CFLAGS=-m32 make -f ../base.mk -C $s clean all BIN_NAME=$BIN32
	$VALGRIND ./$s/$BIN32
	if [ $? -eq $VALGRIND_ERROR_CODE ]; then
		exit 1
	fi

	# Load SUBDIR_OPTIONS
	if [ -f $s/options.sh ]; then
		source $s/options.sh
	fi

	# Compare the console output against the reference file
	OUTPUT_FILE=log.txt

	./$s/$BIN32 $SUBDIR_OPTIONS &> $s/$OUTPUT_FILE
	# Remove test duration before comparing log files
	sed -ri "s/[0-9]+ ms//g" $s/$OUTPUT_FILE
	cmp $s/expected_$OUTPUT_FILE $s/$OUTPUT_FILE
	if [ $? -ne 0 ]; then
		exit 1
	fi

	# Compare the TAP output against the reference file (if any)
	TAP_FILE=log.tap
	./$s/$BIN64 $SUBDIR_OPTIONS -o $s/$TAP_FILE
	if [ -f $s/expected_$TAP_FILE ]; then
		cmp $s/expected_$TAP_FILE $s/$TAP_FILE
		if [ $? -ne 0 ]; then
			exit 1
		fi
	else
		if [ -f $s/$TAP_FILE ]; then
			echo "no TAP file was expected"
			exit 1
		fi
	fi
done

# macro_check is a subdirectory that contains a different type of test, see
# macro_check/test.sh.
if ! ./macro_check/test.sh; then
    echo "macro_check  failed!"
    exit 1
fi

