// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
#include "testprefix.h"
#include <stdlib.h>
#include <string.h>

int test_suite1_ok_integer_comparison(void *t)
{
    TP_ASSERT(t, 1 == 1);
    return 0;
}

int test_suite1_nok_integer_comparison(void *t)
{
    TP_ASSERT(t, 1 == 2);
    return 0;
}

int test_suite1_nok_mem_comparison(void *t)
{
    TP_ASSERT(t, memcmp("abcd", "efghi", sizeof("abcd")) == 0);
    return 0;
}

int test_suite1_ok_mem_comparison(void *t)
{
    TP_ASSERT(t, memcmp("abcd", "abcd", sizeof("abcd")) == 0);
    return 0;
}

int test_suite1_nok_with_free(void *t)
{
    char *ptr = malloc(512);

    TP_ASSERT(t, ptr != NULL);
    TP_ASSERT(t, 1 == 2, free(ptr), printf("Released"));
    free(ptr);

    return 0;
}

int test_suite1_ok_with_free(void *t)
{
    char *ptr = malloc(512);

    TP_ASSERT(t, ptr != NULL);
    TP_ASSERT(t, 2 == 2, free(ptr), printf("Released"));
    free(ptr);

    return 0;
}

static void setup_nok(void *t)
{
    TP_ASSERT(t, 1 == 4);
}

int test_suite1_nok_in_setup(void *t)
{
    setup_nok(t);
    TP_ASSERT(t, 1 == 1);

    return 0;
}

static void setup_ok(void *t)
{
    TP_ASSERT(t, 1 == 1);
}

int test_suite1_ok_with_setup(void *t)
{
    setup_ok(t);
    TP_ASSERT(t, 1 == 1);

    return 0;
}

int test_suite1_skip(void *t)
{
    (void)t;
    return 1;
}
