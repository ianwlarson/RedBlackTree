#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

#include <sys/mman.h>

#include "test_rbtree.h"

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

/* Return a random integer in range (0, max) */
static inline int
randnum(unsigned *const p_rng, int const max)
{
    int o = xorshift32(p_rng) & ~0x80000000u;
    return o % max;
}

static inline float
randuniform(unsigned *const p_rng)
{
    return 1.0f * xorshift32(p_rng) / UINT_MAX;
}

static void
test_basic_functionality(void **state)
{
    (void)state;

    rbt_t tree;
    rbt_init(&tree);

    assert_true(rbt_size(&tree) == 0);
    assert_null(rbt_max(&tree));
    assert_null(rbt_min(&tree));
    assert_null(rbt_rem(&tree, 0));

    for (int i = 0; i < 100; ++i) {
        test_obj_t *obj = test_malloc(sizeof(*obj));
        obj->key = i;
        rbt_add(&tree, obj);
    }

    test_obj_t *tmp = rbt_min(&tree);
    assert_int_equal(tmp->key, 0);
    tmp = rbt_max(&tree);
    assert_int_equal(tmp->key, 99);

    test_free(rbt_rem(&tree, 99));
    test_free(rbt_rem(&tree, 0));

    tmp = rbt_min(&tree);
    assert_int_equal(tmp->key, 1);
    tmp = rbt_max(&tree);
    assert_int_equal(tmp->key, 98);

    for (int i = 1; i < 99; ++i) {
        test_obj_t *obj = rbt_get(&tree, i);
        assert_int_equal(i, obj->key);
        test_free(rbt_rem(&tree, i));
        test_obj_t *min = rbt_min(&tree);
        if (min != NULL)
            assert_int_equal(min->key, i + 1);
    }

    assert_true(rbt_size(&tree) == 0);
}

static void
test_random_double_tree(void **state)
{
    unsigned rng = time(NULL);

    rbt_t tree1;
    rbt_t tree2;
    rbt_init(&tree1);
    rbt_init(&tree2);

    for (int i = 0; i < 100000; ++i) {

        int key1;
        int key2;
        test_obj_t *obj1;
        test_obj_t *obj2;

        float const choice = randuniform(&rng);
        if (choice < 0.33333) {
            key1 = randnum(&rng, 1000000);
            if (rbt_get(&tree1, key1) != NULL) {
                test_free(rbt_rem(&tree1, key1));
                continue;
            }
            obj1 = test_malloc(sizeof(*obj1));
            obj1->key = key1;
            rbt_add(&tree1, obj1);
        } else if (choice < 0.666666) {
            key1 = randnum(&rng, 1000000);
            if (rbt_get(&tree2, key1) != NULL) {
                test_free(rbt_rem(&tree2, key1));
                continue;
            }
            obj1 = test_malloc(sizeof(*obj1));
            obj1->key = key1;
            rbt_add(&tree2, obj1);
        } else if (choice < 0.86666) {
            obj1 = rbt_min(&tree1);
            obj2 = rbt_max(&tree2);
            if ((obj1 == NULL) || (obj2 == NULL) || (obj1->key == obj2->key)) {
                continue;
            }
            key1 = obj1->key;
            key2 = obj2->key;
            test_free(rbt_rem(&tree1, key2));
            test_free(rbt_rem(&tree2, key1));
            rbt_add(&tree1, rbt_rem(&tree2, key2));
            rbt_add(&tree2, rbt_rem(&tree1, key1));
        } else {
            obj1 = rbt_max(&tree1);
            obj2 = rbt_min(&tree2);
            if ((obj1 == NULL) || (obj2 == NULL) || (obj1->key == obj2->key)) {
                continue;
            }
            key1 = obj1->key;
            key2 = obj2->key;
            test_free(rbt_rem(&tree1, key2));
            test_free(rbt_rem(&tree2, key1));
            rbt_add(&tree1, rbt_rem(&tree2, key2));
            rbt_add(&tree2, rbt_rem(&tree1, key1));
        }
    }

    /* pop maximum nodes */
    while (rbt_size(&tree1) > 0) {
        test_free(rbt_popmax(&tree1));
    }

    /* pop minimum nodes */
    while (rbt_size(&tree2) > 0) {
        test_free(rbt_popmin(&tree2));
    }

    assert_null(rbt_popmin(&tree1));
    assert_null(rbt_popmax(&tree2));
}

static void
test_add_twice(void **state)
{
    /*
     * When adding two objects with the same key, the return value of rbt_add
     * will be the existing entry.
     */
    (void)state;

    rbt_t tree1;
    rbt_init(&tree1);

    test_obj_t *obj1 = test_malloc(sizeof(*obj1));
    test_obj_t *obj2 = test_malloc(sizeof(*obj2));

    obj1->key = 22;
    obj2->key = 22;

    void *tmp = rbt_add(&tree1, obj1);
    assert_ptr_equal(tmp, obj1);

    tmp = rbt_add(&tree1, obj2);
    assert_ptr_equal(tmp, obj1);

    test_free(rbt_popmin(&tree1));
    test_free(obj2);
}

static void
test_find_nearest(void **state)
{
    (void)state;

    rbt_t tree1;
    rbt_init(&tree1);

    // Fill the tree with keys like [0,3..27]
    for (int i = 0; i < 10; ++i) {
        test_obj_t *obj1 = test_malloc(sizeof(*obj1));
        *obj1 = (test_obj_t) {
            .key = 3 * i,
        };
        void *tmp = rbt_add(&tree1, obj1);
        assert_ptr_equal(tmp, obj1);
    }

    test_obj_t *obj = rbt_lt(&tree1, 2);
    assert_int_equal(obj->key, 0);
    obj = rbt_lt(&tree1, 4);
    assert_int_equal(obj->key, 3);
    obj = rbt_lt(&tree1, 5);
    assert_int_equal(obj->key, 3);
    obj = rbt_gt(&tree1, 5);
    assert_int_equal(obj->key, 6);

    while (rbt_size(&tree1) > 0) {
        test_free(rbt_popmax(&tree1));
    }
}

int main(void) {

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_basic_functionality),
        cmocka_unit_test(test_random_double_tree),
        cmocka_unit_test(test_add_twice),
        cmocka_unit_test(test_find_nearest),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
