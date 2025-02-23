# testprefix

`testprefix` is a unit testing library for C that is easy to integrate
and easy to use.

## Quick start 

1. Copy *testprefix.c* and *testprefix.h* to your project;
2. In a separate file, include *testprefix.h* and create a function with
   a name starting with `test_`;

```c
#include "testprefix.h"
void test_something()
{
    int ret = something();
    ASSERT_INT_EQ(ret, 0);
}
```
3. Build *testprefix.c* together with the files including the tests and
   the code being tested;
```bash
gcc testprefix.c test.c something.c -o test
```

4. Run the test application;
```bash
./test
```

## Demo

![demo](https://ro.drigo.nl/testprefix/images/demo.gif)

* Functions with names starting with `test_` are automatically executed
  as test functions;
* Console or TAP report can be generated.


[Documentation](https://ro.drigo.nl/testprefix)

## Limitations

* The test application must be a not-stripped ELF executable.

