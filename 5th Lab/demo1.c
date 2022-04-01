#include <stdio.h>

#include "memlab.h"

// Take 250 MB data

// call 10 functions with two parameters x and y (x and y same data type). Each
// function will create and populate an array of 50000 elements of the same
// data type with random data, destroy the array and return.

int main(void)
{

    createMem(10);

    initScope();

    startScope();

    createVar("a", 0);

    assignVar("b", 10);


    endScope();

}