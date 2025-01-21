# Download official distribution tarball (tar.xz) from: https://gitlab.gnome.org/GNOME/libxml2/-/releases
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
rm -f libxml2.2.dylib

if ! cd $root_dir/php/libiconv-*/ ; then
    echo "Can't find iconv directory"
    exit 1
fi
iconv_dir=$(realpath $(pwd))
echo "Found iconv: ${iconv_dir}"

if ! cd $root_dir/php/libxml2-*/ ; then
    echo "Can't find libxml2 directory"
    exit 1
fi
libxml2_dir=$(realpath $(pwd))
echo "Found Libxml2: ${libxml2_dir}"

echo "Configure libxml2..."
./configure \
    --prefix=${libxml2_dir}/dist-install \
    --exec-prefix=${libxml2_dir}/dist-install \
    --without-python \
    --with-iconv="$iconv_dir/dist-install"
echo "Build libxml2..."
make install

cp ./dist-install/lib/libxml2.2.dylib ./../libxml2.2.dylib
install_name_tool -id libxml2.2.dylib ./../libxml2.2.dylib
install_name_tool -change $iconv_dir/dist-install/lib/libiconv.2.dylib libiconv.2.dylib ./../libxml2.2.dylib

echo "Done."
