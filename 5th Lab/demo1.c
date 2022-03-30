#include <stdio.h>

#include "memlab.h"

// Take 250 MB data

// call 10 functions with two parameters x and y (x and y same data type). Each
// function will create and populate an array of 50000 elements of the same
// data type with random data, destroy the array and return.

int main(void)
{
    printf("%ld %ld\n", sizeof(freeSpaceNode), sizeof(pageTableEntry));
    printf("%ld %ld\n", sizeof(char), sizeof(int));
}