#pragma once

#include "rbtree.h"

typedef struct test_obj test_obj_t;
struct test_obj {
    unsigned data1[16];
    int key;
    rbn_t nd;
    unsigned data2[16];
};

// Define a key type that can be used to locate entries in the tree
typedef struct test_key test_key_t;
struct test_key {
    int key;
};

// Define a static inline pure function that compares two nodes
__attribute__((pure))
static inline int
mycmp(rbn_t const*const ln, rbn_t const*const rn)
{
    test_obj_t const*const l = (void *)((unsigned char *)ln - offsetof(test_obj_t, nd));
    test_obj_t const*const r = (void *)((unsigned char *)rn - offsetof(test_obj_t, nd));
    if (l->key < r->key) {
        return -1;
    } else if (l->key > r->key) {
        return 1;
    } else {
        return 0;
    }
}

// Define a static inline pure function that compares a key and a node
__attribute__((pure))
static inline int
mykeycmp(void const*const key, rbn_t const*const rn)
{
    test_key_t const*const l = key;
    test_obj_t const*const r = (void *)((unsigned char *)rn - offsetof(test_obj_t, nd));
    if (l->key < r->key) {
        return -1;
    } else if (l->key > r->key) {
        return 1;
    } else {
        return 0;
    }
}

static inline test_obj_t *
rbt_add(rbt_t *const tree, test_obj_t *const obj)
{
    rbn_t *v = rbt_base_add(tree, &obj->nd, mycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}
static inline test_obj_t *
rbt_get(rbt_t *const tree, int key)
{
    test_key_t const k = {
        key,
    };
    rbn_t *v = rbt_base_get(tree, &k, mykeycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}
static inline test_obj_t *
rbt_rem(rbt_t *const tree, int key)
{
    test_key_t const k = {
        key,
    };
    rbn_t *v = rbt_base_rem(tree, &k, mykeycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}

static inline test_obj_t *
rbt_lt(rbt_t *const tree, int key)
{
    test_key_t const k = {
        key,
    };
    rbn_t *v = rbt_base_lt(tree, &k, mykeycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}

static inline test_obj_t *
rbt_gt(rbt_t *const tree, int key)
{
    test_key_t const k = {
        key,
    };
    rbn_t *v = rbt_base_gt(tree, &k, mykeycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}

static inline test_obj_t *
rbt_popmin(rbt_t *const tree)
{
    rbn_t *v = rbt_base_popmin(tree);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}

static inline test_obj_t *
rbt_popmax(rbt_t *const tree)
{
    rbn_t *v = rbt_base_popmax(tree);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}

static inline test_obj_t *
rbt_min(rbt_t *const tree)
{
    rbn_t *v = rbt_base_min(tree);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}

static inline test_obj_t *
rbt_max(rbt_t *const tree)
{
    rbn_t *v = rbt_base_max(tree);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(test_obj_t, nd));
}
