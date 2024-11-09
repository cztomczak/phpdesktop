#!/bin/bash

# This script builds with PHP extensions enabled: mysqli, pgsql
# openssl, curl and zlib.

# REQUIREMENTS:

# 1. Download PHP sources from http://php.net/downloads.php
#    and extract them to "phpdesktop/php/" directory, so you
#    you should have "phpdesktop/php/php-x.x.x" directory.
#
# 2. Postgresql extension
#    >> brew install postgresql
#
# 3. Openssl support
#    >> brew install openssl
#
# 4. Curl support
#    >> brew install curl
#
# 5. Install pkg-config
#    >> brew install pkg-config
#
# 6. Install iconv
#    >> brew install libiconv

# How to enable other extensions?
# Go to the build/php-xx/ directory and run the help command:
# >> ./configure --help
# Find the flag to enable the extension you want and add it in
# the code below.

# Exit immediately if a command exits with a non-zero status.
set -e

# Print all executed commands to terminal.
set -x

clear && clear

root_dir=$(dirname $0)
cd ${root_dir}/php/php*/
php_dir=$(pwd)
echo "Found PHP: ${php_dir}"
echo "Configure PHP..."
./configure \
    --prefix=${php_dir}/dist-install \
    --exec-prefix=${php_dir}/dist-install-exec-prefix \
    --with-mysqli \
    --with-pgsql=$(brew --prefix postgresql) \
    --with-openssl=$(brew --prefix openssl) \
    --with-curl=$(brew --prefix curl) \
    --with-zlib \
    --with-iconv=$(brew --prefix libiconv)
echo "Build PHP..."
make install
cp ./dist-install-exec-prefix/bin/php-cgi ./../php-cgi
echo "Done."
