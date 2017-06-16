#!/bin/bash
src=$(cat bn_common.h bignum.h bna.h | grep -v "#include")
echo "#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

$src" > yacbnl.full.h
