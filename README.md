# xdbg

`xdbg` is a dynamically allocated memory debugger that tracks and logs every `malloc`, `realloc`, `calloc`, and `free` and displays them to standard output to help find memory leaks, memory corruption, overflows.

> [!note]
> This library is mostly complete. By that I mean the required and intended functionalities are implemented, however there are still some bugs that need to be fixed.

> [!warning]
>
> - RIDDLED WITH BUGS!!!
> - **NOT THREAD SAFE!** (_yet_)
> - Exclusively developed and tested on MacOS M1 (_for now_)

- Memory Debugging – Detect leaks, corruption, overflows.
- Minimal Overhead – As light as possible for fast debugging.
- Toggle debugging
- Track the file, line, and function where memory was allocated.
- Logs and reports each action: `malloc`, `realloc`, `calloc`, and `free`.
- Uses a linked lists to maintain order of allocation.

## TODO

- [ ] Implement hash table to detect double frees.
- [ ] Add canary values before/after blocks to detect memory corruption
- [ ] Add documentation
- [ ] Handle double frees
- [ ] Handle out of bounds pointer frees
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
