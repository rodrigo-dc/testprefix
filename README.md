# testprefix

Test library with function discovery in C.

* Functions whose names start with a certain prefix are automatically
  executed as tests functions. The default prefix is `test_`.
* No special test declaration or registration is needed.
* Console or TAP report.

## Limitations

* The test application must be a not-stripped ELF executable.

## Writing tests

Include `testprefix.h` and create a function named according to the prefix
you want to use. Then, use any of the assertion macros.

```c
void test_suite1_ok_integer_comparison()
{
    ASSERT_INT_NE(1 == 2, "they are different");
}
```

### Assertion macros

```c
ASSERT_TRUE(COND, ...)
```
```c
ASSERT_FALSE(COND, ...)
```
```c
ASSERT_UINT_EQ(VAL1, VAL2, ...)
```
```c
ASSERT_UINT_NE(VAL1, VAL2, ...)
```
```c
ASSERT_INT_EQ(VAL1, VAL2, ...)
```
```c
ASSERT_INT_NE(VAL1, VAL2, ...)
```
```c
ASSERT_PTR_EQ(PTR1, PTR2, ...)
```
```c
ASSERT_PTR_NE(PTR1, PTR2, ...)
```
```c
ASSERT_STR_EQ(STR1, STR2, ...)
```
```c
ASSERT_STR_NE(STR1, STR2, ...)
```
```c
ASSERT_MEM_EQ(PTR1, PTR2, SIZE, ...)
```
```c
ASSERT_MEM_NE(PTR1, PTR2, SIZE, ...)
```

All assertion macros can take a message as argument, which will be included
in the error message in case of assertion failure.

Example:

```c
ASSERT_TRUE(false, "iteration: %u", i);
```

### Skipping tests

To skip a test, use the macro `SKIP(...)` before any assertion macro.
Optionally, a string can be passed to `SKIP()`. This string will be part
of the report.

Example:

```c
SKIP("Skipped for some reason.");
```

### Releasing resources

Not implemented yet.

### Global setup/teardown functions

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

## Compiling

Copy `testprefix.c` and `testprefix.h` to your project. Build a test
application from `testprefix.c` and the other test sources.

Some POSIX-specific functions are used in `testprefix.c`. If possible,
specify `-std=gnu99` to build it cleanly.

## Running

Command line options:

```
Usage: ./test_app [-p PREFIX] [-l | -h | -o FILE]
    -p  Set the test function PREFIX. The default is 'test_'.
    -l  List the tests that match PREFIX.
    -h  Show this help message.
    -o  Write a test report to FILE (TAP format).
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

The new API (2.x) is heavily based on the feedback from @kholeg.
