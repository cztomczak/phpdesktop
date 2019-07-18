#!/bin/bash

# This script builds with PHP extensions enabled: mysqli, pgsql
# and openssl. Tested on Ubuntu 14.04 64-bit. Before running the
# script see the requirements section.

# REQUIREMENTS:

# 1. Download PHP sources from http://php.net/downloads.php
#    and extract them to "build/php*/" directory (create build/
#    directory first).

# 2. Postgresql extension
#    Install these dependencies:
#    >> sudo apt-get install libpq-dev
#    And later when distributing app you have to make sure that 'libpq5'
#    package is installed on end user machine.

# 3. Openssl support
#    Install these dependencies:
#    >> sudo apt-get install libssl-dev libsslcommon2-dev

# 4. Curl support
#    Install these dependencies:
#    >> sudo apt-get install libcurl4-gnutls-dev

# How to enable other extensions?
# Go to the build/php-xx/ directory and run the help command:
# >> ./configure --help
# Find the flag to enable the extension you want and add it in
# the code below.

# Exit immediately if a command exits with a non-zero status.
set -e

# Print all executed commands to terminal.
# set -x

root_dir=$(dirname $0)
cd ${root_dir}/build/php*/
php_dir=$(pwd)
echo "Found PHP: ${php_dir}"
echo "Configure PHP..."
./configure \
    --prefix=${php_dir}/dist-install \
    --exec-prefix=${php_dir}/dist-install-exec-prefix \
    --with-mysqli \
    --with-pgsql=/usr/include/postgresql \
    --with-libdir=/lib/x86_64-linux-gnu \
    --with-openssl=/usr \
    --with-curl=/usr/include/curl
echo "Build PHP..."
make
echo "Copy php-cgi to build/bin/"
cp sapi/cgi/php-cgi ../bin/php-cgi-with-ext
echo "OK completed."
