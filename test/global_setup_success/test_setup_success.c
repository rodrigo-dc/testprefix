#include "testprefix.h"

int TP_global_setup()
{
    printf("Custom global setup!\n");
    return 0;
}

void TP_global_teardown() { printf("Custom global teardown!\n"); }

void test_single() { ASSERT_TRUE(1 != 0, "this will pass"); }
