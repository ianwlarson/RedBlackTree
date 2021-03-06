#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include "rbttype.h"

__attribute__((pure))
static inline size_t
rbt_size(rbt_t const*const p_tree)
{
    return p_tree->m_size;
}

static inline void
right_rotate(rbt_t *const T, rbn_t *const x)
{
    /*
     * Right Rotate with the pivot point as the branch leading to x
     *
     *        R                R
     *       / \              / \
     *      z   x            z   y
     *         / \     ->       / \
     *        y   c            a   x
     *       / \                  / \
     *      a   b                b   c
     *
     */
    rbn_t *const y = x->lc;
    x->lc = y->rc;
    if (y->rc != &T->m_nil) {
        y->rc->p = x;
    }
    y->p = x->p;
    if (x->p == &T->m_nil) {
        T->m_top = y;
    } else if (x == x->p->lc) {
        x->p->lc = y;
    } else {
        x->p->rc = y;
    }
    y->rc = x;
    x->p = y;
}

static inline void
left_rotate(rbt_t *const T, rbn_t *const x)
{
    /*
     * Left Rotate with the pivot point as the branch leading to x
     *
     *        R              R
     *       / \            / \
     *      x   z          y   z
     *     / \       ->   / \
     *    a   y          x   c
     *       / \        / \
     *      b   c      a   b
     *
     */
    rbn_t *const y = x->rc;
    x->rc = y->lc;
    if (y->lc != &T->m_nil) {
        y->lc->p = x;
    }
    y->p = x->p;
    if (x->p == &T->m_nil) {
        T->m_top = y;
    } else if (x == x->p->lc) {
        x->p->lc = y;
    } else {
        x->p->rc = y;
    }
    y->lc = x;
    x->p = y;
}

