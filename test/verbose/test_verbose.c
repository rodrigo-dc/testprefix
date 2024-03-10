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
    dprintf(STDOUT_FILENO, "1 text written to stdout\n");
    dprintf(STDERR_FILENO, "2 text written to stderr\n");

    ASSERT_TRUE(true, "this will pass");
}

void test_fail_verbose()
{
    dprintf(STDOUT_FILENO, "3 text written to stdout\n");
    dprintf(STDERR_FILENO, "4 text written to stderr\n");

    ASSERT_TRUE(false, "this will fail");
}
