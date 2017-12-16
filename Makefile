FILENAME := $(shell basename `pwd`)
ARCH := $(shell uname -m)
OUT_FILENAME := $(FILENAME)

DEBUG_OPTS := -Wall -Wextra -Wfloat-equal -Winline -Wundef -Werror -fverbose-asm -Wint-to-pointer-cast -Wshadow -Wpointer-arith -Wcast-align  -Wcast-qual -Wunreachable-code -Wstrict-overflow=5 -Wwrite-strings -Wconversion --pedantic-errors -ggdb -Wredundant-decls

MEM_OPTS := -fstack-protector -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer

OPTS := -lm -g -std=gnu11 #-fprofile-arcs -ftest-coverage

ifeq ($(CC), g++)
	#MEM_OPTS += -static-libasan -static-libtsan -static-liblsan -static-libubsan -lasan -lubsan
endif
ifeq ($(CC), gcc)
  DEBUG_OPTS +=  -Werror=maybe-uninitialized#-Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn
  MEM_OPTS += -static-libasan -static-libtsan -static-liblsan -static-libubsan -lasan -lubsan -Werror=maybe-uninitialized
endif
ifeq ($(CC), cc)
  DEBUG_OPTS +=  -Werror=maybe-uninitialized# -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn
	MEM_OPTS += -static-libasan -static-libtsan -static-liblsan -static-libubsan -lasan -lubsan  -Werror=maybe-uninitialized
endif

ifeq ($(CC), clang)
	DEBUG_OPTS +=  -Werror=uninitialized -Wno-language-extension-token
endif

CMD_ARGS ?=

FILES := $(FILENAME).c $(FILENAME).h

all: hdr normal mem test

hdr: bignum.h bn_common.h array_factory.h
	./build.sh

normal: $(FILES)
	$(CC) $(FILENAME).c $(CMD_ARGS) $(DEBUG_OPTS) $(OPTS) -o ./bin/$(OUT_FILENAME)
	@echo

mem: $(FILES)
	$(CC) $(FILENAME).c $(CMD_ARGS) $(DEBUG_OPTS) $(OPTS) $(MEM_OPTS) -o ./bin/memdebug_$(OUT_FILENAME)
	@echo

test: $(FILES)
	./concat_tests.sh
	$(CC) -lcriterion test_$(FILENAME).c $(CMD_ARGS) $(DEBUG_OPTS) $(OPTS) -o ./bin/test_$(OUT_FILENAME)
	@echo

clean:
	rm ./bin/* -r
	rm ./yacbnl.full.h

min:
	$(CC) yacbnl.min.h $(CMD_ARGS) $(DEBUG_OPTS) $(OPTS) -c -o yacbnl.o