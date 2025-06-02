# xdbg

WIP!

> [!WARNING]
> This project is intended for experienced C programmers. As such, no build instructions, usage examples, or run-time documentation are provided. Users are expected to be comfortable reading and understanding C code, and capable of integrating or adapting the code to their own build systems and use cases.

An extremely minimal dynamic memory debugger written in C. The goal of this project is to provide a very lightweight memory debugging library that can be included in any C/C++ project to aid in debugging dynamic memory allocation. It works by defining macros that replace calls to `malloc`, `realloc`, `calloc`, and `free` and tracking and logging them to be able to catch memory leaks.

![example](./assets/example.gif)

## Usage

Clone the repository.

```bash
git clone git@github.com:ragibasif/xdbg.git
```

Place `xdbg.h` and `xdbg.c` in your project folder and include the header file.

```c
#define XDBG_ENABLE
#include "xdbg.h"
```

## License

This project is licensed under the **MIT license**. See the [LICENSE](LICENSE)
file for details.

## Acknowledgements

- [Rust's dbg macro](https://doc.rust-lang.org/std/macro.dbg.html)
- [dbg-macro for C++](https://github.com/sharkdp/dbg-macro?tab=readme-ov-file)
- [dbg-macro for C](https://github.com/eerimoq/dbg-macro)
- [stb_leakcheck.h](https://github.com/nothings/stb/blob/master/stb_leakcheck.h)
- [forge memory debugging system](https://github.com/quelsolaar/MergeSource/blob/main/forge.h)
