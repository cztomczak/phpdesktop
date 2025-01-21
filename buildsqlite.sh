# Download sources from: https://www.sqlite.org/download.html
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
rm -f libsqlite3.dylib

if ! cd $root_dir/php/sqlite-*/ ; then
    echo "Can't find sqlite directory"
    exit 1
fi
sqlite_dir=$(realpath $(pwd))
echo "Found sqlite: ${sqlite_dir}"

echo "Configure sqlite..."
./configure \
    --prefix=${sqlite_dir}/dist-install \
    --enable-all
echo "Build sqlite..."
make install

cp ./dist-install/lib/libsqlite3.dylib ./../libsqlite3.dylib
install_name_tool -id libsqlite3.dylib ./../libsqlite3.dylib
install_name_tool -delete_rpath $sqlite_dir/dist-install/lib ./../libsqlite3.dylib

echo "Done."
