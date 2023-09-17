#include "testprefix.h"

int TP_global_setup()
{
    printf("Custom global setup with error!\n");
    return -1;
}

void TP_global_teardown()
{
    printf("Custom global teardown! Should never be executed.\n");
}

void test_single_success()
{
    TP_ASSERT(1 != 1);
}
