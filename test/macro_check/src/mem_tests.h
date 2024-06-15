// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#define MIN(A, B) ((A) < (B) ? (A) : (B))

// Standard set of tests for mactos that check the content of memory buffers,
// such as ASSERT_MEM_EQ, EXPECT_MEM_EQ, ASSERT_MEM_NE and EXPECT_MEM_NE.

#define BASIC_MEM_MACRO_TEST_SET(MACRO, BUF1, BUF2_SUCCESS, BUF2_FAILURE)      \
    /* A successful check does not fail the test */                            \
    void test_err0_msg0_##MACRO##_ok()                                         \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2_SUCCESS));                 \
        MACRO(BUF1, BUF2_SUCCESS, size, "##unexpected message##");             \
    }                                                                          \
    /* A literal message is printed when a check fails */                      \
    void test_err1_msg1_##MACRO##_simple_message()                             \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2_FAILURE));                 \
        MACRO(BUF1, BUF2_FAILURE, size, "##expected message##");               \
    }                                                                          \
    /* A formatted message is printed when a check fails */                    \
    void test_err1_msg1_##MACRO##_formated_message()                           \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2_FAILURE));                 \
        MACRO(BUF1, BUF2_FAILURE, size, "%s", "##expected message##");         \
    }                                                                          \
    /* On multiple errors                                    */                \
    /*     for ASSERT macros: only one message must be seen.   */              \
    /*     for EXPECT macros: all three messages must be seen. */              \
    void test_err3_msg3_##MACRO##_multiple_errors()                            \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2_FAILURE));                 \
        MACRO(BUF1, BUF2_FAILURE, size, "##expected message##");               \
        MACRO(BUF1, BUF2_FAILURE, size, "##expected message##");               \
        MACRO(BUF1, BUF2_FAILURE, size, "##expected message##");               \
    }                                                                          \
    /* Multiple successful checks don't fail the test. */                      \
    void test_err0_msg0_##MACRO##_multiple_success()                           \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2_SUCCESS));                 \
        MACRO(BUF1, BUF2_SUCCESS, size, "##unexpected message##");             \
        MACRO(BUF1, BUF2_SUCCESS, size, "##unexpected message##");             \
        MACRO(BUF1, BUF2_SUCCESS, size, "##unexpected message##");             \
    }

// Set of tests that provide the same buffer (BUF)
#define MEM_MACRO_TEST_SET_EQ(BUF, SIZE, SUFFIX)                               \
    void test_err0_msg0_ASSERT_MEM_EQ_##SUFFIX##_()                            \
    {                                                                          \
        ASSERT_MEM_EQ(BUF, BUF, SIZE);                                         \
    }                                                                          \
    void test_err0_msg0_EXPECT_MEM_EQ_##SUFFIX##_()                            \
    {                                                                          \
        EXPECT_MEM_EQ(BUF, BUF, SIZE);                                         \
    }                                                                          \
    void test_err1_msg0_ASSERT_MEM_NE_##SUFFIX##_()                            \
    {                                                                          \
        ASSERT_MEM_NE(BUF, BUF, SIZE);                                         \
    }                                                                          \
    void test_err1_msg0_EXPECT_MEM_NE_##SUFFIX##_()                            \
    {                                                                          \
        EXPECT_MEM_NE(BUF, BUF, SIZE);                                         \
    }

// Set of tests that provide different buffers (BUF1 and BUF2)
#define MEM_MACRO_TEST_SET_NE(BUF1, BUF2, SUFFIX)                              \
    void test_err1_msg0_ASSERT_MEM_EQ_##SUFFIX##_()                            \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2));                         \
        ASSERT_MEM_EQ(BUF1, BUF2, size);                                       \
    }                                                                          \
    void test_err1_msg0_EXPECT_MEM_EQ_##SUFFIX##_()                            \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2));                         \
        EXPECT_MEM_EQ(BUF1, BUF2, size);                                       \
    }                                                                          \
    void test_err0_msg0_ASSERT_MEM_NE_##SUFFIX##_()                            \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2));                         \
        ASSERT_MEM_NE(BUF1, BUF2, size);                                       \
    }                                                                          \
    void test_err0_msg0_EXPECT_MEM_NE_##SUFFIX##_()                            \
    {                                                                          \
        size_t size = MIN(sizeof(BUF1), sizeof(BUF2));                         \
        EXPECT_MEM_NE(BUF1, BUF2, size);                                       \
    }
