// Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.
// Version 2.3

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

#ifndef TP_CALL_TRACES_MAX
#define TP_CALL_TRACES_MAX 5
#endif
#if TP_CALL_TRACES_MAX < 1
#error TP_CALL_TRACES_MAX must be greater than zero!
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
    const char *call_traces[TP_CALL_TRACES_MAX];
    unsigned int call_traces_index;
};

extern struct TP_test_context TP_context;

// Internal functions, not part of the public API.
void TP_send_message(unsigned int level, const char *fmt, ...);
void TP_mem_to_string(char *str, size_t str_max_size, const void *mem,
                      size_t mem_size);
void TP_push_call_trace(const char *info);
void TP_pop_call_trace(void);
void TP_report_call_traces(unsigned int level);

#define TP_STR(X) #X
#define TP_STR_VAL(X) TP_STR(X)
#define TP_LINE_STR TP_STR_VAL(__LINE__)

#define TP_BASE_BOOL(COND, ERR_MSG, ABORT, ...)                                \
    do {                                                                       \
        if (!(COND)) {                                                         \
            TP_context.result.status = TP_TEST_FAILED;                         \
            TP_report_call_traces(0);                                          \
            TP_send_message(0, __FILE__ ":" TP_LINE_STR ": " ERR_MSG);         \
            TP_send_message(1, "" __VA_ARGS__);                                \
            if (ABORT) {                                                       \
                longjmp(TP_context.env, 1);                                    \
            }                                                                  \
        }                                                                      \
    } while (0)

#define TP_BASE_COMPARISON(COND, ERR_MSG, ABORT, VAL_A, VAL_B, FMT, TYPE, ...) \
    if (!(COND)) {                                                             \
        TP_context.result.status = TP_TEST_FAILED;                             \
        TP_report_call_traces(0);                                              \
        TP_send_message(0, __FILE__ ":" TP_LINE_STR ": " ERR_MSG);             \
        if (strcmp(#TYPE, "uint64_t") == 0) {                                  \
            TP_send_message(                                                   \
                1, "Values: " FMT " (0x%" PRIx64 "), " FMT " (0x%" PRIx64 ")", \
                VAL_A, VAL_A, VAL_B, VAL_B);                                   \
        } else {                                                               \
            TP_send_message(1, "Values: " FMT ", " FMT, VAL_A, VAL_B);         \
        }                                                                      \
        TP_send_message(1, "" __VA_ARGS__);                                    \
        if (ABORT) {                                                           \
            longjmp(TP_context.env, 1);                                        \
        }                                                                      \
    }

#define TP_BASE_MEM_COMPARISON(COND, ERR_MSG, ABORT, BUF_A, BUF_B, SIZE, ...)  \
    do {                                                                       \
        if (!(COND)) {                                                         \
            char a_content[TP_MAX_MSG_SIZE / 2] = {0};                         \
            char b_content[TP_MAX_MSG_SIZE / 2] = {0};                         \
            TP_mem_to_string(a_content, sizeof(a_content), BUF_A, SIZE);       \
            TP_mem_to_string(b_content, sizeof(b_content), BUF_B, SIZE);       \
            TP_context.result.status = TP_TEST_FAILED;                         \
            TP_report_call_traces(0);                                          \
            TP_send_message(0, __FILE__ ":" TP_LINE_STR ": " ERR_MSG);         \
            TP_send_message(1, " First: %s", a_content);                       \
            TP_send_message(1, "Second: %s", b_content);                       \
            TP_send_message(1, "" __VA_ARGS__);                                \
            if (ABORT) {                                                       \
                longjmp(TP_context.env, 1);                                    \
            }                                                                  \
        }                                                                      \
    } while (0)

//                             .------------.
// ----------------------------| Public API |-----------------------------------
//                             '------------'

// .----------------------.
// | ASSERT/EXPECT macros |
// '----------------------'

// Boolean condition
#define ASSERT_TRUE(COND, ...)                                                 \
    TP_BASE_BOOL(COND, #COND " was expected to be TRUE", true, __VA_ARGS__)

#define EXPECT_TRUE(COND, ...)                                                 \
    TP_BASE_BOOL(COND, #COND " was expected to be TRUE", false, __VA_ARGS__)

#define ASSERT_FALSE(COND, ...)                                                \
    TP_BASE_BOOL(!(COND), #COND " was expected to be FALSE", true, __VA_ARGS__)

#define EXPECT_FALSE(COND, ...)                                                \
    TP_BASE_BOOL(!(COND), #COND " was expected to be FALSE", false, __VA_ARGS__)

// Unsigned integer comparison
#define ASSERT_UINT_EQ(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(TP_val1 == TP_val2,                                 \
                           #VAL1 " and " #VAL2 " were expected to be equal",   \
                           true, TP_val1, TP_val2, "%" PRIu64, uint64_t,       \
                           __VA_ARGS__);                                       \
    } while (0)

#define EXPECT_UINT_EQ(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(TP_val1 == TP_val2,                                 \
                           #VAL1 " and " #VAL2 " were expected to be equal",   \
                           false, TP_val1, TP_val2, "%" PRIu64, uint64_t,      \
                           __VA_ARGS__);                                       \
    } while (0)

#define ASSERT_UINT_NE(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 != TP_val2,                                                \
            #VAL1 " and " #VAL2 " were expected to be different", true,        \
            TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);              \
    } while (0)

