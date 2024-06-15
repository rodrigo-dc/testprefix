// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"
#include "mem_tests.h"

uint8_t array_1234[] = {1, 2, 3, 4};
uint8_t array_2234[] = {2, 2, 3, 4};
uint8_t array_1233[] = {1, 2, 3, 3};
uint8_t array_1234567[] = {1, 2, 3, 4, 5, 6, 7};
uint8_t array_longer[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
                          14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};

// For the test set, that will use MIN(sizeof(buf1),sizeof(buf2)), array_1234
// and array_1234567 have the same value.
BASIC_MEM_MACRO_TEST_SET(ASSERT_MEM_EQ, array_1234, array_1234567, array_2234)
BASIC_MEM_MACRO_TEST_SET(EXPECT_MEM_EQ, array_1234, array_1234567, array_2234)
BASIC_MEM_MACRO_TEST_SET(ASSERT_MEM_NE, array_1234, array_2234, array_1234)
BASIC_MEM_MACRO_TEST_SET(EXPECT_MEM_NE, array_1234, array_2234, array_1234)

MEM_MACRO_TEST_SET_EQ(array_1234, 0, empty_buffer)
MEM_MACRO_TEST_SET_EQ(array_longer, sizeof(array_longer), longer)

MEM_MACRO_TEST_SET_NE(array_1234, array_2234, first_byte)
MEM_MACRO_TEST_SET_NE(array_1234, array_1233, last_byte)

