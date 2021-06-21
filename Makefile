CC=gcc

.PHONY: clean all

#all: bench test_rbtree
all: rbspeed test_rbtree

rbspeed: rbspeed.c rbtree.h
	$(CC) -o $@ $< -Ofast -Wall -Wpedantic

test_rbtree: test_rbtree.c rbtree.h test_rbtree.h
	$(CC) -o $@ $< -Ofast -Wall -Wpedantic -lcmocka

clean:
	rm -rf *.o rbspeed
