
#include "rbtree.h"

#include "rbspeed_helper.h"

// Define a key type that can be used to locate entries in the tree
typedef struct my_key_type myk_t;
struct my_key_type {
    int my_key;
};


// Define a static inline pure function that compares two nodes
__attribute__((pure))
static inline int
mycmp(rbn_t const*const ln, rbn_t const*const rn)
{
    my_t const*const l = (void *)((unsigned char *)ln - offsetof(my_t, ok));
    my_t const*const r = (void *)((unsigned char *)rn - offsetof(my_t, ok));
    if (l->my_key < r->my_key) {
        return -1;
    } else if (l->my_key > r->my_key) {
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
    myk_t const*const l = key;
    my_t const*const r = (void *)((unsigned char *)rn - offsetof(my_t, ok));
    if (l->my_key < r->my_key) {
        return -1;
    } else if (l->my_key > r->my_key) {
        return 1;
    } else {
        return 0;
    }
}

my_t *
rbt_add(rbt_t *const tree, my_t *const obj)
{
    rbn_t *v = rbt_base_add(tree, &obj->ok, mycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(my_t, ok));
}
my_t *
rbt_get(rbt_t *const tree, int key)
{
    myk_t const k = {
        key,
    };
    rbn_t *v = rbt_base_get(tree, &k, mykeycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(my_t, ok));
}

my_t *
rbt_rem(rbt_t *const tree, int key)
{
    myk_t const k = {
        key,
    };
    rbn_t *v = rbt_base_rem(tree, &k, mykeycmp);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(my_t, ok));
}

my_t *
rbt_popmax(rbt_t *const tree)
{
    rbn_t *v = rbt_base_popmax(tree);
    if (v == NULL) return NULL;

    return (void *)((unsigned char *)v - offsetof(my_t, ok));
}
