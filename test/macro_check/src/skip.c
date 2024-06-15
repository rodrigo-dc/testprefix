// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"

void test_err0_msg0_SKIP_nomessage() { SKIP(); }

void test_err0_msg1_SKIP_message() { SKIP("##expected message##"); }
