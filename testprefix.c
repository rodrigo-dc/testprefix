// Copyright 2021 Rodrigo Dias Correa. See LICENSE.

#include "testprefix.h"

#include <fcntl.h>
#include <getopt.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Maximum size for strings (except the error message)
#define STRING_SIZE_MAX 64

//
// ELF parsing
//
// Find out if the ELF file generated by the current compiler is
// 32 or 64 bits.
#if __SIZEOF_POINTER__ == 8
    #define ELF64 1
    #define EXPECTED_ELF_CLASS 2 // ELF64
#else
    #define EXPECTED_ELF_CLASS 1 // ELF32
#endif

#define ELF_MAGIC "\x7f""ELF"
#define ELF_MAGIC_SIZE 4
#define EI_NIDENT 16
#define SYM_TYPE_FUNCTION 2
#define ELF_CLASS_OFFSET 4
#define SEC_TYPE_SYMTAB 2
#define SEC_TYPE_STRTAB 3

#ifdef ELF64

struct elf_header {
    uint8_t e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct elf_sec_header {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

struct elf_sym {
    uint32_t st_name;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

#else // #ifdef ELF64

struct elf_header {
    uint8_t e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct elf_sec_header {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
};

struct elf_sym {
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
};
#endif // #ifdef ELF64

struct string_table {
    size_t size;
    char *strings;
};

static int read_elf_header(int fd, struct elf_header *header)
{
    lseek(fd, ELF_CLASS_OFFSET, SEEK_SET);

    char elf_class = 0;
    int ret = read(fd, &elf_class, 1);
    if (ret != 1) {
        dprintf(STDERR_FILENO, "Error. Could not read ELF class.\n");
        return -1;
    }

    if (elf_class != EXPECTED_ELF_CLASS) {
        dprintf(STDERR_FILENO, "Error. ELF header size mismatch.\n");
        dprintf(STDERR_FILENO, "Expected: %d, Actual: %d\n", EXPECTED_ELF_CLASS, elf_class);
        return -1;
    }

    lseek(fd, 0, SEEK_SET);

    int elf_header_size = (int)sizeof(struct elf_header);
    ret = read(fd, header, elf_header_size);
    if (ret != elf_header_size) {
        dprintf(STDERR_FILENO, "Error. Could not read ELF header.\n");
        return -1;
    }

    if (memcmp(header->e_ident, ELF_MAGIC, ELF_MAGIC_SIZE) != 0) {
        dprintf(STDERR_FILENO, "Error. ELF magic does not match.\n");
        dprintf(STDERR_FILENO, "magic: %x(%c) %x(%c) %x(%c) %x(%c)\n",
            header->e_ident[0], header->e_ident[0], header->e_ident[1], header->e_ident[1],
            header->e_ident[2], header->e_ident[2], header->e_ident[3], header->e_ident[3]);
        return -1;
    }

    return 0;
}

static int read_string_table(int fd,
                             struct elf_sec_header *sec_header,
                             struct string_table *table)
{
    table->size = sec_header->sh_size;
    table->strings = malloc(table->size);
    if (table->strings == NULL) {
        dprintf(STDERR_FILENO, "Error. Could not allocate memory.");
        return -1;
    }

    lseek(fd, sec_header->sh_offset, SEEK_SET);
    int ret = read(fd, table->strings, table->size);
    if (ret != (int)table->size) {
        dprintf(STDERR_FILENO, "Error. Could not read string section data.");
        free(table->strings);
        table->strings = NULL;
        table->size = 0;
        return -1;
    }

    return 0;
}

static void destroy_string_table(struct string_table *str_table)
{
    if (str_table == NULL) {
        return;
    }

    if (str_table->strings == NULL) {
        return;
    }

    free(str_table->strings);
    str_table->strings = NULL;
    str_table->size = 0;
}

static int read_sec_header_at_index(int fd,
                                    struct elf_header *header,
                                    uint16_t index,
                                    struct elf_sec_header *sec_header)
{
    if (index >= header->e_shnum) {
        return -1;
    }

    off_t sec_header_off = header->e_shoff + (index * header->e_shentsize);
    lseek(fd, sec_header_off, SEEK_SET);

    int ret = read(fd, sec_header, header->e_shentsize);
    if (ret != header->e_shentsize) {
        dprintf(STDERR_FILENO, "Error. Could not read section header.\n");
        return -1;
    }

    return 0;
}

static int read_next_symtab_header(int fd,
                                   struct elf_header *header,
                                   uint16_t *index,
                                   struct elf_sec_header *symtab_header)
{
    while (read_sec_header_at_index(fd, header, (*index)++, symtab_header) == 0) {
        if (symtab_header->sh_type == SEC_TYPE_SYMTAB) {
            return 0;
        }
    }
    return -1;
}

//
// Test reporters
//
enum test_status {
    TEST_PASSED,
    TEST_FAILED,
    TEST_SKIPPED
};

struct test_result {
    enum test_status status;
    struct timespec begin;
    struct timespec end;
    char err_msg[TP_MAX_MSG_SIZE];
};

// Interface that must be implemented by a test reporter.
struct test_reporter {
    // Reporter's private data.
    void *private;
    // Called once, before any test execution.
    int (*init_cb)(struct test_reporter *self, int test_count);
    // Called before each test execution.
    int (*test_begin_cb)(int index,
                         const char *name,
                         void *private);
    // Called after each test execution.
    int (*test_end_cb)(int index,
                       const char *name,
                       const struct test_result *result,
                       void *private);
    // Called once, after all test executions. Any allocated resource
    // must be released here.
    int (*finish_cb)(struct test_reporter *self);
};

//
// Console Reporter
//
#define FG_GREEN "\x1b[32m"
#define FG_RED "\x1b[31m"
#define FG_YELLOW "\x1b[33m"
#define FG_BOLD "\x1b[1m"
#define TERM_RESET "\x1b[0m"

#define CONST_STR(code, str) isatty(STDOUT_FILENO) ? code str TERM_RESET : str

#define BOLD_CONST(str) CONST_STR(FG_BOLD, str)
#define GREEN_CONST(str) CONST_STR(FG_GREEN, str)
#define RED_CONST(str) CONST_STR(FG_RED, str)
#define YELLOW_CONST(str) CONST_STR(FG_YELLOW, str)

struct console_private {
    int test_count;
    int success_counter;
    int failure_counter;
    int skip_counter;
};

static long elapsed_time_ms(const struct timespec *begin, const struct timespec *end)
{
    long begin_ms = (begin->tv_sec * 1000) + (begin->tv_nsec / 1000000);
    long end_ms = (end->tv_sec * 1000) + (end->tv_nsec / 1000000);

    return end_ms - begin_ms;
}

static int console_init_cb(struct test_reporter *self, int test_count)
{
    struct console_private *cp = (struct console_private *)self->private;

    cp->test_count = test_count;
    cp->success_counter = 0;
    cp->failure_counter = 0;
    cp->skip_counter = 0;

    return 0;
}

static int console_test_begin_cb(int index,
                                 const char *name,
                                 void *private)
{
    (void)private;

    if (isatty(STDOUT_FILENO)) {
        printf("[ TEST ] %d " FG_BOLD " %s " TERM_RESET "\n", index, name);
    } else {
        printf("[ TEST ] %d %s\n", index, name);
    }
    return 0;
}

static int console_test_end_cb(int index,
                               const char *name,
                               const struct test_result *result,
                               void *private)
{
    (void)name;

    char *result_string;
    struct console_private *cp = (struct console_private*)private;

    if (result->status == TEST_PASSED) {
        cp->success_counter++;
        result_string = GREEN_CONST("PASS");
    } else if (result->status == TEST_SKIPPED) {
        cp->skip_counter++;
        result_string = YELLOW_CONST("SKIP");
    } else {
        cp->failure_counter++;
        result_string = RED_CONST("FAIL");
    }

    printf("[ %s ] %d | %ld ms", result_string, index,
        elapsed_time_ms(&result->begin, &result->end));

    if (result->err_msg[0] != '\0') {
        printf(" | %s", result->err_msg);
    }
    printf("\n");

    return 0;
}

static int console_finish_cb(struct test_reporter *self)
{
    struct console_private *cp = (struct console_private*)self->private;

    printf("\n%s: %d", BOLD_CONST("  Total"), cp->test_count);
    printf("\n%s: %d", BOLD_CONST(" Passed"), cp->success_counter);
    printf("\n%s: %d", BOLD_CONST(" Failed"), cp->failure_counter);
    printf("\n%s: %d\n", BOLD_CONST("Skipped"), cp->skip_counter);

    return 0;
}

static void setup_console_reporter(struct console_private *priv, struct test_reporter *r)
{
    memset(r, 0, sizeof(struct test_reporter));
    memset(priv, 0, sizeof(struct console_private));

    r->init_cb = console_init_cb;
    r->test_begin_cb = console_test_begin_cb;
    r->test_end_cb = console_test_end_cb;
    r->finish_cb = console_finish_cb;

    r->private = priv;
}

//
// TAP Reporter
//
struct tap_private {
    int fd;
    char path[STRING_SIZE_MAX];
};

static int tap_init_cb(struct test_reporter *self, int test_count)
{
    struct tap_private *tp = (struct tap_private *)self->private;

    int fd = open(tp->path, O_CREAT | O_TRUNC | O_RDWR,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd < 0) {
        dprintf(STDERR_FILENO, "Error. Unable to open '%s'\n", tp->path);
        return -1;
    }

    tp->fd = fd;
    dprintf(tp->fd, "TAP version 13\n");
    dprintf(tp->fd, "1..%d\n", test_count);

    return 0;
}

static int tap_test_begin_cb(int index,
                             const char *name,
                             void *private)
{
    (void)index;
    (void)name;
    (void)private;

    return 0;
}

static int tap_test_end_cb(int index,
                           const char *name,
                           const struct test_result *result,
                           void *private)
{
    struct tap_private *tp = (struct tap_private*)private;

    if (result->status == TEST_FAILED) {
        dprintf(tp->fd, "not ");
    }

    dprintf(tp->fd, "ok %d %s", index + 1, name);
    if (result->status == TEST_SKIPPED) {
        dprintf(tp->fd, " # skipped");
    } else if (result->status == TEST_FAILED) {
        dprintf(tp->fd, "\n  ---\n");
        dprintf(tp->fd, "  message: \"%s\"\n", result->err_msg);
        dprintf(tp->fd, "  ...");
    }
    dprintf(tp->fd, "\n");

    return 0;
}

static int tap_finish_cb(struct test_reporter *self)
{
    struct tap_private *tp = (struct tap_private*)self->private;
    if (tp->fd > 0) {
        close(tp->fd);
        tp->fd = -1;
    }

    return 0;
}

static void setup_tap_reporter(const char *path, struct tap_private *priv, struct test_reporter *r)
{
    memset(r, 0, sizeof(struct test_reporter));
    memset(priv, 0, sizeof(struct tap_private));

    r->init_cb = tap_init_cb;
    r->test_begin_cb = tap_test_begin_cb;
    r->test_end_cb = tap_test_end_cb;
    r->finish_cb = tap_finish_cb;

    strncpy(priv->path, path, STRING_SIZE_MAX);
    r->private = priv;
}

//
// Test discovery
//
#define FUNC_PREFIX_DEFAULT "test_"

int main(int argc, char *argv[]);
typedef int (*testfunc)(void *);

struct test_info {
    testfunc func;
    char name[STRING_SIZE_MAX];
};

static int compar_test_info(const void* a, const void *b)
{
    const struct test_info *a_ti = (const struct test_info*)a;
    const struct test_info *b_ti = (const struct test_info*)b;

    return strcmp(a_ti->name, b_ti->name);
}

static int read_symbols(int fd,
                        struct elf_sec_header *symtab_header,
                        struct string_table str_table,
                        const char *prefix,
                        bool count_only,
                        struct test_info *tests)
{
    int test_count = 0;
    unsigned long addr_offset = 0;
    bool offset_found = false;
    int symbol_count = symtab_header->sh_size / symtab_header->sh_entsize;

    lseek(fd, symtab_header->sh_offset, SEEK_SET);

    for (int i = 0; i < symbol_count; i++) {
        struct elf_sym sym;
        int ret = read(fd, &sym, symtab_header->sh_entsize);
        if (ret != (int)symtab_header->sh_entsize) {
            dprintf(STDERR_FILENO, "Error. Could not read symbol.");
            break;
        }
        if ((sym.st_info & 0xf) != SYM_TYPE_FUNCTION) {
            continue;
        }
        uint32_t str_index = sym.st_name;
        if (str_index >= str_table.size) {
            dprintf(STDERR_FILENO, "Error. Invalid string index.\n");
            break;
        }
        size_t prefix_len = strnlen(prefix, STRING_SIZE_MAX);
        if (strncmp(&str_table.strings[str_index], prefix, prefix_len) != 0) {
            if (!count_only && !offset_found) {
                if (strcmp(&str_table.strings[str_index], "main") == 0) {
                    // To compensate the relocation in a Position Independent Executable,
                    // we calculated an address offset based on 'main'.
                    addr_offset = (unsigned long)(main - sym.st_value);
                    offset_found = true;
                }
            }
            continue;
        }
        if (!count_only) {
            tests[test_count].func = (testfunc)sym.st_value;
            strncpy(tests[test_count].name, &str_table.strings[str_index], STRING_SIZE_MAX);
            tests[test_count].name[STRING_SIZE_MAX - 1] = '\0';
        }
        test_count++;
    }
    if (!count_only && addr_offset != 0) {
        for (int i = 0; i < test_count; i++) {
            tests[i].func += addr_offset;
        }
    }

    return test_count;
}

static int create_test_list(int fd,
                            struct elf_header *header,
                            struct elf_sec_header *symtab_header,
                            const char *prefix,
                            struct test_info **tests)
{
    int test_count = 0;

    struct elf_sec_header sec_header;
    int ret = read_sec_header_at_index(fd, header, symtab_header->sh_link, &sec_header);
    if (ret != 0) {
        dprintf(STDERR_FILENO, "Error. Could not read string table header.\n");
        return -1;
    }

    struct string_table str_table;
    ret = read_string_table(fd, &sec_header, &str_table);
    if (ret != 0) {
        dprintf(STDERR_FILENO, "Error. Could not read string table.\n");
        return -1;
    }

    test_count = read_symbols(fd, symtab_header, str_table, prefix, true, NULL);
    if (test_count < 1) {
        destroy_string_table(&str_table);
        return test_count;
    }

    *tests = malloc(sizeof(struct test_info) * test_count);
    if (*tests == NULL) {
        destroy_string_table(&str_table);
        dprintf(STDERR_FILENO, "Error. Could not read string table.\n");
        return -1;
    }

    read_symbols(fd, symtab_header, str_table, prefix, false, *tests);
    qsort(*tests, test_count, sizeof(struct test_info), compar_test_info);

    destroy_string_table(&str_table);

    return test_count;
}

static int find_tests(int fd, const char *prefix, struct test_info **tests)
{
    struct elf_header header;
    int ret = read_elf_header(fd, &header);
    if (ret != 0) {
        return -1;
    }

    struct elf_sec_header symtab_header;
    uint16_t index = 0;
    ret = read_next_symtab_header(fd, &header, &index, &symtab_header);
    if (ret != 0) {
        return -1;
    }

    return create_test_list(fd, &header, &symtab_header, prefix, tests);
}

//
// CLI arguments
//
struct cli_args {
    bool print_usage;
    bool print_test_names;
    char prefix[STRING_SIZE_MAX];
    char output_path[STRING_SIZE_MAX];
};

static void print_usage(char *argv[])
{
    printf("Usage: %s [-p PREFIX] [-l | -h | -o FILE]\n", argv[0]);
    printf("    -p  Set the test function PREFIX. The default is 'test_'.\n");
    printf("    -l  List the tests that match PREFIX.\n");
    printf("    -h  Show this help message.\n");
    printf("    -o  Write a test report to FILE (TAP format).\n");
}

static int parse_args(int argc, char *argv[], struct cli_args *args)
{
    int opt;

    args->print_usage = false;
    args->print_test_names = false;
    strcpy(args->prefix, FUNC_PREFIX_DEFAULT);
    args->output_path[0] = '\0';

    while ((opt = getopt(argc, argv, "p:hlo:")) != -1) {
        switch (opt) {
        case 'p':
            strncpy(args->prefix, optarg, sizeof(args->prefix) - 1);
            break;
        case 'h':
            args->print_usage = true;
            break;
        case 'l':
            args->print_test_names = true;
            break;
        case 'o':
            strncpy(args->output_path, optarg, sizeof(args->output_path) - 1);
            break;
        default:
            return -1;
        }
    }

    return 0;
}

//
// Test execution
//
static int run_tests(int test_count, struct test_info *tests, struct test_reporter *r)
{
    int ret = r->init_cb(r, test_count);
    if (ret != 0) {
        dprintf(STDERR_FILENO, "Error. Unable to initialize reporter.\n");
        return -1;
    }

    for (int i = 0; i < test_count; i++) {
        struct test_result result = {0};
        struct test_context ctx = {
            .err_msg = result.err_msg
        };

        clock_gettime(CLOCK_REALTIME, &result.begin);
        ret = setjmp(ctx.env);
        if (ret == 0) {
            r->test_begin_cb(i, tests[i].name, r->private);
            int test_ret = tests[i].func(&ctx);
            if (test_ret == 0) {
                result.status = TEST_PASSED;
            } else {
                result.status = TEST_SKIPPED;
            }
        } else {
            result.status = TEST_FAILED;
        }
        clock_gettime(CLOCK_REALTIME, &result.end);
        r->test_end_cb(i, tests[i].name, &result, r->private);
    }
    r->finish_cb(r);

    return 0;
}

static void print_test_names(int test_count, struct test_info *tests)
{
    for (int i = 0; i < test_count; i++) {
        printf("    - %s\n", tests[i].name);
    }

    printf("\n%s: %d\n", BOLD_CONST("Total"), test_count);
}

int main(int argc, char *argv[])
{
    struct cli_args args;
    int ret = parse_args(argc, argv, &args);
    if (ret < 0 || args.print_usage) {
        print_usage(argv);
        return ret;
    }

    int fd = open(argv[0], O_RDONLY);
    if (fd < 0) {
        dprintf(STDERR_FILENO, "Error. Could not open '%s'\n", argv[0]);
        return -1;
    }

    struct test_info *tests = NULL;
    int test_count = find_tests(fd, args.prefix, &tests);
    if (test_count < 1) {
        dprintf(STDERR_FILENO, "No tests found.\n");
        close(fd);
        return -1;
    }

    if (args.print_test_names) {
        print_test_names(test_count, tests);
    } else {
        struct test_reporter reporter;
        struct console_private console_priv;
        struct tap_private tap_priv;
        if (strlen(args.output_path) > 0) {
            setup_tap_reporter(args.output_path, &tap_priv, &reporter);
        } else {
            setup_console_reporter(&console_priv, &reporter);
        }
        ret = run_tests(test_count, tests, &reporter);
    }

    free(tests);
    close(fd);

    return ret;
}

