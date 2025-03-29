# xdbg

**NOT THREAD SAFE!** (yet)

- Memory Debugging – Detect leaks, corruption, overflows.
- Minimal Overhead – As light as possible for fast debugging.
- Toggle debugging

## TODO

- [ ] Add documentation
- [ ] Add `realloc`
- [ ] Add `calloc`
- [ ] Add thread safety
- [ ] Add checks to match allocation and freeing of individual pointers by their address

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
