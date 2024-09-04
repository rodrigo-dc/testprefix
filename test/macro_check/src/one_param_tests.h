// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

// Standard set of tests for macros that test a single parameter, such as
// ASSERT_TRUE, EXPECT_TRUE, ASSERT_FALSE and EXPECT_FALSE.

//
// ERROR_VALUE must be defined by the caller of this macro, before invoking
//             it. It must be defined to a literal value that causes the macro
//             to fail. Ex.: #define ERROR_VALUE false
//

#include <assert.h>

#define ONE_PARAM_MACRO_TEST_SET(MACRO, PARAM_SUCCESS, PARAM_FAILURE)          \
    /* A successful check does not fail the test */                            \
    void test_err0_msg0_##MACRO##_ok()                                         \
    {                                                                          \
        MACRO(PARAM_SUCCESS, "##unexpected message##");                        \
    }                                                                          \
    /* A failed check on a literal fails the test and prints the literal */    \
    void test_err1_msg0_##MACRO##_literal() { MACRO(PARAM_FAILURE); }          \
    /* A failed check on a preprocessor definition fails the test and prints   \
     * the preprocessor symbol */                                              \
    void test_err1_msg0_##MACRO##_define() { MACRO(ERROR_VALUE); }             \
    /* A failed check on a variable fails the test and prints the variable     \
     * name */                                                                 \
    void test_err1_msg0_##MACRO##_variable()                                   \
    {                                                                          \
        bool some_variable = PARAM_FAILURE;                                    \
        MACRO(some_variable);                                                  \
    }                                                                          \
    /* A literal message is printed when a check fails */                      \
    void test_err1_msg1_##MACRO##_simple_message()                             \
    {                                                                          \
        MACRO(PARAM_FAILURE, "##expected message##");                          \
    }                                                                          \
    /* A formatted message is printed when a check fails */                    \
    void test_err1_msg1_##MACRO##_formated_message()                           \
    {                                                                          \
        MACRO(PARAM_FAILURE, "%s", "##expected message##");                    \
    }                                                                          \
    /* On multiple errors                                    */                \
    /*     for ASSERT macros: only one message must be seen.   */              \
    /*     for EXPECT macros: all three messages must be seen. */              \
    void test_err3_msg3_##MACRO##_multiple_errors()                            \
    {                                                                          \
        MACRO(PARAM_FAILURE, "##expected message##");                          \
        MACRO(PARAM_FAILURE, "##expected message##");                          \
        MACRO(PARAM_FAILURE, "##expected message##");                          \
    }                                                                          \
    /* Multiple successful checks don't fail the test. */                      \
    void test_err0_msg0_##MACRO##_multiple_success()                           \
    {                                                                          \
        MACRO(PARAM_SUCCESS, "##unexpected message##");                        \
        MACRO(PARAM_SUCCESS, "##unexpected message##");                        \
        MACRO(PARAM_SUCCESS, "##unexpected message##");                        \
    }                                                                          \
    /* The macro works with arguments that have side effects */                \
    void test_err1_msg1_##MACRO##_side_effect_arguments()                      \
    {                                                                          \
        int i = 0;                                                             \
        MACRO((i++, PARAM_FAILURE), "##expected message##");                   \
        assert(i == 1);                                                        \
    }                                                                          \
    /* The macro works with arguments that have side effects */                \
    void test_err0_msg0_##MACRO##_side_effect_arguments()                      \
    {                                                                          \
        int i = 0;                                                             \
        MACRO((i++, PARAM_SUCCESS), "##unexpected message##");                 \
        assert(i == 1);                                                        \
    }
