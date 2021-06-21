CC=gcc

.PHONY: clean all

#all: bench test_rbtree
all: rbspeed test_rbtree

rbspeed.o: rbspeed.c rbspeed_helper.h
	$(CC) -c -o $@ $< -Ofast -Wall -Wpedantic

rbspeed_helper.o: rbspeed_helper.c rbspeed_helper.h rbtree.h
	$(CC) -c -o $@ $< -Ofast -Wall -Wpedantic

rbspeed: rbspeed.o rbspeed_helper.o
	$(CC) -o $@ $^ -Ofast -Wall -Wpedantic

test_rbtree: test_rbtree.c rbtree.h test_rbtree.h
	$(CC) -o $@ $< -Ofast -Wall -Wpedantic -lcmocka -fsanitize=undefined -fsanitize=address -ggdb3

clean:
	rm -rf *.o rbspeed test_rbtree
