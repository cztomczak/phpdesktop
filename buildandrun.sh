script=$(realpath "$0")
root_dir=$(dirname "$script")

if [[ $(uname -m) == "arm64" ]]; then
    arch="arm64"
elif [[ $(uname -m) == "x86_64" ]]; then
    arch="x86_64"
else
    echo "Unknown architecture"
    exit 1
fi

build_dir=$root_dir/build_$arch

clear && printf '\e[3J'
set -o xtrace

touch $root_dir/app.cpp  # To force copying resources if changed.

cd $build_dir
ninja -j 8 phpdesktop
retval=$?
if [ $retval -ne 0 ]; then
    echo "ninja FAILED with error code $retval"
    exit $retval
else
    export PHPDESKTOP_ENABLE_LOGGING_STDERR=1
    open -W --stdout $(tty) --stderr $(tty) "./Release/PHP Desktop.app"
fi
