// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

// Standard set of tests for macros that test strings.

// Set of tests that provide the same string (STR)
#define STRING_MACRO_TEST_SET_EQ(STR, SUFFIX)                                  \
    void test_err0_msg0_ASSERT_STR_EQ_##SUFFIX##_()                            \
    {                                                                          \
        ASSERT_STR_EQ(STR, STR);                                               \
    }                                                                          \
    void test_err0_msg0_EXPECT_STR_EQ_##SUFFIX##_()                            \
    {                                                                          \
        EXPECT_STR_EQ(STR, STR);                                               \
    }                                                                          \
    void test_err1_msg0_ASSERT_STR_NE_##SUFFIX##_()                            \
    {                                                                          \
        ASSERT_STR_NE(STR, STR);                                               \
    }                                                                          \
    void test_err1_msg0_EXPECT_STR_NE_##SUFFIX##_() { EXPECT_STR_NE(STR, STR); }

// Set of tests that provide different strings (STR1 and STR2)
#define STRING_MACRO_TEST_SET_NE(STR1, STR2, SUFFIX)                           \
    void test_err1_msg0_ASSERT_STR_EQ_##SUFFIX##_()                            \
    {                                                                          \
        ASSERT_STR_EQ(STR1, STR2);                                             \
    }                                                                          \
    void test_err1_msg0_EXPECT_STR_EQ_##SUFFIX##_()                            \
    {                                                                          \
        EXPECT_STR_EQ(STR1, STR2);                                             \
    }                                                                          \
    void test_err0_msg0_ASSERT_STR_NE_##SUFFIX##_()                            \
    {                                                                          \
        ASSERT_STR_NE(STR1, STR2);                                             \
    }                                                                          \
    void test_err0_msg0_EXPECT_STR_NE_##SUFFIX##_()                            \
    {                                                                          \
        EXPECT_STR_NE(STR1, STR2);                                             \
    }
