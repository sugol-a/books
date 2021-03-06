#!/bin/bash

# Copyright (C) 2021  Alister Sanders

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

set -e

APP_NAME="books"
ARCH="x86_64-windows"
LDD="x86_64-w64-mingw32-ldd"
MINGW="/usr/x86_64-w64-mingw32"
MESON_BUILD="$(dirname $0)/../meson.build"

version=$(cat "$MESON_BUILD" | grep 'version' | head -n 1 | sed -e 's/\s*version\s*:\s*'"'"'\([[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+\).*/\1/')

echo " => Found version $version from $MESON_BUILD"

build_dir="$(dirname $0)/../build"
executable="$build_dir/$APP_NAME.exe"
dist_dir="$build_dir/$ARCH-$APP_NAME-$version"

echo " => Creating directory structure"
[ -d "$dist_dir" ] && rm -r "$dist_dir"
mkdir -pv "$dist_dir/bin"
mkdir -pv "$dist_dir/share/icons"
mkdir -pv "$dist_dir/share/glib-2.0/schemas"

echo " => Copying executable to $dist_dir/bin"
cp -v "$executable" "$dist_dir/bin"

echo " => Copying DLLs"
$LDD "$executable" | grep mingw | awk '{print $3}' | xargs -I {} cp -v {} "$dist_dir/bin"
mkdir -pv "$dist_dir/lib/gdk-pixbuf-2.0/2.10.0/loaders"
find $MINGW/lib/gdk-pixbuf-2.0/2.10.0/loaders -name '*.dll' | xargs -I {} cp -v {} "$dist_dir/lib/gdk-pixbuf-2.0/2.10.0/loaders"
#cp -v $MINGW/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache "$dist_dir/lib/gdk-pixbuf-2.0/2.10.0"

echo " => Copying icons"
[ -f icons.txt ] && rm icons.txt

# Window icons
echo "share/icons/Adwaita/scalable/ui/window-close-symbolic.svg" >> icons.txt
echo "share/icons/Adwaita/scalable/ui/window-maximize-symbolic.svg" >> icons.txt
echo "share/icons/Adwaita/scalable/ui/window-minimize-symbolic.svg" >> icons.txt

# Other named icons
find $MINGW/share/icons/Adwaita/scalable | grep -f <(grep 'icon-name' ../assets/ui/*.ui | sed -e 's/.*<property name="icon-name">\(\S*\)<.*/\1.svg/g') | sed -e 's,/mingw64/,,g' >> icons.txt

mkdir "$dist_dir/share/icons/Adwaita"
cat icons.txt | xargs -I {} dirname {} | sort | uniq | xargs -I {} mkdir -pv "$dist_dir/{}"
cat icons.txt | xargs -I {} cp "$MINGW/{}" "$dist_dir/{}"

echo " => Copying schemas"
cp $MINGW/share/glib-2.0/schemas/gschemas.compiled "$dist_dir/share/glib-2.0/schemas/gschemas.compiled"

echo " => Zipping"
cd "$build_dir"
zip -r "$(basename $dist_dir).zip" "$(basename $dist_dir)"
