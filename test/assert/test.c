// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.
#include "testprefix.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void test_pass_assert_true()
{
    ASSERT_TRUE(13 * 13 == 169, "simple multiplication still works");
}

void test_fail_assert_true_formatted_message()
{
    ASSERT_TRUE(0 > 1, "%s", "not that useful");
}

void test_fail_assert_true_literal()
{
    ASSERT_TRUE(0 > 1, "check error messages with literals");
}

void test_fail_assert_true_var()
{
    int a = 2;
    int b = 3;

    ASSERT_TRUE(a == b, "check error messages with variable names");
}

void test_fail_assert_true_define()
{
#define A 5
#define B 10
    ASSERT_TRUE(A > B, "check error messages with preprocessor definitions");
#undef A
#undef B
}

void test_pass_assert_false()
{
    ASSERT_FALSE(0, "simple multiplication still works");
}

void test_fail_assert_false_no_message() { ASSERT_FALSE(20 > 10); }

void test_fail_assert_false_literal()
{
    ASSERT_FALSE(20 > 10, "check error messages with literals");
}

void test_fail_assert_false_var()
{
    int a = 2;
    int b = 3;

    ASSERT_FALSE(a != b, "check error messages with variable names");
}

void test_fail_assert_false_define()
{
#define A 5
#define B 10
    ASSERT_FALSE(B > A, "check error messages with preprocessor definitions");
#undef A
#undef B
}

void test_pass_assert_uint_eq()
{
    ASSERT_UINT_EQ(255, 255, "checking equality");
}

void test_fail_assert_uint_eq_no_message() { ASSERT_UINT_EQ(20, 10); }

void test_fail_assert_uint_eq_literal()
{
    ASSERT_UINT_EQ(20, 10, "check error messages with literals");
}

void test_fail_assert_uint_eq_var()
{
    unsigned char a = 2;
    unsigned char b = 3;

    ASSERT_UINT_EQ(a, b, "check error messages with variable names");
}

void test_fail_assert_uint_eq_define()
{
#define A 5
#define B 10
    ASSERT_UINT_EQ(A, B, "check error messages with preprocessor definitions");
#undef A
#undef B
}

void test_pass_assert_uint_ne()
{
    ASSERT_UINT_NE(255, 262, "checking inequality");
}

void test_fail_assert_uint_ne_literal()
{
    ASSERT_UINT_NE(20, 20, "check error messages with literals");
}

void test_fail_assert_uint_ne_var()
{
    unsigned char a = 3;
    unsigned char b = 3;

    ASSERT_UINT_NE(a, b, "check error messages with variable names");
}

void test_fail_assert_uint_ne_define()
{
#define A 500
#define B 500
    ASSERT_UINT_NE(A, B, "check error messages with preprocessor definitions");
#undef A
#undef B
}

void test_pass_assert_int_eq()
{
    ASSERT_INT_EQ(-255, -255, "checking equality");
}

void test_fail_assert_int_eq_formatted_message()
{
    char a = 2;
    char b = -3;

    ASSERT_INT_EQ(a, b, "sum: %d", a + b);
}

void test_fail_assert_int_eq_literal()
{
    ASSERT_INT_EQ(-20, 10, "check error messages with literals");
}

void test_fail_assert_int_eq_var()
{
    char a = 2;
    char b = -3;

    ASSERT_INT_EQ(a, b, "check error messages with variable names");
}

void test_fail_assert_int_eq_define()
{
#define A 5
#define B 10
    ASSERT_INT_EQ(A, B, "check error messages with preprocessor definitions");
#undef A
#undef B
}

void test_pass_assert_int_ne()
{
    ASSERT_INT_NE(-255, 262, "checking inequality");
}

void test_fail_assert_int_ne_literal()
{
    ASSERT_INT_NE(-20, -20, "check error messages with literals");
}

void test_fail_assert_int_ne_var()
{
    char a = 3;
    char b = 3;

    ASSERT_INT_NE(a, b, "check error messages with variable names");
}

void test_fail_assert_int_ne_define()
{
#define A (-500)
#define B (-500)
    ASSERT_INT_NE(A, B, "check error messages with preprocessor definitions");
#undef A
#undef B
}

void test_pass_assert_ptr_eq()
{
    void *ptr = (void *)0x80000001;

    ASSERT_PTR_EQ(ptr, ptr, "comparing the same pointer");
}

void test_pass_assert_ptr_eq_null()
{
    void *ptr = NULL;

    ASSERT_PTR_EQ(ptr, NULL, "comparing pointer to NULL");
}

void test_fail_assert_ptr_eq()
{
    void *ptr = (void *)0x80000001;

    ASSERT_PTR_EQ(ptr, NULL, "comparing valid pointer to NULL");
}

void test_pass_assert_ptr_ne()
{
    int a = 3;
    int b = 2;

    ASSERT_PTR_NE(&a, &b, "comparing pointers to different variables");
}

void test_pass_assert_ptr_ne_null()
{
    int a = 0;

    ASSERT_PTR_NE(&a, NULL, "comparing pointer to NULL");
}

void test_fail_assert_ptr_ne()
{
    void *ptr = (void *)0x80000001;

    ASSERT_PTR_NE(ptr, ptr, "comparing the same pointer");
}

