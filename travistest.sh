#! /bin/sh
set -e
util/premake5-"$(uname -s | tr '[:upper:]' '[:lower:]')"-"$(uname -i)" gmake
make -j
make config=dist -j
bin/dbg/test_yacbnl
bin/dist/test_yacbnl
make CFLAGS=-DPREFER_CHAR_CONV -j
make CFLAGS=-DPREFER_CHAR_CONV config=dist -j
bin/dbg/test_yacbnl
bin/dist/test_yacbnl
set +e