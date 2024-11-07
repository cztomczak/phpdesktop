SCRIPT=$(realpath "$0")
ROOTDIR=$(dirname "$SCRIPT")
clear && printf '\e[3J'
set -o xtrace
cd $ROOTDIR
rm -rf build/*
mkdir build
mkdir downloads
cd $ROOTDIR/build
if [[ $(uname -m) == "arm64" ]]; then
    PHPDESKTOP_ARCH="arm64"
elif [[ $(uname -m) == "x86_64" ]]; then
    PHPDESKTOP_ARCH="x86_64"
fi
cmake -G "Ninja" -DPROJECT_ARCH="$PHPDESKTOP_ARCH" -DCMAKE_BUILD_TYPE=Debug $ROOTDIR
ninja -j 8 phpdesktop
