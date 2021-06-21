
# Fancy Red Black Tree Implementation

It's a red black tree with some additional flare.

## Using the Implementation

The implementation aggressively inlines with the main goal of not having to do
any function calls for comparisons between nodes.

An example of how to set this up is available in the:

rbspeed_helper.h
rbspeed_helper.c

files.

The header contains only the minimum definitions of the objects that are used
for the specific implementation as well as the specialized prototypes.

The c file is the place where the `rbtree.h` header is actually included, and
all the specialization is done.

If the library is compiled directly, ALL of the `rbtree.h` code should be
inlined into the `rbspeed_helper.c` file.
