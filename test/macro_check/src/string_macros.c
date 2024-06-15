// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

// Basic tests for string macros

#include "testprefix.h"
#include "two_param_tests.h"
#include "string_tests.h"

// Standard tests
#define ERROR_VALUE "not equal"
TWO_PARAM_MACRO_TEST_SET(ASSERT_STR_EQ, "equal", "equal", "not equal", char *)
TWO_PARAM_MACRO_TEST_SET(EXPECT_STR_EQ, "equal", "equal", "not equal", char *)
#undef ERROR_VALUE

#define ERROR_VALUE "equal"
TWO_PARAM_MACRO_TEST_SET(ASSERT_STR_NE, "equal", "not equal", "equal", char *)
TWO_PARAM_MACRO_TEST_SET(EXPECT_STR_NE, "equal", "not equal", "equal", char *)
#undef ERROR_VALUE

STRING_MACRO_TEST_SET_EQ("", empty)
STRING_MACRO_TEST_SET_EQ(" ", space)
STRING_MACRO_TEST_SET_EQ("\t", tab)
STRING_MACRO_TEST_SET_EQ("\n", newline)
STRING_MACRO_TEST_SET_NE("abcdefghijkl", "abcdefghijkk", last_char)
STRING_MACRO_TEST_SET_NE("zbcdefghijkl", "abcdefghijkl", first_char)
STRING_MACRO_TEST_SET_NE("abc", "abcd", common_prefix)
STRING_MACRO_TEST_SET_NE("", "abcd", one_empty)
