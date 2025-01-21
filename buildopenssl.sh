# Download OpenSSL sources from https://github.com/openssl/openssl/releases
# and extract them in the php/ directory.
#
# Then run buildopenssl.sh script.

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
rm -f libcrypto.3.dylib
rm -f libssl.3.dylib
rm -f openssl.cnf

if ! cd $root_dir/php/openssl-*/ ; then
    echo "Can't find openssl directory"
    exit 1
fi
openssl_dir=$(realpath $(pwd))
echo "Found OpenSSL: ${openssl_dir}"

echo "Configure OpenSSL..."
./Configure \
    --prefix=${openssl_dir}/dist-install \
    --openssldir=${openssl_dir}/dist-openssldir
echo "Build OpenSSL..."
make install

cp ./dist-install/lib/libcrypto.3.dylib ./../libcrypto.3.dylib
cp ./dist-install/lib/libssl.3.dylib ./../libssl.3.dylib
cp -r ./dist-openssldir/openssl.cnf ./../openssl.cnf

install_name_tool -id libcrypto.3.dylib ./../libcrypto.3.dylib
install_name_tool -id libssl.3.dylib ./../libssl.3.dylib
install_name_tool -change $openssl_dir/dist-install/lib/libcrypto.3.dylib libcrypto.3.dylib ./../libssl.3.dylib

echo "Done."
