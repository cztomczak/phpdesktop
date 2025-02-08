# Download sources from: https://www.gnu.org/software/libiconv/
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

rm -f libiconv.2.dylib

if ! cd $php_dir/libiconv-*/ ; then
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
