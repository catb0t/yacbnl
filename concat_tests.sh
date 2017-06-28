#!/bin/bash

(
  echo -e '#include<criterion/criterion.h>\n#include"yacbnl.h"\n'
  paste --delimiter=\\n --serial tests/*.c | grep -v '#include'
) > test_yacbnl.c
util/minify.py test_yacbnl.c > test_yacbnl.min.c
mv test_yacbnl.min.c test_yacbnl.c