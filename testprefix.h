// Copyright 2021 Rodrigo Dias Correa. See LICENSE.

#ifndef TESTPREFIX_H_
#define TESTPREFIX_H_

#include <stdio.h>
#include <setjmp.h>

#define TP_MAX_MSG_SIZE 255

struct test_context {
    jmp_buf env;
    char *err_msg;
};

#define TP_ASSERT(t, cond)                                                    \
    do {                                                                      \
        struct test_context *ctx = (struct test_context*)t;                   \
        if (!(cond)) {                                                        \
            snprintf(ctx->err_msg, TP_MAX_MSG_SIZE,                           \
                     "%s:%d: assertion failed: '%s'", __FILE__, __LINE__,     \
                     #cond);                                                  \
            longjmp(ctx->env, 1);                                             \
        }                                                                     \
    } while (0)

#endif // TESTPREFIX_H_
