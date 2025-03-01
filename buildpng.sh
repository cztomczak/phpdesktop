# Exit immediately if a command exits with a non-zero status.
set -e

# Print all executed commands to terminal.
set -x

clear && clear

if [[ $(uname -m) == "arm64" ]]; then
    arch="arm64"
elif [[ $(uname -m) == "x86_64" ]]; then
    arch="x86_64"
else
    echo "Unknown architecture"
    exit 1
fi

root_dir=$(realpath $(dirname $0))
echo "root_dir=$root_dir"

if ! cd $root_dir/php-$arch/ ; then
    echo "Can't find PHP directory"
    exit 1
fi
php_dir=$(realpath $(pwd))
echo "Found PHP: ${php_dir}"

rm -f libpng.dylib

if ! cd $php_dir/zlib-*/ ; then
    echo "Can't find zlib directory"
    exit 1
fi
zlib_dir=$(realpath $(pwd))
echo "Found zlib: ${zlib_dir}"

if ! cd $php_dir/libpng-*/ ; then
    echo "Can't find libpng directory"
    exit 1
fi
png_dir=$(realpath $(pwd))
echo "Found PNG: ${png_dir}"

echo "Configure PNG..."
./Configure \
    --prefix=${png_dir}/dist-install \
    --exec-prefix=${png_dir}/dist-install
echo "Build PNG..."
make install

cp ./dist-install/lib/libpng.dylib ./../libpng.dylib

install_name_tool -id libpng.dylib ./../libpng.dylib
install_name_tool -change $png_dir/dist-install/lib/libpng.dylib libpng.dylib ./../libpng.dylib
install_name_tool -change /usr/lib/libz.1.dylib libz.1.dylib ./../libpng.dylib

echo "Done."
