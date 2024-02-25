# testprefix

Test library with function discovery in C.

* Functions with names starting with a certain prefix are automatically
  executed as tests functions. The default prefix is `test_`.
* No special test declaration or registration is needed.
* Console or TAP report.

## Limitations

* The test application must be a not-stripped ELF executable.

## Creating a simple test

Create a test file, for example, `test.c`. In the test file, include
`testprefix.h` and create a test function. The name of the test function
must start with the prefix you want to use. For the example below, we will
use the default prefix, `test_`.

```c
#include "testprefix.h"

void test_that_will_fail()
{
    ASSERT_TRUE(1 == 2, "one is not equal to two");
}
```

[Here](#ASSERT-and-EXPECT-macros) you can find the `ASSERT_` and `EXPECT_`
macros available.

Now you need to create an ELF executable from `testprefix.c`, the test file and
your source code.

> [!NOTE]
> `testprefix.c` defines a `main()` function, so you need to keep your original
> `main()` out of the test binary.

```shell
# For a real test, you will want to add other source codes here
gcc -std=gnu99 testprefix.c test.c -o test_app
```

Execute the test application.

```shell
./test_app
```
The console output looks like this:

![image](https://github.com/rodrigo-dc/testprefix/assets/7612217/8ffed09d-5149-4e11-8cf1-011ef34db094)

## `ASSERT` and `EXPECT` macros

When an `ASSERT_` macro fails, the test function is abandoned and marked as a
failure.
When an `EXPECT_` macro fails, the execution of the test function continues,
despite being marked as a failure.

```c
// Boolean tests
ASSERT_TRUE(COND, ...)
EXPECT_TRUE(COND, ...)
ASSERT_FALSE(COND, ...)
EXPECT_FALSE(COND, ...)

// Compare unsigned values
ASSERT_UINT_EQ(VAL1, VAL2, ...)
EXPECT_UINT_EQ(VAL1, VAL2, ...)
ASSERT_UINT_NE(VAL1, VAL2, ...)
EXPECT_UINT_NE(VAL1, VAL2, ...)

// Compare signed values
ASSERT_INT_EQ(VAL1, VAL2, ...)
EXPECT_INT_EQ(VAL1, VAL2, ...)
ASSERT_INT_NE(VAL1, VAL2, ...)
EXPECT_INT_NE(VAL1, VAL2, ...)

// Compare pointer values
ASSERT_PTR_EQ(PTR1, PTR2, ...)
EXPECT_PTR_EQ(PTR1, PTR2, ...)
ASSERT_PTR_NE(PTR1, PTR2, ...)
EXPECT_PTR_NE(PTR1, PTR2, ...)

// Compare the content of null-terminated strings
ASSERT_STR_EQ(STR1, STR2, ...)
EXPECT_STR_EQ(STR1, STR2, ...)
ASSERT_STR_NE(STR1, STR2, ...)
EXPECT_STR_NE(STR1, STR2, ...)

// Compare memory regions
ASSERT_MEM_EQ(PTR1, PTR2, SIZE, ...)
EXPECT_MEM_EQ(PTR1, PTR2, SIZE, ...)
ASSERT_MEM_NE(PTR1, PTR2, SIZE, ...)
EXPECT_MEM_NE(PTR1, PTR2, SIZE, ...)
```

> [!TIP]
> All `ASSERT_` and `EXPECT_` macros can take a message as argument, which will
> be included in the error message in case of failure.
> ```c
> ASSERT_TRUE(false, "iteration: %u", i);
> ```

## Skipping tests

To skip a test, use the macro `SKIP(...)` before any other macro.
Optionally, a string can be passed to `SKIP()`. This string will be part
of the report.

```c
SKIP("Skipped for some reason.");
```

## Releasing resources on test failures

When an `ASSERT_` macro fails, the test is aborted immediately. To avoid
resource leaks, it's possible to register a failure handler for a specific test.
The failure handler is a function with following format:

```c
void handler_function(void *);
```

To set the failure handler, use the macro `SET_TEST_FAILURE_HANDLER(HANDLER, HANDLER_ARG)`.

When `HANDLER` is invoked, `HANDLER_ARG` is passed as argument. Here is
an example where `HANDLER_ARG` is a `FILE *`.

```c
// Custom function that closes a FILE
void file_closer(void *ptr)
{
    FILE *stream = (FILE *)ptr;
    if (stream != NULL) {
        (void)fclose(stream);
    }
}

void test_something()
{
    FILE *stream = tmpfile();
    ASSERT_PTR_NE(stream, NULL);

    SET_TEST_FAILURE_HANDLER(file_closer, stream);
    ASSERT_TRUE(false);
}
```

If you just want to deallocate one pointer, you can simply use `free` as
your error handler.

```c
void test_something_else()
{
    uint8_t *buf = malloc(10);

    SET_TEST_FAILURE_HANDLER(free, buf);
    // If this fails, `free` will be invoked with `buf` as argument.
    ASSERT_TRUE(...);
}
```

## Global setup/teardown functions

Optionally, global setup and teardown functions can be defined.

The global setup function is executed once, before the first test. If its
return value is not `0`, the test application exits immediately without
invoking `TP_global_teardown`.

```c
// Global setup prototype
int TP_global_setup();
```

The global teardown function is executed once, after the last test.

```c
// Global teardown prototype
void TP_global_teardown()
```

## Build issues

Some POSIX-specific functions are used in `testprefix.c`. If possible,
specify `-std=gnu99` to build it cleanly.

## CLI interface

When executed with no arguments, the test application executes all tests
starting with `test_` and prints the results to the standard output.

The following options are available to change this default behavior.

```
Usage: ./test_app [-p PREFIX] [-l | -h | -o FILE]
    -p  Set the test function PREFIX. The default is 'test_'.
    -l  List the tests that match PREFIX.
    -h  Show this help message.
    -o  Write a test report to FILE (TAP format).
    -v  Verbose (do not silence stdout/stderr).
```

Examples:

```shell
./test_app # Run all tests
```
```shell
./test_app -o output_tap.txt # Write the results to a file (TAP format)
```
```shell
./test_app -p test_suite1_ # Execute test functions which names start with 'test_suite1_'
```
```shell
./test_app -l # Print the name of all test functions
```

## Credits

I would like to thank [@kholeg](https://github.com/KhOleg). Most of the changes
in the current API were based on the feedback from him.

