// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"
#include <stdio.h>
#include <unistd.h>

void a6() { ASSERT_UINT_EQ(2, 1); }
void a5() { TRACE_CALL(a6()); }
void a4() { TRACE_CALL(a5()); }
void a3() { TRACE_CALL(a4()); }
void a2() { TRACE_CALL(a3()); }
void a1() { TRACE_CALL(a2()); }

void test_fail_call_trace_truncated() { TRACE_CALL(a1()); }

void test_fail_call_trace_not_truncated() { TRACE_CALL(a2()); }

void b6() {}
void b5() { TRACE_CALL(b6()); }
void b4() { TRACE_CALL(b5()); }
void b3() { TRACE_CALL(b4()); }
void b2() { TRACE_CALL(b3()); }
void b1() { TRACE_CALL(b2()); }

void test_fail_call_trace_unroll()
{
    TRACE_CALL(b1());
    ASSERT_TRUE(false, "No call traces should be visible");
}

void c2() { EXPECT_STR_EQ("one", "two"); }
void c1() { TRACE_CALL(c2()); }

void d2() { EXPECT_MEM_EQ("aaa", "bbb", 3); }
void d1() { TRACE_CALL(d2()); }

void test_fail_call_trace_expect()
{
    TRACE_CALL(c1());
    TRACE_CALL(d1());
}
