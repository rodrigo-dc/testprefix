// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
#include "testprefix.h"
#include <stdio.h>

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
    TP_ASSERT(t, memcmp("asdf", __FUNCTION__,
                     sizeof(__FUNCTION__)) == 0);
    return 0;
}

int test_suite1_ok_mem_comparison(void *t)
{
    TP_ASSERT(t, memcmp("test_suite1_ok_mem_comparison", __FUNCTION__,
                     sizeof(__FUNCTION__)) == 0);
    return 0;
}

void setup(void *t)
{
    TP_ASSERT(t, 1 == 4);
}

int test_suite1_com_setup(void *t)
{
    setup(t);
    TP_ASSERT(t, 1);

    return 0;
}

int test_suite1_skip(void *t)
{
    (void)t;
    return 1;
}
