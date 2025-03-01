#!/bin/bash

# This script builds with PHP extensions enabled: mysqli, openssl.

# Download PHP sources from http://php.net/downloads.php and extract them in the
# "phpdesktop/php-$arch/" directory, so you you should have a "phpdesktop/php-$arch/php-x.x.x"
# directory.
#
# Then run buildphp.sh script.
#
# Use otool to check dependencies:
# otool -L php-cgi
# otool -l php-cgi
#
# Check if everything is okay:
# php-cgi --version

# How to enable other extensions? Go to the php-x.x.x/ directory and run the help command:
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

rm -f php-cgi
rm -f php.ini

if ! cd $php_dir/openssl-*/ ; then
    echo "Can't find openssl directory"
    exit 1
fi
openssl_dir=$(realpath $(pwd))
echo "Found OpenSSL: ${openssl_dir}"

if ! cd $php_dir/libiconv-*/ ; then
    echo "Can't find iconv directory"
    exit 1
fi
iconv_dir=$(realpath $(pwd))
echo "Found iconv: ${iconv_dir}"

if ! cd $php_dir/libxml2-*/ ; then
    echo "Can't find libxml2 directory"
    exit 1
fi
libxml2_dir=$(realpath $(pwd))
echo "Found libxml2: ${libxml2_dir}"

if ! cd $php_dir/sqlite-*/ ; then
    echo "Can't find sqlite directory"
    exit 1
fi
sqlite_dir=$(realpath $(pwd))
echo "Found sqlite: ${sqlite_dir}"

if ! cd $php_dir/zlib-*/ ; then
    echo "Can't find zlib directory"
    exit 1
fi
zlib_dir=$(realpath $(pwd))
echo "Found zlib: ${zlib_dir}"

if ! cd $php_dir/libpng-*/ ; then
    echo "Can't find libpng directory"
    exit 1
fi
png_dir=$(realpath $(pwd))
echo "Found PNG: ${png_dir}"

if ! cd $php_dir/jpeg-*/ ; then
    echo "Can't find jpeg directory"
    exit 1
fi
jpeg_dir=$(realpath $(pwd))
echo "Found JPEG: ${jpeg_dir}"

if ! cd $php_dir/onig-*/ ; then
    echo "Can't find onig directory"
    exit 1
fi
onig_dir=$(realpath $(pwd))
echo "Found onig: ${onig_dir}"

if ! cd $php_dir/php-*/ ; then
    echo "Can't find PHP sources directory"
    exit 1
fi
php_sources_dir=$(realpath $(pwd))
echo "Found PHP sources: ${php_sources_dir}"

echo "Configure PHP..."
cp $sqlite_dir/dist-install/lib/libsqlite3.dylib ./libsqlite3.dylib  # To get around bug in conftest
export EXTRA_CFLAGS="-Wno-unused-command-line-argument -lresolv"  # To get around bug: uresolved symbol "_res_9_dn_expand".
export OPENSSL_CFLAGS="-I${openssl_dir}/dist-install/include"
export OPENSSL_LIBS="-L${openssl_dir}/dist-install/lib -lcrypto -lssl"
export LIBXML_CFLAGS="-I${libxml2_dir}/dist-install/include"
export LIBXML_LIBS="-L${libxml2_dir}/dist-install/lib -lxml2"
export SQLITE_CFLAGS="-I${sqlite_dir}/dist-install/include"
export SQLITE_LIBS="-L${sqlite_dir}/dist-install/lib -lsqlite3"
export ZLIB_CFLAGS="-I${zlib_dir}/dist-install/include"
export ZLIB_LIBS="-L${zlib_dir}/dist-install/lib -lz"
export PNG_CFLAGS="-I${png_dir}/dist-install/include"
export PNG_LIBS="-L${png_dir}/dist-install/lib -lpng"
export JPEG_CFLAGS="-I${jpeg_dir}/dist-install/include"
export JPEG_LIBS="-L${jpeg_dir}/dist-install/lib -ljpeg"
export ONIG_CFLAGS="-I${onig_dir}/dist-install/include"
export ONIG_LIBS="-L${onig_dir}/dist-install/lib -lonig"
./configure -v \
    --prefix=${php_sources_dir}/dist-install \
    --exec-prefix=${php_sources_dir}/dist-install \
    --with-mysqli \
    --with-openssl \
    --with-iconv="$iconv_dir/dist-install" \
    --with-zlib="$zlib_dir/dist-install"  \
    --enable-gd \
    --with-jpeg \
    --enable-mbstring
echo "Build PHP..."
make install

cp ./dist-install/bin/php-cgi $php_dir/php-cgi
cp $root_dir/php.ini $php_dir/php.ini

cd $php_dir

install_name_tool -rpath $openssl_dir/dist-install/lib  @loader_path/. ./php-cgi
install_name_tool -delete_rpath $iconv_dir/dist-install/lib ./php-cgi
install_name_tool -delete_rpath $libxml2_dir/dist-install/lib ./php-cgi
install_name_tool -delete_rpath $sqlite_dir/dist-install/lib ./php-cgi
install_name_tool -delete_rpath $zlib_dir/dist-install/lib ./php-cgi
install_name_tool -delete_rpath $png_dir/dist-install/lib ./php-cgi
install_name_tool -delete_rpath $jpeg_dir/dist-install/lib ./php-cgi
install_name_tool -delete_rpath $onig_dir/dist-install/lib ./php-cgi

install_name_tool -change $openssl_dir/dist-install/lib/libcrypto.3.dylib libcrypto.3.dylib ./php-cgi
install_name_tool -change $openssl_dir/dist-install/lib/libssl.3.dylib libssl.3.dylib ./php-cgi
install_name_tool -change $iconv_dir/dist-install/lib/libiconv.2.dylib libiconv.2.dylib ./php-cgi
install_name_tool -change $libxml2_dir/dist-install/lib/libxml2.2.dylib libxml2.2.dylib ./php-cgi
install_name_tool -change $sqlite_dir/dist-install/lib/libsqlite3.dylib libsqlite3.dylib ./php-cgi
install_name_tool -change $zlib_dir/dist-install/lib/libz.1.dylib libz.1.dylib ./php-cgi
install_name_tool -change $png_dir/dist-install/lib/libpng16.16.dylib libpng.dylib ./php-cgi
install_name_tool -change $jpeg_dir/dist-install/lib/libjpeg.9.dylib libjpeg.dylib ./php-cgi
install_name_tool -change $onig_dir/dist-install/lib/libonig.5.dylib libonig.dylib ./php-cgi

echo "Done."
