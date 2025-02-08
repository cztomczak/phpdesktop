# Download zlib 1.3.1 sources from: https://github.com/madler/zlib/releases
# Then extract in the PHP directory.

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

rm -f libz.1.3.1.dylib

if ! cd $php_dir/zlib-*/ ; then
    echo "Can't find zlib directory"
    exit 1
fi
zlib_dir=$(realpath $(pwd))
echo "Found zlib: ${zlib_dir}"

echo "Configure zlib..."
./configure \
    --prefix=${zlib_dir}/dist-install \
    --eprefix=${zlib_dir}/dist-install
echo "Build zlib..."
make install

cp ./dist-install/lib/libz.1.3.1.dylib ./../libz.1.3.1.dylib
install_name_tool -id libz.1.3.1.dylib ./../libz.1.3.1.dylib

echo "Done."