#define EXPECT_UINT_NE(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 != TP_val2,                                                \
            #VAL1 " and " #VAL2 " were expected to be different", false,       \
            TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);              \
    } while (0)

#define ASSERT_UINT_LT(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 < TP_val2, #VAL1 " was expected to be less than " #VAL2,   \
            true, TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);        \
    } while (0)

#define EXPECT_UINT_LT(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 < TP_val2, #VAL1 " was expected to be less than " #VAL2,   \
            false, TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);       \
    } while (0)

#define ASSERT_UINT_GT(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(TP_val1 > TP_val2,                                  \
                           #VAL1 " was expected to be greater than " #VAL2,    \
                           true, TP_val1, TP_val2, "%" PRIu64, uint64_t,       \
                           __VA_ARGS__);                                       \
    } while (0)

#define EXPECT_UINT_GT(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(TP_val1 > TP_val2,                                  \
                           #VAL1 " was expected to be greater than " #VAL2,    \
                           false, TP_val1, TP_val2, "%" PRIu64, uint64_t,      \
                           __VA_ARGS__);                                       \
    } while (0)

#define ASSERT_UINT_LE(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 <= TP_val2,                                                \
            #VAL1 " was expected to be less than or equal to " #VAL2, true,    \
            TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);              \
    } while (0)

#define EXPECT_UINT_LE(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 <= TP_val2,                                                \
            #VAL1 " was expected to be less than or equal to " #VAL2, false,   \
            TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);              \
    } while (0)

#define ASSERT_UINT_GE(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 >= TP_val2,                                                \
            #VAL1 " was expected to be greater than or equal to " #VAL2, true, \
            TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);              \
    } while (0)

#define EXPECT_UINT_GE(VAL1, VAL2, ...)                                        \
    do {                                                                       \
        uint64_t TP_val1 = (uint64_t)(VAL1);                                   \
        uint64_t TP_val2 = (uint64_t)(VAL2);                                   \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 >= TP_val2,                                                \
            #VAL1 " was expected to be greater than or equal to " #VAL2,       \
            false, TP_val1, TP_val2, "%" PRIu64, uint64_t, __VA_ARGS__);       \
    } while (0)

// Signed integer comparison
#define ASSERT_INT_EQ(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(TP_val1 == TP_val2,                                 \
                           #VAL1 " and " #VAL2 " were expected to be equal",   \
                           true, TP_val1, TP_val2, "%" PRIi64, int64_t,        \
                           __VA_ARGS__);                                       \
    } while (0)

#define EXPECT_INT_EQ(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(TP_val1 == TP_val2,                                 \
                           #VAL1 " and " #VAL2 " were expected to be equal",   \
                           false, TP_val1, TP_val2, "%" PRIi64, int64_t,       \
                           __VA_ARGS__);                                       \
    } while (0)

#define ASSERT_INT_NE(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 != TP_val2,                                                \
            #VAL1 " and " #VAL2 " were expected to be different", true,        \
            TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);               \
    } while (0)

#define EXPECT_INT_NE(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 != TP_val2,                                                \
            #VAL1 " and " #VAL2 " were expected to be different", false,       \
            TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);               \
    } while (0)

#define ASSERT_INT_LT(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 < TP_val2, #VAL1 " was expected to be less than " #VAL2,   \
            true, TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);         \
    } while (0)

#define EXPECT_INT_LT(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 < TP_val2, #VAL1 " was expected to be less than " #VAL2,   \
            false, TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);        \
    } while (0)

#define ASSERT_INT_GT(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(TP_val1 > TP_val2,                                  \
                           #VAL1 " was expected to be greater than " #VAL2,    \
                           true, TP_val1, TP_val2, "%" PRIi64, int64_t,        \
                           __VA_ARGS__);                                       \
    } while (0)

