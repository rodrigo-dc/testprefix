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

int test_single_success(void *t)
{
    TP_ASSERT(t, 1 != 1);
    return 0;
}
