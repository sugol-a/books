#!/bin/bash

set -e

APP_NAME="books"
ARCH="x86_64-windows"
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
ldd "$executable" | grep mingw | awk '{print $3}' | xargs -I {} cp -v {} "$dist_dir/bin"
mkdir -pv "$dist_dir/lib/gdk-pixbuf-2.0/2.10.0/loaders"
find /mingw64/lib/gdk-pixbuf-2.0/2.10.0/loaders -name '*.dll' | xargs -I {} cp -v {} "$dist_dir/lib/gdk-pixbuf-2.0/2.10.0/loaders"
cp -v /mingw64/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache "$dist_dir/lib/gdk-pixbuf-2.0/2.10.0"

echo " => Copying icons"
[ -f icons.txt ] && rm icons.txt

# Window icons
echo "share/icons/Adwaita/scalable/ui/window-close-symbolic.svg" >> icons.txt
echo "share/icons/Adwaita/scalable/ui/window-maximize-symbolic.svg" >> icons.txt
echo "share/icons/Adwaita/scalable/ui/window-minimize-symbolic.svg" >> icons.txt

# Other named icons
find /mingw64/share/icons/Adwaita/scalable | grep -f <(grep 'icon-name' ../assets/ui/*.ui | sed -e 's/.*<property name="icon-name">\(\S*\)<.*/\1.svg/g') | sed -e 's,/mingw64/,,g' >> icons.txt

mkdir "$dist_dir/share/icons/Adwaita"
cat icons.txt | xargs -I {} dirname {} | sort | uniq | xargs -I {} mkdir -pv "$dist_dir/{}"
cat icons.txt | xargs -I {} cp "/mingw64/{}" "$dist_dir/{}"
#cp -r /mingw64/share/icons/{Adwaita,hicolor} "$dist_dir/share/icons"

echo " => Copying schemas"
cp /mingw64/share/glib-2.0/schemas/gschemas.compiled "$dist_dir/share/glib-2.0/schemas/gschemas.compiled"

echo " => Zipping"
cd "$build_dir"
zip -r "$(basename $dist_dir).zip" "$(basename $dist_dir)"
