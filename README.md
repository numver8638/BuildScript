BuildScript
=============================
Script-like build engine for my own hobby projects.

> Notice: Because I'm not native English speaker, there may be many typos and incorrect statements.

## How to build
Requisites:
- Boost library(endian, filesystem, iostreams, optional, program_options, variant2) 1.75 or above
- Catch2 library 2.13 or above
- CMake 3.12 or above
- {fmt} library 7.1 or above
- Build tools which can compile C++11 code. (Tested in Visual Studio 2017, 2019 and Clang 11.0.0)

> If you're using [vcpkg](https://github.com/microsoft/vcpkg) as a package manager,
> add `-DCMAKE_TOOLCHAIN_FILE=<your_vcpkg_path>/scripts/buildsystems/vcpkg.cmake` in CMake command line and
> VCPKG_FEATURE_FLAGS=versions for environment variable.

## License
BuildScript is under the MIT License.
Also, BuildScript uses open source projects described below:

### Boost
Licensed under the Boost Software License Version 1.0.
URL: https://www.boost.org

### Catch2
Licensed under the Boost Software License Version 1.0.
URL: https://github.com/catchorg/catch2

### {fmt}
Licensed under the MIT License.
URL: https://github.com/fmtlib/fmt