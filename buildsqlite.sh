# Download sources from: https://www.sqlite.org/download.html
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

rm -f libsqlite3.dylib

if ! cd $php_dir/zlib-*/ ; then
    echo "Can't find zlib directory"
    exit 1
fi
zlib_dir=$(realpath $(pwd))
echo "Found zlib: ${zlib_dir}"

if ! cd $php_dir/sqlite-*/ ; then
    echo "Can't find sqlite directory"
    exit 1
fi
sqlite_dir=$(realpath $(pwd))
echo "Found sqlite: ${sqlite_dir}"

export CPPFLAGS="-I${zlib_dir}/dist-install/include"
export LDFLAGS="-L${zlib_dir}/dist-install/lib"
echo "Configure sqlite..."
./configure \
    --prefix=${sqlite_dir}/dist-install \
    --enable-all
echo "Build sqlite..."
make install

cp ./dist-install/lib/libsqlite3.dylib.3.* ./../libsqlite3.dylib
install_name_tool -id libsqlite3.dylib ./../libsqlite3.dylib
install_name_tool -delete_rpath $sqlite_dir/dist-install/lib ./../libsqlite3.dylib
install_name_tool -change $zlib_dir/dist-install/lib/libz.1.dylib libz.1.3.1.dylib ./../libsqlite3.dylib

echo "Done."
