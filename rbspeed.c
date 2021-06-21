
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <time.h>

#include "rbspeed_helper.h"

static inline unsigned
xorshift32(unsigned *const p_rng)
{
    unsigned x = *p_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *p_rng = x;
    return x;
}

#define NUM_LOOPS 1000000
#define NUM_OBJS 200000

int
main(void)
{
    //unsigned rng = time(NULL);
    unsigned rng = 0xdeadbeefu;

    rbt_t tree;
    rbt_init(&tree);

    my_t *objs = malloc(sizeof(*objs) * NUM_OBJS);
    for (int i = 0; i < NUM_OBJS; ++i) {
        // Randomly put a non-negative key in all the objects
        objs[i].my_key = (int)(xorshift32(&rng) & 0xefffffffu);
    }

    struct timespec start, end;
    uint64_t get_ns = 0;
    uint64_t get_uncached_ns = 0;
    uint64_t add_ns = 0;
    uint64_t rem_ns = 0;

    uint64_t overhead_ns = 0;

    for (int i = 0; i < NUM_OBJS; ++i) {
        // Put all the objects into the tree
        my_t *a = rbt_add(&tree, &objs[i]);
        while (a != &objs[i]) {
            objs[i].my_key = (int)(xorshift32(&rng) & 0xefffffffu);
            a = rbt_add(&tree, &objs[i]);
        }
    }

    for (int i = 0; i < NUM_LOOPS; ++i) {
        clock_gettime(CLOCK_REALTIME, &start);
        clock_gettime(CLOCK_REALTIME, &end);
        overhead_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);
    }

    double const overhead = 1.0 * overhead_ns / NUM_LOOPS;
    printf("Overhead is %f nanoseconds\n", overhead);

    for (int i = 0; i < NUM_LOOPS; ++i) {
        unsigned const idx = xorshift32(&rng) % NUM_OBJS;
        clock_gettime(CLOCK_REALTIME, &start);
        my_t *g = rbt_get(&tree, objs[idx].my_key);
        clock_gettime(CLOCK_REALTIME, &end);

        assert(g == &objs[idx]);
        get_uncached_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);

        clock_gettime(CLOCK_REALTIME, &start);
        g = rbt_get(&tree, objs[idx].my_key);
        clock_gettime(CLOCK_REALTIME, &end);

        assert(g == &objs[idx]);
        get_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);

        clock_gettime(CLOCK_REALTIME, &start);
        my_t *const e = rbt_rem(&tree, objs[idx].my_key);
        clock_gettime(CLOCK_REALTIME, &end);

        assert(e == &objs[idx]);
        rem_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);

        clock_gettime(CLOCK_REALTIME, &start);
        rbt_add(&tree, &objs[idx]);
        clock_gettime(CLOCK_REALTIME, &end);
        add_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);
    }

    printf("Ran test with a tree of size %d\n", NUM_OBJS);
    printf("Average time to get a node(uncached): %f nanoseconds\n", 1.0 * get_uncached_ns / (1.0 * NUM_LOOPS) - overhead);
    printf("Average time to get a node: %f nanoseconds\n", 1.0 * get_ns / (1.0 * NUM_LOOPS) - overhead);
    printf("Average time to add a node: %f nanoseconds\n", 1.0 * add_ns / (1.0 * NUM_LOOPS) - overhead);
    printf("Average time to remove a node: %f nanoseconds\n", 1.0 * rem_ns / (1.0 * NUM_LOOPS) - overhead);

    free(objs);

    return 0;
}
