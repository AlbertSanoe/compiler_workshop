CFLAGS=-std=c11 -g -static
CC = clang
read: read.c

test: read
	@bash ./test.sh


test1: tmp.s
	@bash test1.sh

clean:
	rm -f read *.o *~ tmp*

.PHONY: test clean test1