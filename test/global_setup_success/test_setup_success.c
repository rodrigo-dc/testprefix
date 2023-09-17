#include "testprefix.h"

int TP_global_setup()
{
    printf("Custom global setup!\n");
    return 0;
}

void TP_global_teardown()
{
    printf("Custom global teardown!\n");
}

int test_single_error()
{
    TP_ASSERT(1 != 0);
    return 0;
}
