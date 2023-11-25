// Copyright 2021 Rodrigo Dias Correa. See LICENSE.
// Version 2.x

#ifndef TESTPREFIX_H_
#define TESTPREFIX_H_

#include <inttypes.h>
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

typedef void (*TP_failure_handler)(void *);

struct TP_test_context {
    jmp_buf env;
    struct TP_test_result result;
    TP_failure_handler fail_handler;
    void *fail_handler_arg;
};

extern struct TP_test_context TP_context;

// Internal function, not part of the public API.
void TP_mem_to_string(char *str, size_t str_max_size, const void *mem,
                      size_t mem_size);

#define TP_STR(X) #X
#define TP_STR_VAL(X) TP_STR(X)
#define TP_LINE_STR TP_STR_VAL(__LINE__)

#define SET_TEST_FAILURE_HANDLER(HANDLER, HANDLER_ARG)                         \
    do {                                                                       \
        if ((HANDLER) != NULL) {                                               \
            TP_context.fail_handler = HANDLER;                                 \
            TP_context.fail_handler_arg = HANDLER_ARG;                         \
        }                                                                      \
    } while (0)

#define SKIP(...)                                                              \
    do {                                                                       \
        TP_context.result.status = TP_TEST_SKIPPED;                            \
        (void)snprintf(TP_context.result.message, TP_MAX_MSG_SIZE,             \
                       " " __VA_ARGS__);                                       \
        longjmp(TP_context.env, 1);                                            \
    } while (0)

#define TP_BASE_BOOL(COND, COND_STR, ...)                                      \
    do {                                                                       \
        if (!(COND)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            (void)snprintf(TP_context.result.message, TP_MAX_MSG_SIZE,         \
                           __FILE__ ":" TP_LINE_STR                            \
                                    ": assertion failed: '" COND_STR           \
                                    "' - " __VA_ARGS__);                       \
            TP_context.fail_handler(TP_context.fail_handler_arg);              \
            longjmp(TP_context.env, 1);                                        \
        }                                                                      \
    } while (0)

#define TP_BASE_COMPARISON(COND, COND_STR, VAL_A, VAL_B, FMT, TYPE, ...)       \
    do {                                                                       \
        if (!(COND)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            (void)snprintf(TP_context.result.message, TP_MAX_MSG_SIZE,         \
                           __FILE__ ":" TP_LINE_STR                            \
                                    ": assertion failed: '" COND_STR           \
                                    "'. Values: " FMT ", " FMT ".",            \
                           (TYPE)VAL_A, (TYPE)VAL_B);                          \
            const size_t message_len = strlen(TP_context.result.message);      \
            const size_t remaining_space = TP_MAX_MSG_SIZE - message_len;      \
            (void)snprintf(&TP_context.result.message[message_len],            \
                           remaining_space, " " __VA_ARGS__);                  \
            TP_context.fail_handler(TP_context.fail_handler_arg);              \
            longjmp(TP_context.env, 1);                                        \
        }                                                                      \
    } while (0)

#define TP_BASE_MEM_COMPARISON(COND, BUF_A, BUF_B, SIZE, ...)                  \
    do {                                                                       \
        if (!(COND)) {                                                         \
            char a_content[TP_MAX_MSG_SIZE / 4] = {0};                         \
            char b_content[TP_MAX_MSG_SIZE / 4] = {0};                         \
            TP_mem_to_string(a_content, sizeof(a_content), BUF_A, SIZE);       \
            TP_mem_to_string(b_content, sizeof(b_content), BUF_B, SIZE);       \
            TP_context.result.status = TP_TEST_FAILED;                         \
            (void)snprintf(                                                    \
                TP_context.result.message, TP_MAX_MSG_SIZE,                    \
                __FILE__                                                       \
                ":" TP_LINE_STR                                                \
                ": assertion failed: buffers are not equal." __VA_ARGS__);     \
            const size_t message_len = strlen(TP_context.result.message);      \
            const size_t remaining_space = TP_MAX_MSG_SIZE - message_len;      \
            (void)snprintf(&TP_context.result.message[message_len],            \
                           remaining_space, " - Values: %s, %s", a_content,    \
                           b_content);                                         \
            TP_context.fail_handler(TP_context.fail_handler_arg);              \
            longjmp(TP_context.env, 1);                                        \
        }                                                                      \
    } while (0)

#define ASSERT_TRUE(COND, ...) TP_BASE_BOOL((COND), #COND, __VA_ARGS__)

#define ASSERT_FALSE(COND, ...)                                                \
    TP_BASE_BOOL(!(COND), "!(" #COND ")", __VA_ARGS__)

#define ASSERT_UINT_EQ(VAL1, VAL2, ...)                                        \
    TP_BASE_COMPARISON((VAL1) == (VAL2), #VAL1 " == " #VAL2, VAL1, VAL2,       \
                       "%" PRIu64, uint64_t, __VA_ARGS__)

#define ASSERT_UINT_NE(VAL1, VAL2, ...)                                        \
    TP_BASE_COMPARISON((VAL1) != (VAL2), #VAL1 " != " #VAL2, VAL1, VAL2,       \
                       "%" PRIu64, uint64_t, __VA_ARGS__)

#define ASSERT_INT_EQ(VAL1, VAL2, ...)                                         \
    TP_BASE_COMPARISON((VAL1) == (VAL2), #VAL1 " == " #VAL2, VAL1, VAL2,       \
                       "%" PRIi64, int64_t, __VA_ARGS__)

#define ASSERT_INT_NE(VAL1, VAL2, ...)                                         \
    TP_BASE_COMPARISON((VAL1) != (VAL2), #VAL1 " != " #VAL2, VAL1, VAL2,       \
                       "%" PRIi64, int64_t, __VA_ARGS__)

#define ASSERT_PTR_EQ(PTR1, PTR2, ...)                                         \
    TP_BASE_COMPARISON((PTR1) == (PTR2), #PTR1 " == " #PTR2, PTR1, PTR2, "%p", \
                       void *, __VA_ARGS__)

#define ASSERT_PTR_NE(PTR1, PTR2, ...)                                         \
    TP_BASE_COMPARISON((PTR1) != (PTR2), #PTR1 " != " #PTR2, PTR1, PTR2, "%p", \
                       void *, __VA_ARGS__)

#define ASSERT_STR_EQ(STR1, STR2, ...)                                         \
    TP_BASE_COMPARISON(strcmp(STR1, STR2) == 0,                                \
                       "strcmp(" #STR1 ", " #STR2 ") == 0", STR1, STR2,        \
                       "'%s'", const char *, __VA_ARGS__)

#define ASSERT_STR_NE(STR1, STR2, ...)                                         \
    TP_BASE_COMPARISON(strcmp(STR1, STR2) != 0,                                \
                       "strcmp(" #STR1 ", " #STR2 ") != 0", STR1, STR2,        \
                       "'%s'", const char *, __VA_ARGS__)

#define ASSERT_MEM_EQ(PTR1, PTR2, SIZE, ...)                                   \
    TP_BASE_MEM_COMPARISON(memcmp(PTR1, PTR2, SIZE) == 0, PTR1, PTR2, SIZE,    \
                           __VA_ARGS__)

#define ASSERT_MEM_NE(PTR1, PTR2, SIZE, ...)                                   \
    TP_BASE_MEM_COMPARISON(memcmp(PTR1, PTR2, SIZE) != 0, PTR1, PTR2, SIZE,    \
                           __VA_ARGS__)

#endif // TESTPREFIX_H_
