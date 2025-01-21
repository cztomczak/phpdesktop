#!/bin/bash

# This script builds with PHP extensions enabled: mysqli, openssl.

# Download PHP sources from http://php.net/downloads.php and extract them to "phpdesktop/php/" directory,
# so you you should have "phpdesktop/php/php-x.x.x" directory.
#
# Then run buildphp.sh script.
#
# Use otool to check dependencies:
# otool -L php-cgi
# otool -l php-cgi
#
# Check if everything is okay:
# php-cgi --version

# How to enable other extensions? Go to the build/php-xx/ directory and run the help command:
# >> ./configure --help
# Find the flag to enable the extension you want and add it in the code below.
#
# To build with PostgreSQL support do:
#   Run `brew install postgresql`
#   Add `--with-pgsql=$(brew --prefix postgresql)` to configure command further below.
# Note that this will add a dependency and PHP will fail to launch if library is not installed on end user computer.

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
rm -f php-cgi
rm -f php.ini

if ! cd $root_dir/php/openssl-*/ ; then
    echo "Can't find openssl directory"
    exit 1
fi
openssl_dir=$(realpath $(pwd))
echo "Found OpenSSL: ${openssl_dir}"

if ! cd $root_dir/php/libxml2-*/ ; then
    echo "Can't find libxml2 directory"
    exit 1
fi
libxml2_dir=$(realpath $(pwd))
echo "Found libxml2: ${libxml2_dir}"

if ! cd $root_dir/php/sqlite-*/ ; then
    echo "Can't find sqlite directory"
    exit 1
fi
sqlite_dir=$(realpath $(pwd))
echo "Found sqlite: ${sqlite_dir}"

if ! cd $root_dir/php/zlib-*/ ; then
    echo "Can't find zlib directory"
    exit 1
fi
zlib_dir=$(realpath $(pwd))
echo "Found zlib: ${zlib_dir}"

if ! cd $root_dir/php/php-*/ ; then
    echo "Can't find php directory"
    exit 1
fi
php_dir=$(realpath $(pwd))
echo "Found PHP: ${php_dir}"

echo "Configure PHP..."
export EXTRA_CFLAGS="-Wno-unused-command-line-argument -lresolv"  # To get around bug: uresolved symbol "_res_9_dn_expand".
export OPENSSL_CFLAGS="-I${openssl_dir}/dist-install/include"
export OPENSSL_LIBS="-L${openssl_dir}/dist-install/lib -lcrypto -lssl"
export LIBXML_CFLAGS="-I${libxml2_dir}/dist-install/include"
export LIBXML_LIBS="-L${libxml2_dir}/dist-install-exec-prefix/lib -lxml2"
export SQLITE_CFLAGS="-I${sqlite_dir}/dist-install/include"
export SQLITE_LIBS="-L${sqlite_dir}/dist-install/lib -lsqlite3"
export ZLIB_CFLAGS="-I${zlib_dir}/dist-install/include"
export ZLIB_LIBS="-L${zlib_dir}/dist-install-exec-prefix/lib -lz"
./configure \
    --prefix=${php_dir}/dist-install \
    --exec-prefix=${php_dir}/dist-install-exec-prefix \
    --with-mysqli \
    --with-openssl \
    --without-iconv
echo "Build PHP..."
make install

cp ./dist-install-exec-prefix/bin/php-cgi ./../php-cgi
cp $root_dir/php.ini $root_dir/php/php.ini

cd $root_dir/php/

install_name_tool -rpath $openssl_dir/dist-install/lib  @loader_path/. php-cgi
install_name_tool -delete_rpath $libxml2_dir/dist-install-exec-prefix/lib php-cgi
install_name_tool -delete_rpath $sqlite_dir/dist-install/lib php-cgi
install_name_tool -delete_rpath $zlib_dir/dist-install-exec-prefix/lib php-cgi

install_name_tool -change $openssl_dir/dist-install/lib/libcrypto.3.dylib libcrypto.3.dylib php-cgi
install_name_tool -change $openssl_dir/dist-install/lib/libssl.3.dylib libssl.3.dylib php-cgi
install_name_tool -change $libxml2_dir/dist-install-exec-prefix/lib/libxml2.2.dylib libxml2.2.dylib php-cgi
install_name_tool -change $sqlite_dir/dist-install/lib/libsqlite3.dylib libsqlite3.dylib php-cgi
install_name_tool -change $zlib_dir/dist-install-exec-prefix/lib/libz.1.dylib libz.1.3.1.dylib php-cgi

echo "Done."
