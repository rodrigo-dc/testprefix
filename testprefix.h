// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
// Version 1.1

#ifndef TESTPREFIX_H_
#define TESTPREFIX_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#ifndef TP_MAX_MSG_SIZE
#define TP_MAX_MSG_SIZE 255
#endif

struct test_context {
    jmp_buf env;
    char *err_msg;
};

#define TP_ASSERT(t, cond, ...)                                                \
    do {                                                                       \
        struct test_context *ctx = (struct test_context *)t;                   \
        if (!(cond)) {                                                         \
            snprintf(ctx->err_msg, TP_MAX_MSG_SIZE,                            \
                     "%s:%d: assertion failed: '%s'", __FILE__, __LINE__,      \
                     #cond);                                                   \
            __VA_ARGS__;                                                       \
            longjmp(ctx->env, 1);                                              \
        }                                                                      \
    } while (0)

#define TP_BASE_SCALAR(T, COND, COND_STR, VAL_A, VAL_B, FMT, ...)              \
    do {                                                                       \
        struct test_context *ctx = (struct test_context *)T;                   \
        if (!(COND)) {                                                         \
            snprintf(ctx->err_msg, TP_MAX_MSG_SIZE,                            \
                     "%s:%d: assertion failed: '%s'. Values: " FMT ", " FMT,   \
                     __FILE__, __LINE__, COND_STR, VAL_A, VAL_B);              \
            __VA_ARGS__;                                                       \
            longjmp(ctx->env, 1);                                              \
        }                                                                      \
    } while (0)

#define TP_ASSERT_MEM_EQ(T, BUF_A, BUF_B, SIZE, ...)                           \
    do {                                                                       \
        struct test_context *ctx = (struct test_context *)T;                   \
        if (memcmp(BUF_A, BUF_B, SIZE) != 0) {                                 \
            size_t first_diff = 0;                                             \
            for (size_t i = 0; i < SIZE; i++) {                                \
                if (((uint8_t *)(BUF_A))[i] != ((uint8_t *)(BUF_B))[i]) {      \
                    first_diff = i;                                            \
                    break;                                                     \
                }                                                              \
            }                                                                  \
            snprintf(ctx->err_msg, TP_MAX_MSG_SIZE,                            \
                     "%s:%d: assertion failed: buffers are not equal."         \
                     " First difference at [%zu]: 0x%.2x - 0x%.2x",            \
                     __FILE__, __LINE__, first_diff,                           \
                     ((uint8_t *)(BUF_A))[first_diff],                         \
                     ((uint8_t *)(BUF_B))[first_diff]);                        \
            __VA_ARGS__;                                                       \
            longjmp(ctx->env, 1);                                              \
        }                                                                      \
    } while (0)

#define TP_ASSERT_EQ(T, VAL_A, VAL_B, FMT, ...)                                \
    TP_BASE_SCALAR(T, (VAL_A) == (VAL_B), #VAL_A " == " #VAL_B, VAL_A, VAL_B,  \
                   FMT, __VA_ARGS__)

#define TP_ASSERT_NE(T, VAL_A, VAL_B, FMT, ...)                                \
    TP_BASE_SCALAR(T, (VAL_A) != (VAL_B), #VAL_A " != " #VAL_B, VAL_A, VAL_B,  \
                   FMT, __VA_ARGS__)

#endif // TESTPREFIX_H_
