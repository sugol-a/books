# books

An intelligent bulk cropping tool

## Download
Linux and Windows are both supported.

Windows binaries are available [here](https://git.sugol.org/al/books/releases).

## Building from Source

### Dependencies
 - opencv4
 - gtkmm-4.0
 - cairomm-1.16
 - meson
 - GCC or clang. C++11 support required

### Linux

This project uses the meson build system:

```
$ meson build
$ cd build
$ ninja

```

And install with

```
$ sudo ninja install
```

### Windows
There are a couple of options for building Windows binaries. Either you can
compile natively using `msys2`, or cross-compile from Linux.

⚠ *MSVC is unsupported* ⚠

#### Native
Install `msys2` and build as you would on Linux

#### Cross-compilation
A cross-file is included for building Windows binaries on Linux
(`win32-cross.txt`). This file may need tweaking depending on how your `mingw`
installation is configured. Make sure to install the relevant mingw
dependencies.

Build as you would for Linux, but substitute the first step with `meson setup
build --cross-file=win32-cross.txt`.

Good luck!

#### Packaging
A script for packaging Windows builds is included under `scripts/dist-win32.sh`.
This should work with either `msys2` or a cross-compiled build, but you may need
to tweak the `MINGW` variable to match your mingw installation path.

The script will package the compiled executable, `.dll`s and icons into a zip
archive for distribution.

## License
Copyright © 2021 Alister Sanders (alister@sugol.org)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
