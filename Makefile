CFLAGS=-std=c11 -g -static
CC = clang

current_dir := $(shell basename `pwd`)

tt:
	@echo $(current_dir)
read: read.c

test: read
	@bash ./test.sh

test1: tmp.s
	@bash test1.sh

clean:
	rm -f read *.o *~ tmp*

.PHONY: test clean test1

