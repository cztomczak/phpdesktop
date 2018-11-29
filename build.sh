#!/bin/bash

set -e
# set -x

if [[ -f build/bin/phpdesktop ]]; then
    rm build/bin/phpdesktop
fi

count=`ls -1 build/bin/phpdesktop/*.log 2>/dev/null | wc -l`
if [[ ${count} != 0 ]]; then
    rm build/bin/*.log
fi

make -R -f Makefile "$@"

rc=$?;
if [[ ${rc} = 0 ]]; then
    echo "OK phpdesktop was built";
    ./build/bin/phpdesktop
else
    echo "ERROR";
    exit ${rc};
fi
