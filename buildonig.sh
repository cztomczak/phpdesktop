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

rm -f libonig.dylib

if ! cd $php_dir/onig-*/ ; then
    echo "Can't find onig directory"
    exit 1
fi
onig_dir=$(realpath $(pwd))
echo "Found onig: ${onig_dir}"

echo "Configure onig..."
./Configure \
    --prefix=${onig_dir}/dist-install \
    --exec-prefix=${onig_dir}/dist-install
echo "Build onig..."
make install

cp ./dist-install/lib/libonig.dylib ./../libonig.dylib

install_name_tool -id libonig.dylib ./../libonig.dylib
install_name_tool -change $onig_dir/dist-install/lib/libonig.dylib libonig.dylib ./../libonig.dylib

echo "Done."
