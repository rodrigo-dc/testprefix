// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

// Standard set of tests for macros that test two parameters, such as
// ASSERT_UINT_EQ, EXPECT_UINT_EQ and others.

//
// ERROR_VALUE must be defined by the caller of this macro, before invoking
//             it. It must be defined to a literal value that causes the macro
//             to fail. Ex.: #define ERROR_VALUE false
//

#include <assert.h>

#define TWO_PARAM_MACRO_TEST_SET(MACRO, PARAM1, PARAM2_SUCCESS,                \
                                 PARAM2_FAILURE, VAR_TYPE)                     \
    /* A successful check does not fail the test */                            \
    void test_err0_msg0_##MACRO##_ok()                                         \
    {                                                                          \
        MACRO(PARAM1, PARAM2_SUCCESS, "##unexpected message##");               \
    }                                                                          \
    /* A failed check on literals fails the test and prints the literals */    \
    void test_err1_msg0_##MACRO##_literal() { MACRO(PARAM1, PARAM2_FAILURE); } \
    /* A failed check on a preprocessor definition fails the test and prints   \
     * the preprocessor symbol */                                              \
    void test_err1_msg0_##MACRO##_define() { MACRO(PARAM1, ERROR_VALUE); }     \
    /* A failed check on variables fails the test and prints the name of the   \
     * variables */                                                            \
    void test_err1_msg0_##MACRO##_variable()                                   \
    {                                                                          \
        VAR_TYPE some_variable1 = PARAM1;                                      \
        VAR_TYPE some_variable2 = PARAM2_FAILURE;                              \
        MACRO(some_variable1, some_variable2);                                 \
    }                                                                          \
    /* A literal message is printed when a check fails */                      \
    void test_err1_msg1_##MACRO##_simple_message()                             \
    {                                                                          \
        MACRO(PARAM1, PARAM2_FAILURE, "##expected message##");                 \
    }                                                                          \
    /* A formatted message is printed when a check fails */                    \
    void test_err1_msg1_##MACRO##_formated_message()                           \
    {                                                                          \
        MACRO(PARAM1, PARAM2_FAILURE, "%s", "##expected message##");           \
    }                                                                          \
    /* On multiple errors                                    */                \
    /*     for ASSERT macros: only one message must be seen.   */              \
    /*     for EXPECT macros: all three messages must be seen. */              \
    void test_err3_msg3_##MACRO##_multiple_errors()                            \
    {                                                                          \
        MACRO(PARAM1, PARAM2_FAILURE, "##expected message##");                 \
        MACRO(PARAM1, PARAM2_FAILURE, "##expected message##");                 \
        MACRO(PARAM1, PARAM2_FAILURE, "##expected message##");                 \
    }                                                                          \
    /* Multiple successful checks don't fail the test. */                      \
    void test_err0_msg0_##MACRO##_multiple_success()                           \
    {                                                                          \
        MACRO(PARAM1, PARAM2_SUCCESS, "##unexpected message##");               \
        MACRO(PARAM1, PARAM2_SUCCESS, "##unexpected message##");               \
        MACRO(PARAM1, PARAM2_SUCCESS, "##unexpected message##");               \
    }                                                                          \
    /* The macro works with arguments that have side effects */                \
    void test_err1_msg1_##MACRO##_side_effect_arguments()                      \
    {                                                                          \
        int i = 0;                                                             \
        int j = 0;                                                             \
        MACRO((i++, PARAM1), (j++, PARAM2_FAILURE), "##expected message##");   \
        assert(i == 1);                                                        \
        assert(j == 1);                                                        \
    }                                                                          \
    /* The macro works with arguments that have side effects */                \
    void test_err0_msg0_##MACRO##_side_effect_arguments()                      \
    {                                                                          \
        int i = 0;                                                             \
        int j = 0;                                                             \
        MACRO((i++, PARAM1), (j++, PARAM2_SUCCESS), "##unexpected message##"); \
        assert(i == 1);                                                        \
        assert(j == 1);                                                        \
    }