#define EXPECT_INT_GT(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(TP_val1 > TP_val2,                                  \
                           #VAL1 " was expected to be greater than " #VAL2,    \
                           false, TP_val1, TP_val2, "%" PRIi64, int64_t,       \
                           __VA_ARGS__);                                       \
    } while (0)

#define ASSERT_INT_LE(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 <= TP_val2,                                                \
            #VAL1 " was expected to be less than or equal to " #VAL2, true,    \
            TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);               \
    } while (0)

#define EXPECT_INT_LE(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 <= TP_val2,                                                \
            #VAL1 " was expected to be less than or equal to " #VAL2, false,   \
            TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);               \
    } while (0)

#define ASSERT_INT_GE(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 >= TP_val2,                                                \
            #VAL1 " was expected to be greater than or equal to " #VAL2, true, \
            TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);               \
    } while (0)

#define EXPECT_INT_GE(VAL1, VAL2, ...)                                         \
    do {                                                                       \
        int64_t TP_val1 = (int64_t)(VAL1);                                     \
        int64_t TP_val2 = (int64_t)(VAL2);                                     \
        TP_BASE_COMPARISON(                                                    \
            TP_val1 >= TP_val2,                                                \
            #VAL1 " was expected to be greater than or equal to " #VAL2,       \
            false, TP_val1, TP_val2, "%" PRIi64, int64_t, __VA_ARGS__);        \
    } while (0)

// Pointer comparison
#define ASSERT_PTR_EQ(PTR1, PTR2, ...)                                         \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        TP_BASE_COMPARISON(TP_ptr1 == TP_ptr2,                                 \
                           #PTR1 " and " #PTR2 " were expected to be equal",   \
                           true, TP_ptr1, TP_ptr2, "%p", void *, __VA_ARGS__); \
    } while (0)

#define EXPECT_PTR_EQ(PTR1, PTR2, ...)                                         \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        TP_BASE_COMPARISON(TP_ptr1 == TP_ptr2,                                 \
                           #PTR1 " and " #PTR2 " were expected to be equal",   \
                           false, TP_ptr1, TP_ptr2, "%p", void *,              \
                           __VA_ARGS__);                                       \
    } while (0)

#define ASSERT_PTR_NE(PTR1, PTR2, ...)                                         \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        TP_BASE_COMPARISON(TP_ptr1 != TP_ptr2,                                 \
                           #PTR1 " and " #PTR2                                 \
                                 " were expected to be different",             \
                           true, TP_ptr1, TP_ptr2, "%p", void *, __VA_ARGS__); \
    } while (0)

#define EXPECT_PTR_NE(PTR1, PTR2, ...)                                         \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        TP_BASE_COMPARISON(                                                    \
            TP_ptr1 != TP_ptr2,                                                \
            #PTR1 " and " #PTR2 " were expected to be different", false,       \
            TP_ptr1, TP_ptr2, "%p", void *, __VA_ARGS__);                      \
    } while (0)

// String comparison
#define ASSERT_STR_EQ(STR1, STR2, ...)                                         \
    do {                                                                       \
        const char *TP_str1 = (const char *)STR1;                              \
        const char *TP_str2 = (const char *)STR2;                              \
        TP_BASE_COMPARISON(TP_str1 != NULL && TP_str2 != NULL &&               \
                               strcmp(TP_str1, TP_str2) == 0,                  \
                           #STR1 " and " #STR2 " were expected to be equal",   \
                           true, TP_str1, TP_str2, "'%s'", const char *,       \
                           __VA_ARGS__);                                       \
    } while (0)

#define EXPECT_STR_EQ(STR1, STR2, ...)                                         \
    do {                                                                       \
        const char *TP_str1 = (const char *)STR1;                              \
        const char *TP_str2 = (const char *)STR2;                              \
        TP_BASE_COMPARISON(TP_str1 != NULL && TP_str2 != NULL &&               \
                               strcmp(TP_str1, TP_str2) == 0,                  \
                           #STR1 " and " #STR2 " were expected to be equal",   \
                           false, TP_str1, TP_str2, "'%s'", const char *,      \
                           __VA_ARGS__);                                       \
    } while (0)

#define ASSERT_STR_NE(STR1, STR2, ...)                                         \
    do {                                                                       \
        const char *TP_str1 = (const char *)STR1;                              \
        const char *TP_str2 = (const char *)STR2;                              \
        TP_BASE_COMPARISON(TP_str1 != NULL && TP_str2 != NULL &&               \
                               strcmp(TP_str1, TP_str2) != 0,                  \
                           #STR1 " and " #STR2 " were expected to be equal",   \
                           true, TP_str1, TP_str2, "'%s'", const char *,       \
                           __VA_ARGS__);                                       \
    } while (0)

