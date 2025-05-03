# xdbg

An extremely minimal memory debugger written in standard C (C99) and uses GNU Make for building. The goal of this project is to provide a very lightweight memory debugging library that can be included in any C/C++ project to aid in debugging dynamic memory allocation. It works by defining macros that replace calls to `malloc`, `realloc`, `calloc`, and `free` and tracking and logging them to be able to catch memory leaks.

## 🛠️ Usage

Clone the repository.

```bash
git clone git@github.com:ragibasif/xdbg.git
```

Place `xdbg.h` and `xdbg.c` in your project folder and include the header file.

```c
#define XDBG_ENABLE
#include "xdbg.h"
```

## 🪪 License

This project is licensed under the **MIT license**. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

Heavily inspired by [stb_leakcheck.h](https://github.com/nothings/stb/blob/master/stb_leakcheck.h) and [forge.h](https://github.com/quelsolaar/MergeSource/blob/main/forge.h).
