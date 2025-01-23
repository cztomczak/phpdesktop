SCRIPT=$(realpath "$0")
ROOTDIR=$(dirname "$SCRIPT")
clear && printf '\e[3J'
set -o xtrace
touch $ROOTDIR/app.cpp  # To force copying resources if changed.
cd $ROOTDIR/build
ninja -j 8 phpdesktop
retval=$?
if [ $retval -ne 0 ]; then
    echo "ninja FAILED with error code $retval"
    exit $retval
else
    export PHPDESKTOP_ENABLE_LOGGING_STDERR=1
    open -W --stdout $(tty) --stderr $(tty) "./Release/PHP Desktop.app"
fi
