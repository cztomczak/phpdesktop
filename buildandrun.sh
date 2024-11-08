SCRIPT=$(realpath "$0")
ROOTDIR=$(dirname "$SCRIPT")
clear && printf '\e[3J'
set -o xtrace
cd $ROOTDIR/build
ninja -j 8 phpdesktop
retval=$?
if [ $retval -ne 0 ]; then
    echo "ninja FAILED with error code $retval"
    exit $retval
fi
open -W --stdout $(tty) --stderr $(tty) "./Debug/PHP Desktop.app"
#./Debug/PHP\ Desktop.app/Contents/MacOS/PHP\ Desktop
