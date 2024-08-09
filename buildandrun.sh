SCRIPT=$(realpath "$0")
ROOTDIR=$(dirname "$SCRIPT")
cd $ROOTDIR/build
clear
ninja -j 8 phpdesktop
retval=$?
if [ $retval -ne 0 ]; then
    echo "ninja FAILED with error code $retval"
    exit $retval
fi
