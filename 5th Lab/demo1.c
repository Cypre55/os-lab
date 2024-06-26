#include <stdio.h>

#include "memlab.h"

// Take 250 MB data

// call 10 functions with two parameters x and y (x and y same data type). Each
// function will create and populate an array of 50000 elements of the same
// data type with random data, destroy the array and return.



int main(void)
{

    createMem(250);

    initScope();

    startScope();

    createVar("a", CHAR);

    assignVar("a", 'c');

    int value;
    char c;
    int type;
    getVar("a", &value, &c, &type);

    if (type == 0)
        printf("%d\n", value);
    else
        printf("%c\n", c);

    createVar("b", BOOL);

    assignVar("b", 0);

    getVar("b", &value, &c, &type);

    if (type == 0)
        printf("%d\n", value);
    else
        printf("%c\n", c);

    // createArr("arr", INT, 10);

    // assignArr("arr", 1, 69);

    // getArr("arr", 3, &value, &c, &type);
    // if (type == 0)
    //     printf("%d\n", value);
    // else
    //     printf("%c\n", c);



    endScope();

}