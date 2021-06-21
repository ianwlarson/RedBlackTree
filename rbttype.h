#pragma once

#include <stdint.h>

#define RED 1
#define BLACK 0

// Embeddable node
typedef struct red_black_tree_node rbn_t;

struct red_black_tree_node {
    rbn_t *p;
    rbn_t *lc;
    rbn_t *rc;
    int color;
#if UINTPTR_MAX == 0xffffffffffffffffull
    // It's sort of pointless to include this but it's good to be explicit that
    // this field will be present. The implementation doesn't touch it so it
    // could be used to store extra information (maybe typing information or
    // something, on 64-bit)
    int reserved;
#endif
};

typedef int (*rbtcmp_t)(rbn_t const*, rbn_t const*);
typedef int (*rbtkeycmp_t)(void const*, rbn_t const*);

typedef struct red_black_tree rbt_t;

struct red_black_tree {
    rbn_t m_nil;
    rbn_t *m_top;           // top of the tree
    size_t m_size;
    rbn_t *m_min;
    rbn_t *m_max;
    unsigned m_gen; /* generation is used for iterators */
};

static inline void
rbt_init(rbt_t *const p_tree)
{
    *p_tree = (rbt_t) {
        .m_nil = {
            .p = &p_tree->m_nil,
            .lc = &p_tree->m_nil,
            .rc = &p_tree->m_nil,
            .color = BLACK,
        },
        .m_top = &p_tree->m_nil,
        .m_size = 0,
        .m_min = &p_tree->m_nil,
        .m_max = &p_tree->m_nil,
        .m_gen = 0,
    };
}

