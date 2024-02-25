// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.
// Version 2.1

#ifndef TESTPREFIX_H_
#define TESTPREFIX_H_

#include <inttypes.h>
#include <stdbool.h>
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

// Internal functions, not part of the public API.
void TP_send_message(unsigned int level, const char *fmt, ...);
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
        TP_send_message(0, "" __VA_ARGS__);                                    \
        longjmp(TP_context.env, 1);                                            \
    } while (0)

#define TP_BASE_BOOL(COND, ERR_MSG, ABORT, ...)                                \
    do {                                                                       \
        if (!(COND)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            TP_send_message(0, __FILE__ ":" TP_LINE_STR ": " ERR_MSG);         \
            TP_send_message(1, "" __VA_ARGS__);                                \
            if (ABORT) {                                                       \
                longjmp(TP_context.env, 1);                                    \
            }                                                                  \
        }                                                                      \
    } while (0)

#define TP_BASE_COMPARISON(COND, ERR_MSG, ABORT, VAL_A, VAL_B, FMT, TYPE, ...) \
    do {                                                                       \
        if (!(COND)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            TP_send_message(0, __FILE__ ":" TP_LINE_STR ": " ERR_MSG);         \
            TP_send_message(1, "Values: " FMT ", " FMT, (TYPE)VAL_A,           \
                            (TYPE)VAL_B);                                      \
            TP_send_message(1, "" __VA_ARGS__);                                \
            if (ABORT) {                                                       \
                longjmp(TP_context.env, 1);                                    \
            }                                                                  \
        }                                                                      \
    } while (0)

#define TP_BASE_MEM_COMPARISON(COND, ERR_MSG, ABORT, BUF_A, BUF_B, SIZE, ...)  \
    do {                                                                       \
        if (!(COND)) {                                                         \
            char a_content[TP_MAX_MSG_SIZE / 2] = {0};                         \
            char b_content[TP_MAX_MSG_SIZE / 2] = {0};                         \
            TP_mem_to_string(a_content, sizeof(a_content), BUF_A, SIZE);       \
            TP_mem_to_string(b_content, sizeof(b_content), BUF_B, SIZE);       \
            TP_context.result.status = TP_TEST_FAILED;                         \
            TP_send_message(0, __FILE__ ":" TP_LINE_STR ": " ERR_MSG);         \
            TP_send_message(1, "First: %s", a_content);                        \
            TP_send_message(1, "Second: %s", b_content);                       \
            TP_send_message(1, "" __VA_ARGS__);                                \
            if (ABORT) {                                                       \
                longjmp(TP_context.env, 1);                                    \
            }                                                                  \
        }                                                                      \
    } while (0)

