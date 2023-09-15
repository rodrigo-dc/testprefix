# testprefix

![Demo](/doc/demo.gif?raw=true)

Test library with function discovery in C.

* Functions starting with a certain prefix are executed as tests (default prefix: `test_`);
* No special macros to declare or register test functions;
* No special treatment for setup/teardown, they are just functions;
* Console or TAP output.

## Limitations

* It only works with ELF executables;
* You can't strip your test application. `testprefix` uses the symbol table.

## Usage

### Write a test

Include `testprefix.h`. Write a function that returns an `int` and takes a `void *` as parameter.
Name your function according to the prefix you want to use. The default is `test_`.
Make assertions using `TP_ASSERT` macro.

```c
int test_suite1_nok_integer_comparison(void *t)
{
    TP_ASSERT(t, 1 == 2);
    return 0;
}
```

Return `0` to indicate the test has passed.
Return other values to indicate the test was skipped.
The test fails in case of any assertion failure.

You can call `TP_ASSERT` from the inside of a setup-like function. You only need to
pass the `void *` parameter received by the test function.

#### Releasing resources

If ``TP_ASSERT`` fails, the test is aborted. To avoid resource leaks, ``TP_ASSERT``
takes optional parameters that can be used to deallocate memory, close files,
or even print additional information.

```c
TP_ASSERT(t, 1 == 2, free(ptr), printf("Released"));
```

#### Additional assert macros

`TP_ASSERT` adds only the conditional statement to the error message.
The following macros add actual values to the error messages, making the
debug process easier.

```c
int a = 4;
int b = 3;
// These macros add the values to the error message.
// The format string is mandatory.
TP_ASSERT_EQ(t, a, b, "%d");
TP_ASSERT_NE(t, a, b, "0x%x");
```

```c
uint8_t a1[] = {1, 2, 3, 4};
uint8_t a2[] = {1, 2, 33, 4};
// Compares two memory regions using `memcmp`.
// This macro adds the value of the first different value.
TP_ASSERT_MEM_EQ(t, a1, a2, sizeof(a1));
```

### Global setup/teardown functions

Optionally, setup and teardown functions can be defined.

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

### Run

Copy `testprefix.c` and `testprefix.h` to your project. Build a test application from
`testprefix.c` and the other test sources.

Execute the test application as in one of the examples bellow.

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
