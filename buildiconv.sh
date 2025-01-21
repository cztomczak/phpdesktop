# Download sources from: https://www.gnu.org/software/libiconv/
# Then extract in php/ directory.

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
rm -f libiconv.2.dylib

if ! cd $root_dir/php/libiconv-*/ ; then
    echo "Can't find iconv directory"
    exit 1
fi
iconv_dir=$(realpath $(pwd))
echo "Found iconv: ${iconv_dir}"

echo "Configure iconv..."
./configure \
    --prefix=${iconv_dir}/dist-install \
    --exec-prefix=${iconv_dir}/dist-install
echo "Build iconv..."
make install

cp ./dist-install/lib/libiconv.2.dylib ./../libiconv.2.dylib
install_name_tool -id libiconv.2.dylib ./../libiconv.2.dylib

echo "Done."
