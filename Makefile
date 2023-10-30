CFLAGS=-std=c11 -g -static
CC = clang

current_dir := $(shell basename `pwd`)

tt:
	@echo $(current_dir)
read: read.c

test: read
	@bash ./test.sh

test1: tmp.s
	clang tmp.s -o tmp
	./tmp
	echo $?

clean:
	rm -f read *.o *~ tmp*

.PHONY: test clean test1

