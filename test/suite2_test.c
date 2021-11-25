// Copyright 2021 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"
#include <string.h>

int test_suite2_nok_string_comparison(void *t)
{
    TP_ASSERT(t, strcmp("asdf", __FUNCTION__) == 0);
    return 0;
}

int test_suite2_ok_string_comparison(void *t)
{
    TP_ASSERT(t, strcmp("test_suite2_ok_string_comparison",
                         __FUNCTION__) == 0);
    return 0;
}
