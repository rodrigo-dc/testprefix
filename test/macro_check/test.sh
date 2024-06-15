#!/bin/bash -e

# Build, run and analyse the output of macro_check, a test application dedicated
# to check testprefix's assertion macros.
#
# Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

# This script
# -----------
# This script makes sure the macros are working by analysing the output of a
# test application. It looks for specific patterns and checks for consistency.
#
# The test application (macro_check)
# ----------------------------------
# The test application, which is called macro_check, contains several test
# functions that invoke all testprefix's ASSERT_*, EXPECT_* and SKIP macros.
# As we are testing testprefix itself, most of these functions fail. The
# challenge is to know if they are failing in an expected way.

# Uncomment here to debug
# set -x
cd $(dirname $0)

VALGRIND="valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --error-exitcode=88"

error_count_from_test_name()
{
    local test_name="$1"
    sed -r "s/test_err([0-9]+)_.+/\1/g" <<< "$test_name"
}

error_count_from_test_output()
{
    local test_output="$1"
    grep -Ec "^\|      \|   .+\.c:[0-9]+: .+$" <<< "$test_output" || true
}

message_count_from_test_name()
{
    local test_name="$1"
    sed -r "s/test_err[0-9]+_msg([0-9]+)_.+/\1/g" <<< "$test_name"
}

count_occurrences()
{
    local test_output="$1"
    local pattern="$2"
    grep -Ec "$pattern" <<< "$test_output" || true
}

macro_type_from_test_name()
{
    local test_name="$1"
    sed -r "s/test_err[0-9]+_msg[0-9]+_([A-Z]+)_.+/\1/g" <<< "$test_name"
}

total_field_value()
{
    local test_output="$1"
    local field_name="$2"
    sed -rn "s/^.+${field_name}: ([0-9]+)$/\1/gp" <<< "$test_output"
}

test_status_from_output()
{
    local test_output="$1"
    sed -rn "s/^\| (FAIL|PASS|SKIP) \|.+$/\1/pg" <<< "$test_output"
}

# Step 1 - Individual test execution
# ----------------------------------
# The expected behavior for each function is extracted from its name.
# For example, a function named 'test_err1_msg0_EXPECT_UINT_NE_something', will
# result in the following expectations:
#
#     Number of errors: 1
#     Number of messages: 0
#     Macro type: EXPECT
# 
# This is the general function name format expected:
#
#     test_err<X>_msg<Y>_<MACRO_TYPE>_[ANYTHING]
#
#          <X> - number of errors: errors are printed to the console output
#                when an assertion fails. For example:
#
#                      "boolean_macros.c:15: 0 was expected to be TRUE"
#
#          <Y> - number of messages: these are custom messages printed when an
#                assertion fails or when a reason is given to the SKIP macro.
#                This script expects all messages to be the same, containing
#                only the string "##expected message##"
#
# <MACRO_TYPE> - this indicates which type of macro is being tested. There
#                are three possible values: ASSERT, EXPECT and SKIP.
#
# NOTE: the name of one function cannot be another function's prefix. To run
#       tests individually, we use testprefix's -p option. This option was
#       created to allow specifying the test prefix at runtime, not to run
#       individual tests as we do here. For this reason, if we call the test app
#       with '-p test_a', another test function, called 'test_ab', would be
#       executed as well.
run_individual_tests()
{
    # Build a list of tests
    local test_list=$(./macro_check -l|sed -r "s/^ +\- //g"|grep test_)

    # Iterate over the tests, executing them one by one
    for test_name in $test_list; do
        echo -n "."
        # Extract information from the test name
        local expected_error_count=$(error_count_from_test_name $test_name)
        local expected_message_count=$(message_count_from_test_name $test_name)
        local macro_type=$(macro_type_from_test_name $test_name)

        # Avoid aborting the script because of the exit code of test_app
        set +e
        # Need to declare the variable separately. Declaring local and assigning
        # at the same time, makes it impossible to get the exit code from test_app.
        local test_app_output
        test_app_output=$(./macro_check -p $test_name)
        local test_exit_code=$?
        set -e

        # Extract information from the test output
        local actual_error_count=$(error_count_from_test_output "$test_app_output")
        local actual_message_count=$(count_occurrences "$test_app_output" "##expected message##")
        local total_passed=$(total_field_value "$test_app_output" "Passed")
        local total_failed=$(total_field_value "$test_app_output" "Failed")
        local total_skipped=$(total_field_value "$test_app_output" "Skipped")
        local test_status=$(test_status_from_output "$test_app_output")
        local unexpected_message_count=$(count_occurrences "$test_app_output" "##unexpected message##")

        # As ASSERT macros abort the test instantly, the number of expected errors
        # and expected messages a limited to 1.
        if [[ $macro_type = "ASSERT" ]] && [[ $expected_error_count -gt 0 ]]; then
            expected_error_count=1
        fi
        if [[ $macro_type = "ASSERT" ]] && [[ $expected_message_count -gt 0 ]]; then
            expected_message_count=1
        fi

        # Check expected and actual number of errors in the test.
        # Multiple errors are only possible for EXPECT macros because ASSERT macros
        # abort the test when the first error occurs.
        test $expected_error_count -eq $actual_error_count

        # Check the expected and actual number of messages printed. The messages that
        # are expected to be printed have the same content: ##expected message##
        test $expected_message_count -eq $actual_message_count 

        # Check the consistency of the summary fields and the test application
        # exit status.
        case $macro_type in
        ASSERT | EXPECT)
            if [[ $expected_error_count -eq 0 ]]; then
                test $test_status = "PASS"
                test $total_passed -eq 1
                test $total_failed -eq 0
                test $total_skipped -eq 0
                test $test_exit_code -eq 0
            else
                test $test_status = "FAIL"
                test $total_passed -eq 0
                test $total_failed -eq 1
                test $total_skipped -eq 0
                test $test_exit_code -eq 255
            fi
            ;;
        SKIP)
            test $test_status = "SKIP"
            test $total_skipped -eq 1
            test $total_passed -eq 0
            test $total_failed -eq 0
            test $test_exit_code -eq 0
            ;;
        esac

        # Many ASSERT and EXPECT macro invocations, that are expected to pass,
        # contain the message "##unexpected message##. If this message appears in
        # the output, something is wrong.
        test $unexpected_message_count -eq 0
    done
    echo
}

