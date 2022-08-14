# testprefix

![Demo](/doc/demo.gif?raw=true)

Test library with function discovery in C.

* Functions starting with a certain prefix are executed as tests (default prefix: `test_`);
* No special macros to declare or register test functions;
* No special treatment for setup/teardown, they are just functions;
* One single assert macro (`TP_ASSERT`);
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
