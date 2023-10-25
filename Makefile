CFLAGS=-std=c11 -g -static

read: read.c

test: read
	bash ./test.sh

clean:
	rm -f read *.o *~ tmp*

.PHONY: test clean