void test_pass_assert_str_eq()
{
    ASSERT_STR_EQ("abcdef", "abcdef", "the same string, should not fail");
}

void test_fail_assert_str_eq_literal()
{
    ASSERT_STR_EQ("abcdef", "zyx", "check error messages with literals");
}

void test_fail_assert_str_eq_var()
{
    const char *s1 = "eee";
    const char *s2 = "qwertyuiop";

    ASSERT_STR_EQ(s1, s2, "check error messages with variables");
}

void test_fail_assert_str_eq_define()
{
#define STRING1 "asdfghjkl"
#define STRING2 ""
    ASSERT_STR_EQ(STRING1, STRING2,
                  "check error messages with preprocessor definitions");
#undef STRING1
#undef STRING2
}

void test_pass_assert_str_ne()
{
    ASSERT_STR_NE("+++", "===", "different strings, should not fail");
}

void test_fail_assert_str_ne_literal()
{
    ASSERT_STR_NE("zxcvbnm", "zxcvbnm", "check error messages with literals");
}

void test_fail_assert_str_ne_var()
{
    const char *s1 = "qwertyuiop";
    const char *s2 = "qwertyuiop";

    ASSERT_STR_NE(s1, s2, "check error messages with variables");
}

void test_fail_assert_str_ne_define()
{
#define STRING1 "asdfghjkl"
#define STRING2 "asdfghjkl"
    ASSERT_STR_NE(STRING1, STRING2,
                  "check error messages with preprocessor definitions");
#undef STRING1
#undef STRING2
}

void test_pass_assert_mem_eq()
{
    uint8_t a1[] = {0x01, 0x02, 0x03};
    uint8_t a2[] = {0x01, 0x02, 0x03};

    ASSERT_MEM_EQ(a1, a2, 3, "two arrays, same content");
}

void test_fail_assert_mem_eq_no_message()
{
    uint8_t a1[] = {0xaa, 0xbb, 0xcc};
    uint8_t a2[] = {0xdd, 0xbb, 0xcc};

    ASSERT_MEM_EQ(a1, a2, 3);
}

void test_fail_assert_mem_eq_first_byte()
{
    uint8_t a1[] = {0xaa, 0xbb, 0xcc};
    uint8_t a2[] = {0xdd, 0xbb, 0xcc};

    ASSERT_MEM_EQ(a1, a2, 3, "first byte different");
}

void test_fail_assert_mem_eq_last_byte()
{
    uint8_t a1[] = {0xaa, 0xbb, 0xcc};
    uint8_t a2[] = {0xaa, 0xbb, 0xdd};

    ASSERT_MEM_EQ(a1, a2, 3, "last byte different");
}

void test_fail_assert_mem_eq_truncated_buf()
{
    uint8_t a1[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    uint8_t a2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1e};

    ASSERT_MEM_EQ(a1, a2, sizeof(a1), "big buffer, last byte different");
}

#if 0 // Causes compilation error
void test_fail_assert_mem_eq_truncated_msg()
{
    uint8_t a1[] = {0xaa, 0xbb, 0xcc};
    uint8_t a2[] = {0xdd, 0xbb, 0xcc};

    ASSERT_MEM_EQ(a1, a2, 3,
                  "first byte different, and trying to create a very long "
                  "message so we can test what happens to the error message");
}
#endif

void test_pass_assert_mem_ne()
{
    uint8_t a1[] = {0x01, 0x02, 0x03};
    uint8_t a2[] = {0x02, 0x02, 0x03};

    ASSERT_MEM_NE(a1, a2, 3, "two different arrays");
}

void test_fail_assert_mem_ne()
{
    uint8_t a1[] = {0xaa, 0xbb, 0xcc};
    uint8_t a2[] = {0xaa, 0xbb, 0xcc};

    ASSERT_MEM_NE(a1, a2, 3, "same content, size: %zu", sizeof(a1));
}

static void setup_not_ok() { ASSERT_TRUE(1 == 4, "this should fail"); }
static void setup_ok() { ASSERT_TRUE(4 == 4, "this should pass"); }

void test_fail_in_setup() { setup_not_ok(); }

void test_pass_in_setup()
{
    setup_ok();
    ASSERT_TRUE(0 == 0, "should be true");
}

void test_skip()
{
    SKIP("testing SKIP");
    ASSERT_TRUE(0, "this would fail, but we are skipping the test");
}

void test_skip_without_reason()
{
    SKIP();
    ASSERT_TRUE(0, "this would fail, but we are skipping the test");
}

void test_fail_with_free()
{
    uint8_t *buf = malloc(10);

    SET_TEST_FAILURE_HANDLER(free, buf);
    ASSERT_TRUE(false);
}

void file_closer(void *ptr)
{
    FILE *stream = (FILE *)ptr;
    if (stream != NULL) {
        (void)fclose(stream);
    }
}

void test_fail_with_custom_func()
{
    FILE *stream = tmpfile();
    ASSERT_PTR_NE(stream, NULL);

    SET_TEST_FAILURE_HANDLER(file_closer, stream);
    ASSERT_TRUE(false);
}

void test_fail_with_null_handler()
{
    // NULL failure handler has no effect
    SET_TEST_FAILURE_HANDLER(NULL, NULL);
    ASSERT_TRUE(false);
}