static inline void
rb_insert_fixup(rbt_t *const tree, rbn_t *z)
{
    while (z->p->color == RED) {

        if (z->p == z->p->p->lc) {
            rbn_t *const y = z->p->p->rc;
            if (y->color == RED) {
                /* case 1 */
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else {
                if (z == z->p->rc) {
                    /* case 2 */
                    z = z->p;
                    left_rotate(tree, z);
                }
                /* case 3 */
                z->p->color = BLACK;
                z->p->p->color = RED;
                right_rotate(tree, z->p->p);
            }
        } else {
            rbn_t *const y = z->p->p->lc;
            if (y->color == RED) {
                /* case 1 */
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else {
                if (z == z->p->lc) {
                    /* case 2 */
                    z = z->p;
                    right_rotate(tree, z);
                }
                /* case 3 */
                z->p->color = BLACK;
                z->p->p->color = RED;
                left_rotate(tree, z->p->p);
            }
        }
    }
    tree->m_top->color = BLACK;
}

static inline rbn_t *
rbt_base_add(rbt_t *const tree, rbn_t *const z, rbtcmp_t const cmpfunc)
{
    z->p = &tree->m_nil;
    z->lc = &tree->m_nil;
    z->rc = &tree->m_nil;
    z->color = RED;

    rbn_t *y = &tree->m_nil;
    rbn_t *x = tree->m_top;
    /* find insertion point into the tree, or an existing element */
    while (x != &tree->m_nil) {
        y = x;
        int const cmp = cmpfunc(z, x);
        if (cmp < 0)
            x = x->lc;
        else if (cmp > 0)
            x = x->rc;
        else
            return x;
    }

    /* Once we decide to insert, fixup the max/min */
    if ((tree->m_min == &tree->m_nil) ||
        (cmpfunc(z, tree->m_min) < 0)) {
        tree->m_min = z;
    }

    if ((tree->m_max == &tree->m_nil) ||
        (cmpfunc(z, tree->m_max) > 0)) {
        tree->m_max = z;
    }


    z->p = y;
    if (y == &tree->m_nil) {
        tree->m_top = z;
    } else {
        int const cmp = cmpfunc(z, y);
        if (cmp < 0)
            y->lc = z;
        else
            y->rc = z;
    }

    rb_insert_fixup(tree, z);

    ++tree->m_size;
    ++tree->m_gen;

    return z;
}

__attribute__((pure))
static inline rbn_t *
rb_find_node_by_key(rbt_t const*const tree, void const*const key, rbtkeycmp_t const cmpfunc)
{
    rbn_t *x = tree->m_top;
    for (;;) {
        if (x == &tree->m_nil)
            return NULL;

        int const cmp = cmpfunc(key, x);
        if (cmp < 0)
            x = x->lc;
        else if (cmp > 0)
            x = x->rc;
        else
            return x;
    }
}

// Gets the pointer associated with a key.
__attribute__((pure))
static inline rbn_t *
rbt_base_get(rbt_t const*const tree, void const*const key, rbtkeycmp_t const cmpfunc)
{
    rbn_t *const x = rb_find_node_by_key(tree, key, cmpfunc);
    if (x == NULL)
        return NULL;

    return x;
}

static inline void
rb_transplant(rbt_t *const tree, rbn_t *const u, rbn_t *const v)
{
    if (u->p == &tree->m_nil) {
        tree->m_top = v;
    } else if (u == u->p->lc) {
        u->p->lc = v;
    } else {
        u->p->rc = v;
    }
    v->p = u->p;
}

__attribute__((pure))
static inline rbn_t *
tree_minimum(rbt_t const*const T, rbn_t *x)
{
    while (x->lc != &T->m_nil)
        x = x->lc;

    return x;
}

__attribute__((pure))
static inline rbn_t *
tree_maximum(rbt_t const*const T, rbn_t *x)
{
    while (x->rc != &T->m_nil)
        x = x->rc;

    return x;
}

static inline void
rb_delete_fixup(rbt_t *const tree, rbn_t *x)
{
    while ((x != tree->m_top) && (x->color == BLACK)) {
        if (x == x->p->lc) {
            rbn_t *w = x->p->rc;
            if (w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                left_rotate(tree, x->p);
                w = x->p->rc;
            }
            if ((w->lc->color == BLACK) && (w->rc->color == BLACK)) {
                w->color = RED;
                x = x->p;
            } else {
                if (w->rc->color == BLACK) {
                    w->lc->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x->p->rc;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->rc->color = BLACK;
                left_rotate(tree, x->p);
                x = tree->m_top;
            }
        } else {
            rbn_t *w = x->p->lc;
            if (w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                right_rotate(tree, x->p);
                w = x->p->lc;
            }
            if ((w->rc->color == BLACK) && (w->lc->color == BLACK)) {
                w->color = RED;
                x = x->p;
            } else {
                if (w->lc->color == BLACK) {
                    w->rc->color = BLACK;
                    w->color = RED;
                    left_rotate(tree, w);
                    w = x->p->lc;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->lc->color = BLACK;
                right_rotate(tree, x->p);
                x = tree->m_top;
            }
        }
    }

    x->color = BLACK;
}

static inline void
rb_base_delete(rbt_t *const tree, rbn_t *const z)
{
    if (z == tree->m_min) {
        tree->m_min = (z->rc != &tree->m_nil) ? z->rc : z->p;
    }
    if (z == tree->m_max) {
        tree->m_max = (z->lc != &tree->m_nil) ? z->lc : z->p;
    }

    rbn_t *y = z;
    int y_orig_color = y->color;
    rbn_t *x;
    if (z->lc == &tree->m_nil) {
        x = z->rc;
        rb_transplant(tree, z, z->rc);
    } else if (z->rc == &tree->m_nil) {
        x = z->lc;
        rb_transplant(tree, z, z->lc);
    } else {
        y = tree_minimum(tree, z->rc);
        y_orig_color = y->color;
        x = y->rc;
        if (y->p == z) {
            x->p = y;
        } else {
            rb_transplant(tree, y, y->rc);
            y->rc = z->rc;
            y->rc->p = y;
        }
        rb_transplant(tree, z, y);
        y->lc = z->lc;
        y->lc->p = y;
        y->color = z->color;
    }

    if (y_orig_color == BLACK)
        rb_delete_fixup(tree, x);

    z->p = NULL;
    z->rc = NULL;
    z->lc = NULL;

    tree->m_size--;
    tree->m_gen++;
}

static inline rbn_t *
rbt_base_rem(rbt_t *const tree, void const*const key, rbtkeycmp_t cmpfunc)
{
    rbn_t *const x = rb_find_node_by_key(tree, key, cmpfunc);

    if (x == NULL) {
        return NULL;
    }

    rb_base_delete(tree, x);

    return x;
}

static inline rbn_t *
rbt_base_popmin(rbt_t *const tree)
{
    rbn_t *const x = tree->m_min;

    if (x == &tree->m_nil) {
        return NULL;
    }

    rb_base_delete(tree, x);

    return x;
}

static inline rbn_t *
rbt_base_popmax(rbt_t *const tree)
{
    rbn_t *const x = tree->m_max;

    if (x == &tree->m_nil) {
        return NULL;
    }

    rb_base_delete(tree, x);

    return x;
}


static inline rbn_t *
rbt_base_min(rbt_t const*const tree)
{
    if (tree->m_min == &tree->m_nil) {
        return NULL;
    }

    return tree->m_min;
}

static inline rbn_t *
rbt_base_max(rbt_t const*const tree)
{
    if (tree->m_max == &tree->m_nil) {
        return NULL;
    }

    return tree->m_max;
}

static inline rbn_t *
rbt_base_lt(rbt_t const*const tree, void const*const key, rbtkeycmp_t const cmpfunc)
{
    rbn_t *x = tree->m_top;
    rbn_t *y = x;
    if (x == &tree->m_nil)
        return NULL;

    int cmp = cmpfunc(key, tree->m_min);
    // If all elements in the tree are greater than or equal to the key, return
    // NULL
    if (cmp <= 0) {
        return NULL;
    }

    while (x != &tree->m_nil) {
        y = x;
        cmp = cmpfunc(key, x);
        if (cmp < 0) {
            x = x->lc;
        } else if (cmp > 0) {
            x = x->rc;
        } else {
            return NULL;
        }
    }

    assert(x == &tree->m_nil);
    for (;;) {
        cmp = cmpfunc(key, y);
        if (cmp > 0) {
            // y is less than key, done!
            break;
        }
        y = y->p;
    }

    return y;
}

static inline rbn_t *
rbt_base_gt(rbt_t const*const tree, void const*const key, rbtkeycmp_t const cmpfunc)
{
    rbn_t *x = tree->m_top;
    rbn_t *y = x;
    if (x == &tree->m_nil) {
        return NULL;
    }

    int cmp = cmpfunc(key, tree->m_max);
    // If all elements in the tree are less than or equal to the key, return
    // NULL
    if (cmp >= 0) {
        return NULL;
    }

    while (x != &tree->m_nil) {
        y = x;
        cmp = cmpfunc(key, x);
        if (cmp < 0) {
            x = x->lc;
        } else if (cmp > 0) {
            x = x->rc;
        } else {
            return NULL;
        }
    }

    assert(x == &tree->m_nil);
    for (;;) {
        cmp = cmpfunc(key, y);
        if (cmp < 0) {
            // y is greater than key, done!
            break;
        }
        y = y->p;
    }
    return y;
}

__attribute__((pure))
static inline rbn_t *
rbt_base_next(rbt_t *const tree, rbn_t *const x, rbtcmp_t const cmpfunc)
{
    rbn_t *y = NULL;

    if (x->rc != &tree->m_nil) {
        y = tree_minimum(tree, x->rc);
    } else {
        // No right child, we need to traverse up the tree until we find a
        // larger node
        y = x;
        for (;;) {
            y = y->p;
            if (y == &tree->m_nil) {
                break;
            }
            int cmp = cmpfunc(x, y);
            if (cmp < 0) {
                break;
            }
        }
    }

    if (y == &tree->m_nil) {
        return NULL;
    }

    return y;
}

__attribute__((pure))
static inline rbn_t *
rbt_base_prev(rbt_t *const tree, rbn_t *const x, rbtcmp_t const cmpfunc)
{
    rbn_t *y = &tree->m_nil;

    if (x->lc != &tree->m_nil) {
        y = tree_maximum(tree, x->lc);
    } else {
        // No left child, we need to traverse up the tree until we find a
        // smaller node
        y = x;
        for (;;) {
            y = y->p;
            if (y == &tree->m_nil) {
                break;
            }
            int cmp = cmpfunc(x, y);
            if (cmp > 0) {
                break;
            }
        }
    }

    if (y == &tree->m_nil) {
        return NULL;
    }

    return y;
}

