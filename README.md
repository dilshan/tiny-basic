# Tiny Basic for Arduino

Tiny Basic for Arduino is a lightweight, fast, and fully featured [BASIC](https://en.wikipedia.org/wiki/BASIC) interpreter designed for mid-range Arduino boards and other microcontroller targets. It lets you write, edit, and run BASIC programs directly on hardware, making it a practical choice for education, rapid prototyping, and embedded experimentation.

## Overview

This project brings the familiar simplicity of BASIC to embedded development. With an in-hardware editor, a broad set of built-in functions, and support for multiple targets, Tiny Basic for Arduino is well suited for:

- Teaching programming and embedded concepts.
- Prototyping small control and automation projects.
- Running BASIC scripts on supported Arduino boards.
- Exploring MCU programming without needing a full desktop application / compiler.

## Features

- In-hardware editor for writing and editing BASIC programs directly on the device.
- Support for common BASIC language constructs and control flow.
- Built-in functions for math, I/O, bit manipulation, and timing.
- Portable design that can be adapted to different MCU targets.
- Native build support for local testing and debugging.

## Getting Started

### Prerequisites

To build the project, you will need:

- Python 3
- PlatformIO
- `bison` and `yacc`
- A C/C++ build toolchain such as `build-essential`.

On Debian/Ubuntu-based systems, install the required tools with:

```bash
sudo apt-get update
sudo apt-get install yacc bison build-essential
pip install -U platformio
```

### Build for Arduino Due

The default environment targets the Arduino Due:

```bash
pio run -e due
```

To upload the firmware to a connected board:

```bash
pio run -e due -t upload
```

### Build for the native host

For local testing and debugging on a desktop machine:

```bash
pio run -e native
./tinybasic.sh
```

### First program

Once the interpreter is running on your board, you can start with a simple program such as:

```basic
10 PRINT("HELLO, WORLD!")
20 END
```

Run it with:

```basic
RUN
```

## Documentation

The project documentation is available online at:

- [Tiny Basic for Arduino homepage](http://dilshan.github.io/tiny-basic/)
- [Getting started guide](http://dilshan.github.io/tiny-basic/guide/)
- [Building and flashing](http://dilshan.github.io/tiny-basic/guide/building/)
- [Hello World example](http://dilshan.github.io/tiny-basic/guide/hello-world/)
- [User manual](http://dilshan.github.io/tiny-basic/manual/)
- [Language reference](http://dilshan.github.io/tiny-basic/manual/language-reference/)
- [Editor guide](http://dilshan.github.io/tiny-basic/manual/editor/)
- [Function reference](http://dilshan.github.io/tiny-basic/manual/function-list/)

## Contributing

Contributions are welcome. To contribute:

1. Fork the repository.
2. Create a feature or bugfix branch.
3. Make your changes and test them.
4. Submit a pull request with a **clear description of the update**.

If you find a bug or have a feature request, please open an issue on the GitHub repository.

## License

This project is licensed under the [BSD 3-Clause License](LICENSE).