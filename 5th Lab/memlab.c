#include <stdio.h>
#include "memlab.h"

// TODO
// Check typeToSize() because it returns the size in bits and not bytes


void createMem(int MBs)
{
    // Range of MBs

    long long Bs = MBs * 1024 * 1024;

    mem = malloc(Bs);

    pageTableIndex = 0;
    internalCounter = 0;

    // 'mem' contains the total memory, we'll reserve some space out 
    // of it for book-keeping, like the page table and holes list

    // TODO 
    // Take care of PageTable and FreeSpaceNodeList

    return;
}

void createVar(char *varName, int type)
{
    pageTableEntry newVar;
    strcpy(newVar.name, varName);
    newVar.type = type;
    newVar.size = typeToSize(type);
    newVar.isMarkToDelete = 0;
    newVar.internalCounter = internalCounter;
    internalCounter += 4;
    
    // TODO
    // find the physical address and update the entry 'newVar'

    // put it in the page table

    memcpy(&pageTable[pageTableIndex], &newVar, sizeof(pageTableEntry));
    pageTableIndex++;

    return;

}

void assignVar(char *varName, long long int value)
{
    for (int i = 0; i < pageTableIndex; i++)
    {
        if (strcmp(varName, pageTable[i].name) == 0)
        {
            if (pageTable[i].isMarkToDelete)
            {
                printf("Cannot assign this variable, it is marked for deletion.\n");
                return;
            }
            // Find if the variable can fit the value it is being assigned to

            // TODO
            // Don't know how to handle little endian and big endian
            int type = pageTable[i].type;
            if (type == 0)
            {
                if (value > (1LL << 31) - 1 || value < -(1 << 31))
                {
                    printf("Cannot assign this variable, the value is out of range.\n");
                    return;
                }
                int toAssign = value;
                memcpy(pageTable[i].physicalAddress, &toAssign, sizeof(int));
            }
            else if (type == 1)
            {
                if (value > (1LL << 7) - 1 || value < -(1 << 7))
                {
                    printf("Cannot assign this variable, the value is out of range.\n");
                    return;
                }
                char toAssign = value;
                memcpy(pageTable[i].physicalAddress, &toAssign, sizeof(char));
            }
            else if (type == 2)
            {
                // TODO
            }
            else
            {
                if (value > 1 || value < 0)
                {
                    printf("Cannot assign this variable, the value is out of range.\n");
                }
                char toAssign = 1;
                memcpy(pageTable[i].physicalAddress, &toAssign, sizeof(char));
            }
        }
    }
    return;
}

void createArr(char *arrName, int size, int type)
{
    int numWords = ((size * typeToSize(type) + 31) / 32) * 32;
    pageTableEntry newArr;
    strcpy(newArr.name, arrName);
    newArr.type = type;
    newArr.size = typeToSize(type) * size;
    newArr.isMarkToDelete = 0;
    newArr.internalCounter = internalCounter;
    internalCounter += 4 * numWords;
}

