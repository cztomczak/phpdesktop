# Download zlib 1.3.1 sources from: https://github.com/madler/zlib/releases
# Then extract in the php/ directory.

# Exit immediately if a command exits with a non-zero status.
set -e

# Print all executed commands to terminal.
set -x

clear && clear

root_dir=$(realpath $(dirname $0))
echo "root_dir=$root_dir"

if ! cd $root_dir/php/ ; then
    echo "php/ directory doesn't exist"
    exit 1
fi
rm -f libz.1.3.1.dylib

if ! cd $root_dir/php/zlib-*/ ; then
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
