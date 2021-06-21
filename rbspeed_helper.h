#pragma once

#include "rbttype.h"

// Define the base type that contains an embedded node
typedef struct my_type my_t;
struct my_type {
    rbn_t ok;
    int my_key;
};

// Define a key type that can be used to locate entries in the tree
typedef struct my_key_type myk_t;
struct my_key_type {
    int my_key;
};

my_t *rbt_add(rbt_t *const tree, my_t *const obj);
my_t *rbt_get(rbt_t *const tree, int key);
my_t *rbt_rem(rbt_t *const tree, int key);