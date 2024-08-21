// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

#include <stdlib.h>
#include "testprefix.h"

void test_fail_with_free()
{
    uint8_t *buf = malloc(10);

    SET_TEST_FAILURE_HANDLER(free, buf);
    ASSERT_UINT_EQ(1, 2);
}

void file_closer(void *ptr)
{
    FILE *stream = (FILE *)ptr;
    if (stream != NULL) {
        (void)fclose(stream);
    }
}

void test_fail_with_custom_func()
{
    FILE *stream = tmpfile();
    ASSERT_PTR_NE(stream, NULL);

    SET_TEST_FAILURE_HANDLER(file_closer, stream);
    ASSERT_TRUE(false);
}

void test_fail_with_null_handler()
{
    // NULL failure handler has no effect
    SET_TEST_FAILURE_HANDLER(NULL, NULL);
    ASSERT_TRUE(false);
}
