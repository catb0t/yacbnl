#!/bin/bash
src=$(cat bn_common.h misc_util.h addr_interp.h base256.h array_factory.h bignum.h | grep -v "#include")
includes="#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>"
copyleft="/*
  YACBNL Yet Another C BigNum Library
  Copyright (C) 2017 Cat Stevens

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/"
echo "$includes
$src" > yacbnl.full.h

big=$(wc -c < yacbnl.full.h)

minisrc=$(util/minify.py yacbnl.full.h)
echo "$copyleft
$minisrc" > yacbnl.min.h
lil=$(wc -c < yacbnl.min.h)
echo "saved $((big - lil)) bytes"