#define ASSERT_TRUE(COND, ...)                                                 \
    TP_BASE_BOOL(COND, #COND " was expected to be TRUE", true, __VA_ARGS__)
#define EXPECT_TRUE(COND, ...)                                                 \
    TP_BASE_BOOL(COND, #COND " was expected to be TRUE", false, __VA_ARGS__)

#define ASSERT_FALSE(COND, ...)                                                \
    TP_BASE_BOOL(!(COND), #COND " was expected to be FALSE", true, __VA_ARGS__)
#define EXPECT_FALSE(COND, ...)                                                \
    TP_BASE_BOOL(!(COND), #COND " was expected to be FALSE", false, __VA_ARGS__)

#define ASSERT_UINT_EQ(VAL1, VAL2, ...)                                        \
    TP_BASE_COMPARISON((VAL1) == (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be equal", true, \
                       VAL1, VAL2, "%" PRIu64, uint64_t, __VA_ARGS__)
#define EXPECT_UINT_EQ(VAL1, VAL2, ...)                                        \
    TP_BASE_COMPARISON((VAL1) == (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be equal",       \
                       false, VAL1, VAL2, "%" PRIu64, uint64_t, __VA_ARGS__)

#define ASSERT_UINT_NE(VAL1, VAL2, ...)                                        \
    TP_BASE_COMPARISON((VAL1) != (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be different",   \
                       true, VAL1, VAL2, "%" PRIu64, uint64_t, __VA_ARGS__)
#define EXPECT_UINT_NE(VAL1, VAL2, ...)                                        \
    TP_BASE_COMPARISON((VAL1) != (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be different",   \
                       false, VAL1, VAL2, "%" PRIu64, uint64_t, __VA_ARGS__)

#define ASSERT_INT_EQ(VAL1, VAL2, ...)                                         \
    TP_BASE_COMPARISON((VAL1) == (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be equal", true, \
                       VAL1, VAL2, "%" PRIi64, int64_t, __VA_ARGS__)
#define EXPECT_INT_EQ(VAL1, VAL2, ...)                                         \
    TP_BASE_COMPARISON((VAL1) == (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be equal",       \
                       false, VAL1, VAL2, "%" PRIi64, int64_t, __VA_ARGS__)

#define ASSERT_INT_NE(VAL1, VAL2, ...)                                         \
    TP_BASE_COMPARISON((VAL1) != (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be different",   \
                       true, VAL1, VAL2, "%" PRIi64, int64_t, __VA_ARGS__)
#define EXPECT_INT_NE(VAL1, VAL2, ...)                                         \
    TP_BASE_COMPARISON((VAL1) != (VAL2),                                       \
                       #VAL1 " and " #VAL2 " were expected to be different",   \
                       false, VAL1, VAL2, "%" PRIi64, int64_t, __VA_ARGS__)

#define ASSERT_PTR_EQ(PTR1, PTR2, ...)                                         \
    TP_BASE_COMPARISON((PTR1) == (PTR2),                                       \
                       #PTR1 " and " #PTR2 " were expected to be equal", true, \
                       PTR1, PTR2, "%p", void *, __VA_ARGS__)
#define EXPECT_PTR_EQ(PTR1, PTR2, ...)                                         \
    TP_BASE_COMPARISON((PTR1) == (PTR2),                                       \
                       #PTR1 " and " #PTR2 " were expected to be equal",       \
                       false, PTR1, PTR2, "%p", void *, __VA_ARGS__)

#define ASSERT_PTR_NE(PTR1, PTR2, ...)                                         \
    TP_BASE_COMPARISON((PTR1) != (PTR2),                                       \
                       #PTR1 " and " #PTR2 " were expected to be different",   \
                       true, PTR1, PTR2, "%p", void *, __VA_ARGS__)
#define EXPECT_PTR_NE(PTR1, PTR2, ...)                                         \
    TP_BASE_COMPARISON((PTR1) != (PTR2),                                       \
                       #PTR1 " and " #PTR2 " were expected to be different",   \
                       false, PTR1, PTR2, "%p", void *, __VA_ARGS__)

#define ASSERT_STR_EQ(STR1, STR2, ...)                                         \
    TP_BASE_COMPARISON(strcmp(STR1, STR2) == 0,                                \
                       #STR1 " and " #STR2 " were expected to be equal", true, \
                       STR1, STR2, "'%s'", const char *, __VA_ARGS__)
#define EXPECT_STR_EQ(STR1, STR2, ...)                                         \
    TP_BASE_COMPARISON(strcmp(STR1, STR2) == 0,                                \
                       #STR1 " and " #STR2 " were expected to be equal",       \
                       false, STR1, STR2, "'%s'", const char *, __VA_ARGS__)

#define ASSERT_STR_NE(STR1, STR2, ...)                                         \
    TP_BASE_COMPARISON(strcmp(STR1, STR2) != 0,                                \
                       #STR1 " and " #STR2 " were expected to be different",   \
                       true, STR1, STR2, "'%s'", const char *, __VA_ARGS__)
#define EXPECT_STR_NE(STR1, STR2, ...)                                         \
    TP_BASE_COMPARISON(strcmp(STR1, STR2) != 0,                                \
                       #STR1 " and " #STR2 " were expected to be different",   \
                       false, STR1, STR2, "'%s'", const char *, __VA_ARGS__)

#define ASSERT_MEM_EQ(PTR1, PTR2, SIZE, ...)                                   \
    TP_BASE_MEM_COMPARISON(memcmp(PTR1, PTR2, SIZE) == 0,                      \
                           #PTR1 " and " #PTR2                                 \
                                 " were expected to contain the same data",     \
                           true, PTR1, PTR2, SIZE, __VA_ARGS__)
#define EXPECT_MEM_EQ(PTR1, PTR2, SIZE, ...)                                   \
    TP_BASE_MEM_COMPARISON(memcmp(PTR1, PTR2, SIZE) == 0,                      \
                           #PTR1 " and " #PTR2                                 \
                                 " were expected to contain the same data",     \
                           false, PTR1, PTR2, SIZE, __VA_ARGS__)

#define ASSERT_MEM_NE(PTR1, PTR2, SIZE, ...)                                   \
    TP_BASE_MEM_COMPARISON(memcmp(PTR1, PTR2, SIZE) != 0,                      \
                           #PTR1 " and " #PTR2                                 \
                                 " were expected to contain different data",   \
                           true, PTR1, PTR2, SIZE, __VA_ARGS__)
#define EXPECT_MEM_NE(PTR1, PTR2, SIZE, ...)                                   \
    TP_BASE_MEM_COMPARISON(memcmp(PTR1, PTR2, SIZE) != 0,                      \
                           #PTR1 " and " #PTR2                                 \
                                 " were expected to contain different data",   \
                           false, PTR1, PTR2, SIZE, __VA_ARGS__)

#endif // TESTPREFIX_H_
