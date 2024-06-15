// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

// Basic tests for boolean macros

#include "testprefix.h"
#include "one_param_tests.h"

#include <stdbool.h>

#define ERROR_VALUE true
ONE_PARAM_MACRO_TEST_SET(ASSERT_FALSE, false, true)
ONE_PARAM_MACRO_TEST_SET(EXPECT_FALSE, false, true)
#undef ERROR_VALUE

#define ERROR_VALUE false
ONE_PARAM_MACRO_TEST_SET(ASSERT_TRUE, true, false)
ONE_PARAM_MACRO_TEST_SET(EXPECT_TRUE, true, false)
#undef ERROR_VALUE