#define EXPECT_STR_NE(STR1, STR2, ...)                                         \
    do {                                                                       \
        const char *TP_str1 = (const char *)STR1;                              \
        const char *TP_str2 = (const char *)STR2;                              \
        TP_BASE_COMPARISON(TP_str1 != NULL && TP_str2 != NULL &&               \
                               strcmp(TP_str1, TP_str2) != 0,                  \
                           #STR1 " and " #STR2 " were expected to be equal",   \
                           false, TP_str1, TP_str2, "'%s'", const char *,      \
                           __VA_ARGS__);                                       \
    } while (0)

// Memory region comparison
#define ASSERT_MEM_EQ(PTR1, PTR2, SIZE, ...)                                   \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        size_t TP_size = (size_t)SIZE;                                         \
        TP_BASE_MEM_COMPARISON(                                                \
            memcmp(TP_ptr1, TP_ptr2, TP_size) == 0,                            \
            #PTR1 " and " #PTR2 " were expected to contain the same data",     \
            true, TP_ptr1, TP_ptr2, TP_size, __VA_ARGS__);                     \
    } while (0)

#define EXPECT_MEM_EQ(PTR1, PTR2, SIZE, ...)                                   \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        size_t TP_size = (size_t)SIZE;                                         \
        TP_BASE_MEM_COMPARISON(                                                \
            memcmp(TP_ptr1, TP_ptr2, TP_size) == 0,                            \
            #PTR1 " and " #PTR2 " were expected to contain the same data",     \
            false, TP_ptr1, TP_ptr2, TP_size, __VA_ARGS__);                    \
    } while (0)

#define ASSERT_MEM_NE(PTR1, PTR2, SIZE, ...)                                   \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        size_t TP_size = (size_t)SIZE;                                         \
        TP_BASE_MEM_COMPARISON(                                                \
            memcmp(TP_ptr1, TP_ptr2, TP_size) != 0,                            \
            #PTR1 " and " #PTR2 " were expected to contain the same data",     \
            true, TP_ptr1, TP_ptr2, TP_size, __VA_ARGS__);                     \
    } while (0)

#define EXPECT_MEM_NE(PTR1, PTR2, SIZE, ...)                                   \
    do {                                                                       \
        void *TP_ptr1 = (void *)(PTR1);                                        \
        void *TP_ptr2 = (void *)(PTR2);                                        \
        size_t TP_size = (size_t)SIZE;                                         \
        TP_BASE_MEM_COMPARISON(                                                \
            memcmp(TP_ptr1, TP_ptr2, TP_size) != 0,                            \
            #PTR1 " and " #PTR2 " were expected to contain the same data",     \
            false, TP_ptr1, TP_ptr2, TP_size, __VA_ARGS__);                    \
    } while (0)

// .--------------.
// | Other macros |
// '--------------'

#define SKIP(...)                                                              \
    do {                                                                       \
        TP_context.result.status = TP_TEST_SKIPPED;                            \
        TP_report_call_traces(0);                                              \
        TP_send_message(0, "" __VA_ARGS__);                                    \
        longjmp(TP_context.env, 1);                                            \
    } while (0)

#define FAIL(...)                                                              \
    do {                                                                       \
        TP_context.result.status = TP_TEST_FAILED;                             \
        TP_report_call_traces(0);                                              \
        TP_send_message(0, __FILE__ ":" TP_LINE_STR ": FAIL() invoked");       \
        TP_send_message(1, "" __VA_ARGS__);                                    \
        longjmp(TP_context.env, 1);                                            \
    } while (0)

#define SET_TEST_FAILURE_HANDLER(HANDLER, HANDLER_ARG)                         \
    do {                                                                       \
        if ((HANDLER) != NULL) {                                               \
            TP_context.fail_handler = HANDLER;                                 \
            TP_context.fail_handler_arg = HANDLER_ARG;                         \
        }                                                                      \
    } while (0)

#define TRACE_CALL(CALL)                                                       \
    do {                                                                       \
        TP_push_call_trace(__FILE__ ":" TP_LINE_STR ": " #CALL                 \
                                    " -- traced call");                        \
        CALL;                                                                  \
        TP_pop_call_trace();                                                   \
    } while (0)

//                          .-------------------.
// -------------------------| End of Public API |-------------------------------
//                          '-------------------'
#endif // TESTPREFIX_H_
