// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"

void test_err1_msg0_FAIL_nomessage() { FAIL(); }

void test_err1_msg1_FAIL_message()
{
    FAIL("##expected message##");
    ASSERT_TRUE(false, "##unexpected message##");
}
