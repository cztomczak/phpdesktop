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

rm -f libjpeg.dylib

if ! cd $php_dir/jpeg-*/ ; then
    echo "Can't find libjpeg directory"
    exit 1
fi
jpeg_dir=$(realpath $(pwd))
echo "Found JPEG: ${jpeg_dir}"

echo "Configure JPEG..."
./Configure \
    --prefix=${jpeg_dir}/dist-install \
    --exec-prefix=${jpeg_dir}/dist-install
echo "Build JPEG..."
make install

cp ./dist-install/lib/libjpeg.dylib ./../libjpeg.dylib

install_name_tool -id libjpeg.dylib ./../libjpeg.dylib
install_name_tool -change $jpeg_dir/dist-install/lib/libjpeg.dylib libjpeg.dylib ./../libjpeg.dylib

echo "Done."