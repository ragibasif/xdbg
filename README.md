<div align="center">

# XDBG

</div>

<details open="open">
<summary>Table of Contents</summary>

- [XDBG](#xdbg)
  - [🧠 About](#-about)
  - [📋 TODO](#-todo)
  - [📁 Directory Structure](#-directory-structure)
  - [🛠️ Usage](#️-usage)
    - [💻 Installation](#-installation)
    - [🔧 Build the project](#-build-the-project)
    - [🧹 Clean build files](#-clean-build-files)
    - [🏃 Run the executable](#-run-the-executable)
    - [📥 Use in your projects:](#-use-in-your-projects)
  - [📄 Dependencies](#-dependencies)
  - [📚 Documentation](#-documentation)
  - [🙌 Contributing](#-contributing)
  - [🪪 License](#-license)
  - [🙏 References](#-references)

</details>

## 🧠 About

`xdbg` is a debugger for dynamically allocated memory that tracks and logs every `malloc`, `realloc`, `calloc`, and `free` and displays them to standard output to help find memory leaks, memory corruption, overflows.

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

## 📋 TODO

- [ ] Implement hash table to detect double frees.
- [ ] Add canary values before/after blocks to detect memory corruption
- [ ] Add documentation (with doxygen?)
- [ ] Handle double frees
- [ ] Handle out of bounds pointer frees
- [ ] Add thread safety
- [ ] Add checks to match allocation and freeing of individual pointers by their address
- [ ] Add stack tracing (_maybe_)
- [ ] Make the library portable (usable in UNIX and Windows)

## 📁 Directory Structure

```bash
xdbg/
├── Makefile        # Build system
├── README.md       # This file
└── xdbg.h          # Header file
└── xdbg.c          # Source file
```

## 🛠️ Usage

### 💻 Installation

Clone the repository.

```bash
git clone git@github.com:ragibasif/xdbg.git
```

Make sure you have `gcc` and `make` installed.

### 🔧 Build the project

```bash
make
```

### 🧹 Clean build files

```bash
make clean
```

### 🏃 Run the executable

```bash
./xdbg
```

### 📥 Use in your projects:

Make sure your project has access to `xdbg.h` and `xdbg.c`.

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

## 📄 Dependencies

This project uses:

- C99 Standard
- GNU Make

## 📚 Documentation

- TODO

## 🙌 Contributing

1. Fork the repository
2. Create a new branch (`git checkout -b feature/feature-name`)
3. Make your changes and commit (`git commit -am 'Add new feature'`)
4. Push to your fork (`git push origin feature/feature-name`)
5. Open a pull request

## 🪪 License

This project is licensed under the **MIT license**. See the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgements

- [stb](https://github.com/nothings/stb/blob/master/stb_leakcheck.h)
- [forge](https://github.com/quelsolaar/MergeSource/blob/main/forge.h)
