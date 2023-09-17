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

SUBDIRS="basic global_setup_success global_setup_failure"
# 1 - Run clang-tidy on 32 and 64-bit configurations
clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-m32 -std=gnu99
if [ $? -ne 0 ]; then
	exit 1
fi

clang-tidy --config-file=../.clang-tidy ../testprefix.c ../testprefix.h --\
       	-std=gnu99
if [ $? -ne 0 ]; then
	exit 1
fi

# 2 - Execute test application with valgrind
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

	# 2.1 - for 64 bits
	make -f ../base.mk -C $s clean all BIN_NAME=$BIN64
	$VALGRIND ./$s/$BIN64
	if [ $? -eq $VALGRIND_ERROR_CODE ]; then
		exit 1
	fi

	# 2.2 - for 32 bits
	make -f ../base.mk -C $s clean all CFLAGS=-m32 BIN_NAME=$BIN32
	$VALGRIND ./$s/$BIN32
	if [ $? -eq $VALGRIND_ERROR_CODE ]; then
		exit 1
	fi

	# 3 - subdir specific test
	if [ -f $s/subtest.sh ]; then
		source $s/subtest.sh
	fi

	# 4 - Check outputs
	OUTPUT_FILE=log.txt
	TAP_FILE=log.tap

	./$s/$BIN32 -p test_ > $s/$OUTPUT_FILE
	# Remove test duration before comparing log files
	sed -ri "s/[0-9]+ ms//g" $s/$OUTPUT_FILE
	cmp $s/expected_$OUTPUT_FILE $s/$OUTPUT_FILE
	if [ $? -ne 0 ]; then
		exit 1
	fi

	./$s/$BIN64 -p test_ -o $s/$TAP_FILE
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