# Step 2 - All tests execution
# ----------------------------
# Here, all tests functions are executed at once. The overall behavior of the
# test application is analysed. This script counts the number of failures,
# skips, and successes based on the console output. These numbers are compared
# to those presented in the summary, at the end of the console output.
#
# tapview [1] is used to perform a basic check on the generated TAP [2] report.
#
# [1] https://gitlab.com/esr/tapview
# [2] https://testanything.org/
run_all_tests()
{
    # Execute all tests
    # We don't want to abort the script because of the exit code of test_app
    set +e
    # Need to declare the variable separately. Declaring local and assigning
    # at the same time, makes it impossible to get the exit code from test_app.
    local test_app_output
    rm report.tap
    test_app_output=$($VALGRIND ./macro_check -o report.tap)
    local test_exit_code=$?
    set -e

    # Extract information from the test output
    local total_passed=$(total_field_value "$test_app_output" "Passed")
    local total_failed=$(total_field_value "$test_app_output" "Failed")
    local total_skipped=$(total_field_value "$test_app_output" "Skipped")
    local total_test=$(total_field_value "$test_app_output" "Total")
    local unexpected_message_count=$(count_occurrences "$test_app_output" "##unexpected message##")
    local pass_count=$(count_occurrences "$test_app_output" "\| PASS \|")
    local fail_count=$(count_occurrences "$test_app_output" "\| FAIL \|")
    local skip_count=$(count_occurrences "$test_app_output" "\| SKIP \|")
    local test_count=$(count_occurrences "$test_app_output" "\| TEST \|")

    # Some assertions, that are expected to pass, contain a specific message.
    # If they fail, they will print "##unexpected message##".
    test $unexpected_message_count -eq 0

    # Check if the summary fields match the test output
    test $total_test -eq $(($total_passed + $total_failed + $total_skipped))
    test $total_passed -eq $pass_count
    test $total_failed -eq $fail_count
    test $total_skipped -eq $skip_count
    test $total_test -eq $test_count

    # Basic check of the TAP report
    local tapview_lastline=$(./tapview<report.tap|tail -n 1)
    local tap_test_count=$(echo $tapview_lastline|sed -r "s/^([0-9]+) tests.+$/\1/g")
    local tap_failure_count=$(echo $tapview_lastline|sed -r "s/^[0-9]+ tests, ([0-9]+) failures.+$/\1/g")
    local tap_skip_count=$(echo $tapview_lastline|sed -r "s/^[0-9]+ tests, [0-9]+ failures, ([0-9]+) SKIPs.+$/\1/g")

    # If tapview could understand the TAP report, these values must match
    test $total_test -eq $tap_test_count
    test $total_failed -eq $tap_failure_count
    test $total_skipped -eq $tap_skip_count

}

# Exit the script if any command fails.
# This way we can use the command 'test' to check conditions and exit automatically
# when the check fails.
set -e

echo -e "\n### Building test application ###\n"
make clean && make

echo -e "\n### Running tests one by one. ####\n"
run_individual_tests

echo -e "\n### Running all tests. ###\n"
run_all_tests

