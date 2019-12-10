#ifndef TREE_REDUCE_H
#define TREE_REDUCE_H

#include <stdio.h>
#include <stdlib.h>

extern void tree_gatherv(

    );

#if 0

Tree-based reduce function

Reduces nodes by flipping higher and higher bits until
all elements of the vector are reduced down. For a given
set of processes who each have a piece of the vector.
The 1 << i th bit is flipped to find its partner, and the
lower of the two partners recieves the data:

0  000
      -> 000
1  001
              -> 000
2  010
      -> 010
3  011
                      -> 000
4  100
      -> 100
5  101
              -> 100
6  110
      -> 110
7  111

This way the root node is not throttled, but still recieves
all the data at the end of the operation.
#endif

#endif
