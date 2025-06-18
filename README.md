# xdbg

A minimal memory debugger written in C for **educational and recreational purposes.**

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

- [forge memory debugging system](https://github.com/quelsolaar/MergeSource/blob/main/forge.h)
- [stb_leakcheck.h](https://github.com/nothings/stb/blob/master/stb_leakcheck.h)
- [Stack Canaries](https://ctf101.org/binary-exploitation/stack-canaries/)
- [stack-canaries-gingerly-sidestepping-the-cage](https://www.sans.org/blog/stack-canaries-gingerly-sidestepping-the-cage/)
- [stack-corruption-detection-using-canary-value](https://stackoverflow.com/questions/63024660/stack-corruption-detection-using-canary-value)
- [basics of stack canaries](https://cs4401.walls.ninja/notes/lecture/basics_canaries.html)
- [customizing the stack canary value](https://onlinedocs.microchip.com/oxy/GUID-BE362C91-6400-481F-9D86-10A1C40CA77C-en-US-5/GUID-F9DA6B9F-5799-463D-A8E3-1EBB368A4E68.html)
- [use compiler flags for stack protection in gcc and clang](https://developers.redhat.com/articles/2022/06/02/use-compiler-flags-stack-protection-gcc-and-clang#)
