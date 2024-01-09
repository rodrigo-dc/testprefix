#include "testprefix.h"

// From the logs generated by this test, it's possible to check that:
// - Failed EXPECT macros don't abort the test
// - Despite being variadic, EXPECT macros don't need a message
// - In case of error, in addition to print the values, EXPECT macros reference
//   their parameters (variable name, preprocessor definition or literal value)
void test_fail_expect_with_no_message()
{
    bool a_bool = false;
    EXPECT_TRUE(a_bool);
#define ANOTHER_BOOL (1 == 1)
    EXPECT_FALSE(ANOTHER_BOOL);
#undef ANOTHER_BOOL

#define A (-1)
#define B 1
    EXPECT_INT_EQ(A, B);
    EXPECT_INT_NE(B, B);
#undef A
#undef B

    EXPECT_UINT_EQ(0x1, 0x2);
    EXPECT_UINT_NE(1, 1);

    EXPECT_PTR_EQ(NULL, (void *)0x80000000);
    EXPECT_PTR_NE(NULL, NULL);
    EXPECT_STR_EQ("one", "two");
    EXPECT_STR_NE("one", "one");

    uint8_t a123[] = {0x01, 0x02, 0x03};
    uint8_t a321[] = {0x03, 0x02, 0x01};
    uint8_t b123[] = {0x01, 0x02, 0x03};
    EXPECT_MEM_EQ(a123, a321, 3);
    EXPECT_MEM_NE(a123, b123, 3);

    ASSERT_TRUE(false, "after failed EXPECT macros, ASSERT still works");
}

void test_pass_expect()
{
    const char *message = "satisfied EXPECT macros don't cause test failure";

    EXPECT_TRUE(true, "%s", message);
    EXPECT_FALSE(false, "%s", message);

    EXPECT_INT_EQ(-1, -1);
    EXPECT_INT_NE(1, -2);

    EXPECT_UINT_EQ(1, 1);
    EXPECT_UINT_NE(2, 1);

    void *a_pointer = (void *)0x80000000;
    void *another_pointer_same_value = (void *)0x80000000;
    EXPECT_PTR_EQ(a_pointer, another_pointer_same_value);
    EXPECT_PTR_NE(NULL, a_pointer);

    EXPECT_STR_EQ("one", "one");
    EXPECT_STR_NE("one", "two");

    uint8_t a123[] = {0x01, 0x02, 0x03};
    uint8_t a321[] = {0x03, 0x02, 0x01};
    uint8_t b123[] = {0x01, 0x02, 0x03};
    EXPECT_MEM_EQ(a123, b123, 3);
    EXPECT_MEM_NE(a123, a321, 3);
}

void test_fail_expect_true()
{
    EXPECT_TRUE(1 == 2, "EXPECT_TRUE alone can fail the test");
}

void test_fail_expect_false()
{
    EXPECT_FALSE(1 == 1, "EXPECT_FALSE alone can fail the test");
}

void test_fail_expect_int_eq()
{
    EXPECT_INT_EQ(1, 2, "EXPECT_INT_EQ alone can fail the test");
}

void test_fail_expect_int_ne()
{
    EXPECT_INT_NE(1, 1, "EXPECT_INT_NE alone can fail the test");
}

void test_fail_expect_uint_eq()
{
    EXPECT_UINT_EQ(1, 2, "EXPECT_UINT_EQ alone can fail the test");
}

void test_fail_expect_uint_ne()
{
    EXPECT_UINT_NE(2, 2, "EXPECT_UINT_NE alone can fail the test");
}

void test_fail_expect_ptr_eq()
{
    EXPECT_PTR_EQ(NULL, (void *)0x80000000,
                  "EXPECT_PTR_EQ alone can fail the test");
}

void test_fail_expect_ptr_ne()
{
    EXPECT_PTR_NE(NULL, NULL, "EXPECT_PTR_NE alone can fail the test");
}

void test_fail_expect_str_eq()
{
    EXPECT_STR_EQ("aab", "aabb", "EXPECT_STR_EQ alone can fail the test");
}

void test_fail_expect_str_ne()
{
    EXPECT_STR_NE("ddd", "ddd", "EXPECT_STR_NE alone can fail the test");
}

void test_fail_expect_mem_eq()
{
    uint8_t a[] = {1, 2, 3};
    uint8_t b[] = {1, 2, 4};

    EXPECT_MEM_EQ(a, b, 3, "EXPECT_MEM_EQ alone can fail the test");
}

void test_fail_expect_mem_ne()
{
    uint8_t a[] = {1, 2, 3};
    uint8_t b[] = {1, 2, 3};

    EXPECT_MEM_NE(a, b, 3, "EXPECT_MEM_NE alone can fail the test");
}
