// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
// Version 1.2

#ifndef TESTPREFIX_H_
#define TESTPREFIX_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>

#ifndef TP_MAX_MSG_SIZE
#define TP_MAX_MSG_SIZE 255
#endif

enum TP_test_status { TP_TEST_PASSED, TP_TEST_FAILED, TP_TEST_SKIPPED };

struct TP_test_result {
    enum TP_test_status status;
    struct timespec begin;
    struct timespec end;
    char message[TP_MAX_MSG_SIZE];
};

struct TP_test_context {
    jmp_buf env;
    struct TP_test_result result;
};

extern struct TP_test_context TP_context;

#define TP_SKIP(REASON)                                                        \
    do {                                                                       \
        TP_context.result.status = TP_TEST_SKIPPED;                            \
        snprintf(TP_context.result.message, TP_MAX_MSG_SIZE, "%s",             \
                 (REASON) == NULL ? "" : (REASON));                              \
        longjmp(TP_context.env, 1);                                            \
    } while (0)

#define TP_ASSERT(cond, ...)                                                   \
    do {                                                                       \
        if (!(cond)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            snprintf(TP_context.result.message, TP_MAX_MSG_SIZE,               \
                     "%s:%d: assertion failed: '%s'", __FILE__, __LINE__,      \
                     #cond);                                                   \
            __VA_ARGS__;                                                       \
            longjmp(TP_context.env, 1);                                        \
        }                                                                      \
    } while (0)

#define TP_BASE_SCALAR(COND, COND_STR, VAL_A, VAL_B, FMT, ...)                 \
    do {                                                                       \
        if (!(COND)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            snprintf(TP_context.result.message, TP_MAX_MSG_SIZE,               \
                     "%s:%d: assertion failed: '%s'. Values: " FMT ", " FMT,   \
                     __FILE__, __LINE__, COND_STR, VAL_A, VAL_B);              \
            __VA_ARGS__;                                                       \
            longjmp(TP_context.env, 1);                                        \
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
            TP_context.result.status = TP_TEST_FAILED;                         \
            snprintf(TP_context.result.message, TP_MAX_MSG_SIZE,               \
                     "%s:%d: assertion failed: buffers are not equal."         \
                     " First difference at [%zu]: 0x%.2x - 0x%.2x",            \
                     __FILE__, __LINE__, first_diff,                           \
                     ((uint8_t *)(BUF_A))[first_diff],                         \
                     ((uint8_t *)(BUF_B))[first_diff]);                        \
            __VA_ARGS__;                                                       \
            longjmp(TP_context.env, 1);                                        \
        }                                                                      \
    } while (0)

#define TP_ASSERT_EQ(VAL_A, VAL_B, FMT, ...)                                   \
    TP_BASE_SCALAR((VAL_A) == (VAL_B), #VAL_A " == " #VAL_B, VAL_A, VAL_B,     \
                   FMT, __VA_ARGS__)

#define TP_ASSERT_NE(VAL_A, VAL_B, FMT, ...)                                   \
    TP_BASE_SCALAR((VAL_A) != (VAL_B), #VAL_A " != " #VAL_B, VAL_A, VAL_B,     \
                   FMT, __VA_ARGS__)

#endif // TESTPREFIX_H_
