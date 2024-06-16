// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"
#include <stdio.h>
#include <unistd.h>

int TP_global_setup()
{
    setbuf(stdout, NULL);
    return 0;
}

void test_pass_verbose()
{
    printf("1 text written to stdout\n");
    printf("2 text written to stderr\n");

    ASSERT_TRUE(true, "this will pass");
}

void test_fail_verbose()
{
    printf("3 text written to stdout\n");
    printf("4 text written to stderr\n");

    ASSERT_TRUE(false, "this will fail");
}
