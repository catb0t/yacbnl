#! /bin/sh
set -e
alias premake="util/premake5-\$(uname -s | tr '[:upper:]' '[:lower:]')-\$(uname -i) gmake"

for lang in 'c' 'c++'
do
  make clean

  premake --lang=$lang

  make -j
  make config=dist -j

  bin/dbg/test_yacbnl
  bin/dist/test_yacbnl

  make clean

  premake --lang=$lang

  make CFLAGS=-DPREFER_CHAR_CONV -j
  make CFLAGS=-DPREFER_CHAR_CONV config=dist -j

  bin/dbg/test_yacbnl
  bin/dist/test_yacbnl
done
set +e