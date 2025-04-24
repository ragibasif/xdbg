# xdbg

`xdbg` is a debugger for dynamically allocated memory that tracks and logs every `malloc`, `realloc`, `calloc`, and `free` and displays them to standard output to help find memory leaks, memory corruption, overflows.

- Memory Debugging – Detect leaks, corruption, overflows.
- Minimal Overhead – As light as possible for fast debugging.
- Toggle debugging
- Track the file, line, and function where memory was allocated.
- Logs and reports each action: `malloc`, `realloc`, `calloc`, and `free`.

## Usage

Define `XDBG_ENABLE` and then include `xdbg.h` in one C/C++ source file.

```c
#define XDBG_ENABLE
#include "xdbg.h"
```
