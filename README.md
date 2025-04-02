# xdbg

> [!note]
> This library is not complete and will be converted to a single header file once the required features are complete.

> [!warning]
>
> - **NOT THREAD SAFE!** (_yet_)
> - Exclusively developed and tested on MacOS M1 (_for now_)

- Memory Debugging – Detect leaks, corruption, overflows.
- Minimal Overhead – As light as possible for fast debugging.
- Toggle debugging

## TODO

- [ ] Add documentation
- [ ] Handle double frees
- [ ] Handle out of bounds pointer frees
- [x] Add `realloc` Wed Apr 2 16:08:31 EDT 2025
- [x] Add `calloc` Wed Apr 2 16:41:33 EDT 2025
- [ ] Add thread safety
- [ ] Add checks to match allocation and freeing of individual pointers by their address
- [ ] Add stack tracing (_maybe_)
- [ ] Make the library portable (usable in UNIX and Windows)

## Compiling with Debugging:

To enable the debugging, you can define `XDBG_ENABLE` during compilation, like so:

```bash
gcc -DXDBG_ENABLE -o my_program my_program.c
```

_or_

Include the following define in a source file:

```c
#define XDBG_ENABLE
#include "xdbg.h"
```

This setup keeps `xdbg` as lightweight as possible with macros that can be entirely stripped out when not needed.

## References

1. [malloc](https://en.cppreference.com/w/c/memory/malloc)
2. [realloc](https://en.cppreference.com/w/c/memory/realloc)
3. [calloc](https://en.cppreference.com/w/c/memory/calloc)
4. [free](https://en.cppreference.com/w/c/memory/free)
5. [What is a stack trace...?](https://stackoverflow.com/questions/3988788/what-is-a-stack-trace-and-how-can-i-use-it-to-debug-my-application-errors)
6. [stb_leakcheck.h](https://github.com/nothings/stb/blob/master/stb_leakcheck.h)
7. [forge.h](https://github.com/quelsolaar/MergeSource/blob/main/forge.h)
