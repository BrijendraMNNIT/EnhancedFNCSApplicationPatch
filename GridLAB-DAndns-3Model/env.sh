#!/bin/sh

export FNCS_INSTALL="$HOME/FNCS_install"

# update LD_LIBRARY_PATH
if test "x$LD_LIBRARY_PATH" = x
then
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$FNCS_INSTALL/lib64"
else
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$FNCS_INSTALL/lib64:$LD_LIBRARY_PATH"
fi

# update PATH
if test "x$PATH" = x
then
    export PATH="$FNCS_INSTALL/bin"
else
    export PATH="$FNCS_INSTALL/bin:$PATH"
fi

export FNCS_FATAL=yes
