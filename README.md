# exec-helper
Or How To Get Your Coffee In Peace.

## Continuous integration
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/2d81dc4be75c4cc2bc4cf2a3e9e6f98e)](https://app.codacy.com/app/bverhagen/exec-helper?utm_source=github.com&utm_medium=referral&utm_content=bverhagen/exec-helper&utm_campaign=badger)
[![build status](https://gitlab.com/bverhagen/exec-helper/badges/master/build.svg)](https://gitlab.com/bverhagen/exec-helper/commits/master)
[![coverage report](https://gitlab.com/bverhagen/exec-helper/badges/master/coverage.svg)](https://gitlab.com/bverhagen/exec-helper/commits/master)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/exec-helper/badge.svg)](https://scan.coverity.com/projects/exec-helper)

## Why
Exec-helper is a meta-wrapper for executing tasks on the command line.

## Installing from package
### Arch Linux based distributions
Arch linux users can use the [exec-helper-git](https://aur.archlinux.org/packages/exec-helper-git) (AUR) package or checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building the package from source.

### Debian based distributions
Checkout the [exec-helper-package](https://github.com/bverhagen/exec-helper-package) project for building exec-helper as a package, if your system is supported.

### Other distributions
Checkout the [Building from source](#building-from-source) section.

## Building from source
### Requirements
#### Host build dependencies
- Doxygen for building the man-pages and/or HTML documentation (optional)
- gitchangelog for building the changelog (optional)
- cmake
- make for the quick install

#### Target build dependencies
- A POSIX compliant operating system
- A C++ 14 compatible compiler
- [boost-program-options](https://github.com/boostorg/program_options) development files
- [boost-log](https://github.com/boostorg/log) development files
- [boost-filesystem](https://github.com/boostorg/filesystem) development files
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) development files (optional, will be downloaded and compiled in statically if missing)
- [Microsoft gsl](https://github.com/Microsoft/GSL) development files (optional, will be downloaded and compiled in statically if missing)
- [catch](https://github.com/philsquared/Catch) unittest framework development files for building the tests (optional, for building the tests)

### Quick installation
```sh
$ make
# make install
```

Use
```sh
$ make help
```
for an overview of the available quick installation targets and for an overview of the (very limited) available configuration options. More configuration options are available by following the 'advanced install' section.

### Advanced install
Check the [documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html).

### Cross compilation
Exec-helper supports both native and cross compilation (including building with a custom sysroot) builds. Cross compilation requires invoking cmake directly and appending *-DCMAKE_TOOLCHAIN_FILE=\<toolchain-file\>* to the cmake initialization command. Check the _toolchain.cmake.in_ file for a template on setting up the toolchain file for cross compilation and the _Makefile_ for a template of the cmake initialization command.

## Documentation
See [documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html) for the latest documentation.

## Configuration
Check the [Documentation](http://bverhagen.gitlab.io/exec-helper/docs/html/index.html) for more information about configuring exec-helper.

## Project analysis
The source code of this project is continuously analyzed by multiple tools in an attempt to catch and fix issues and bugs as quickly as possible. Released versions should have passed the analysis from the following tools:
- [valgrind](http://valgrind.org) (memcheck)
- [cppcheck](http://cppcheck.sourceforge.net)
- [clang-tidy](http://clang.llvm.org/extra/clang-tidy)
- [clang-static-analyzer](https://clang-analyzer.llvm.org)
- [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) (UBSan)
- [pmd](https://pmd.github.io) (cpd)
- [coverity](https://scan.coverity.com/projects/exec-helper)
- [clang-format](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)

Check the _.exec-helper_ file for detailed information about how these analysis methods are configured and used. The analysis tools can be executed locally using exec-helper with this project.
