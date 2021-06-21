CC=gcc

.PHONY: clean all

#all: bench test_rbtree
all: rbspeed

rbspeed: rbspeed.c | rbtree.h
	$(CC) -o $@ $^ -Ofast -Wall -Wpedantic

clean:
	rm -rf *.o rbspeed
