// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
#include "testprefix.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void test_suite1_ok_integer_comparison()
{
    TP_ASSERT(1 == 1);
}

void test_suite1_nok_integer_comparison()
{
    TP_ASSERT(1 == 2);
}

void test_suite1_nok_mem_comparison()
{
    TP_ASSERT(memcmp("abcd", "efghi", sizeof("abcd")) == 0);
}

void test_suite1_ok_mem_comparison()
{
    TP_ASSERT(memcmp("abcd", "abcd", sizeof("abcd")) == 0);
}

void test_suite1_nok_with_free()
{
    char *ptr = malloc(512);

    TP_ASSERT(ptr != NULL);
    TP_ASSERT(1 == 2, free(ptr), printf("Released"));
    free(ptr);
}

void test_suite1_ok_with_free()
{
    char *ptr = malloc(512);

    TP_ASSERT(ptr != NULL);
    TP_ASSERT(2 == 2, free(ptr), printf("Released"));
    free(ptr);
}

static void setup_nok() { TP_ASSERT(1 == 4); }

void test_suite1_nok_in_setup()
{
    setup_nok();
    TP_ASSERT(1 == 1);
}

static void setup_ok() { TP_ASSERT(1 == 1); }

void test_suite1_ok_with_setup()
{
    setup_ok();
    TP_ASSERT(1 == 1);
}

void test_suite1_skip()
{
    TP_SKIP("testing skip");
}

void test_suite1_nok_eq_macro_vars()
{
    int x = 5;
    int y = 3;
    TP_ASSERT_EQ(x, y, "%d");
}

void test_suite1_nok_eq_macro_literals()
{
    TP_ASSERT_EQ(5, 100, "%d");
}

#define AA (13 * 13)
#define BB -5000
void test_suite1_nok_eq_macro_defines()
{
    TP_ASSERT_EQ(AA, BB, "0x%.2x");
}

void test_suite1_nok_ne_macro_vars()
{
    int x = -500;
    int y = -500;
    TP_ASSERT_NE(x, y, "%d");
}

void test_suite1_nok_ne_macro_literals()
{
    TP_ASSERT_NE(125, 125, "%d");
}

#define CC (3 * 20)
#define DD (2 * 30)
void test_suite1_nok_ne_macro_defines()
{
    TP_ASSERT_NE(CC, DD, "0x%.2x", printf("extra print\n"));
}

#define Z 3
void test_suite1_ok_eq_macro()
{
    int x = 3;
    int y = 3;

    // literal
    TP_ASSERT_EQ(3, y, "%d", printf("never printed\n"));
    TP_ASSERT_NE(3, 9, "%d");
    // only variables
    TP_ASSERT_EQ(x, y, "%d");
    // define
    TP_ASSERT_EQ(x, Z, "%d");
}

void test_suite1_nok_mem_eq()
{
    uint8_t a1[] = {1, 2, 3, 4};
    uint8_t a2[] = {1, 2, 33, 44};

    TP_ASSERT_MEM_EQ(a1, a2, sizeof(a1));
}
