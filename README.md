# WinExeLua - Lua to EXE Compiler

A C-based compiler that converts Lua source files into standalone Windows executable files.
## help us
***I published this repository (which still doesn't work properly) for you to fix it, improve it, and turn it into the best Lua compiler on the market 😁***

## Features

- Compile Lua scripts to standalone Windows EXE files
- Embed Lua runtime directly in the executable
- Simple command-line interface
- Support for Lua 5.1, 5.2, 5.3, and 5.4
- Cross-platform compilation
- Minimal dependencies

## Building

### Prerequisites

- GCC or Clang compiler
- Make or CMake
- Windows SDK (for Windows development)

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

Or using Visual Studio:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

## Usage

```bash
winexelua input.lua -o output.exe
```

### Options

- `-o, --output <file>` : Output executable filename
- `-i, --icon <file>` : Custom application icon (ICO format)
- `-v, --version <ver>` : Application version string
- `-c, --console` : Enable console window
- `-h, --help` : Display help message

## Example

```bash
winexelua hello.lua -o hello.exe -c
```

This will compile `hello.lua` to `hello.exe` with a console window.

## Project Structure

```
.
├── CMakeLists.txt          # CMake build configuration
├── README.md              # This file
├── src/
│   ├── main.c            # Main entry point
│   ├── compiler.c        # Compiler logic
│   ├── embedder.c        # Executable embedder
│   └── utils.c           # Utility functions
├── include/
│   ├── compiler.h        # Compiler header
│   ├── embedder.h        # Embedder header
│   └── utils.h           # Utilities header
├── lua/
│   ├── lua.h             # Lua header
│   ├── lauxlib.h         # Lua auxiliary library
│   └── lualib.h          # Lua standard library
└── examples/
    ├── hello.lua         # Hello world example
    └── calculator.lua    # Simple calculator example
```

## How It Works

1. **Parsing**: Reads the Lua source file
2. **Embedding**: Embeds the Lua code as a resource in a template executable
3. **Compilation**: Compiles the template with embedded code
4. **Linking**: Links against Lua runtime library
5. **Output**: Produces standalone EXE file

## License

MIT License
