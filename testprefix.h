// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
// Version 1.2

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

extern struct test_context TP_test_context;

#define TP_ASSERT(cond, ...)                                                   \
    do {                                                                       \
        if (!(cond)) {                                                         \
            snprintf(TP_test_context.err_msg, TP_MAX_MSG_SIZE,                 \
                     "%s:%d: assertion failed: '%s'", __FILE__, __LINE__,      \
                     #cond);                                                   \
            __VA_ARGS__;                                                       \
            longjmp(TP_test_context.env, 1);                                   \
        }                                                                      \
    } while (0)

#define TP_BASE_SCALAR(COND, COND_STR, VAL_A, VAL_B, FMT, ...)                 \
    do {                                                                       \
        if (!(COND)) {                                                         \
            snprintf(TP_test_context.err_msg, TP_MAX_MSG_SIZE,                 \
                     "%s:%d: assertion failed: '%s'. Values: " FMT ", " FMT,   \
                     __FILE__, __LINE__, COND_STR, VAL_A, VAL_B);              \
            __VA_ARGS__;                                                       \
            longjmp(TP_test_context.env, 1);                                   \
        }                                                                      \
    } while (0)

#define TP_ASSERT_MEM_EQ(BUF_A, BUF_B, SIZE, ...)                              \
    do {                                                                       \
        if (memcmp(BUF_A, BUF_B, SIZE) != 0) {                                 \
            size_t first_diff = 0;                                             \
            for (size_t i = 0; i < SIZE; i++) {                                \
                if (((uint8_t *)(BUF_A))[i] != ((uint8_t *)(BUF_B))[i]) {      \
                    first_diff = i;                                            \
                    break;                                                     \
                }                                                              \
            }                                                                  \
            snprintf(TP_test_context.err_msg, TP_MAX_MSG_SIZE,                 \
                     "%s:%d: assertion failed: buffers are not equal."         \
                     " First difference at [%zu]: 0x%.2x - 0x%.2x",            \
                     __FILE__, __LINE__, first_diff,                           \
                     ((uint8_t *)(BUF_A))[first_diff],                         \
                     ((uint8_t *)(BUF_B))[first_diff]);                        \
            __VA_ARGS__;                                                       \
            longjmp(TP_test_context.env, 1);                                   \
        }                                                                      \
    } while (0)

#define TP_ASSERT_EQ(VAL_A, VAL_B, FMT, ...)                                   \
    TP_BASE_SCALAR((VAL_A) == (VAL_B), #VAL_A " == " #VAL_B, VAL_A, VAL_B,     \
                   FMT, __VA_ARGS__)

#define TP_ASSERT_NE(VAL_A, VAL_B, FMT, ...)                                   \
    TP_BASE_SCALAR((VAL_A) != (VAL_B), #VAL_A " != " #VAL_B, VAL_A, VAL_B,     \
                   FMT, __VA_ARGS__)

#endif // TESTPREFIX_H_
