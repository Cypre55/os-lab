#include <stdio.h>
#include <stdlib.h>
#include "memlab.h"

// TODO
// Check typeToSize() because it returns the size in bits and not bytes
// Garbage Collection
//      If Arr or Var scope is greater than currentScope

// Compaction
//      If FreeSpaceList is full
//      Or Contiguous Space not found for making Arr

// Scope Shit



void createMem(int MBs)
{
    // Range of MBs

    long long Bs = MBs * 1000 * 1000;

    mem = malloc(Bs);
    printf("createMem: Allocated %lld Bytes.\n", Bs);

    bk = (BookkeepingSegment*) mem;
    printf("createMem: Separated %ld Bytes for Booking Keeping.\n", sizeof(BookkeepingSegment));

    bk->pageTableIndex = 0;
    bk->internalCounter = 0;

    // 'mem' contains the total memory, we'll reserve some space out 
    // of it for book-keeping, like the page table and holes list

    // TODO 
    // Take care of PageTable and FreeSpaceNodeList
    
    bk->startMem = mem + ((sizeof(BookkeepingSegment)));
    bk->endMem = bk->startMem + ((Bs - sizeof(BookkeepingSegment)) / 4) * 4;

    bk->usableWords = (bk->endMem - bk->startMem)/4;

    // bk->freeSpaceList
    bk->freeSpaceListHead = bk->freeSpaceList;
    bk->freeSpaceListHead->next = bk->freeSpaceListHead->prev =  -1;
    bk->freeSpaceListHead->start = bk->startMem;
    bk->freeSpaceListHead->end = bk->endMem;

    for (int i = 0; i < FREE_SPACE_LIST_SIZE; i++)
    {
        bk->freeSpaceList[i].used = 0;
    }
    bk->freeSpaceList[0].used = 1;

    printf("createMem: Initialized Booking Keeping data structures.\n");


    return;
}

void createVar(char *varName, int type)
{
    pageTableEntry newVar;
    strcpy(newVar.name, varName);
    newVar.type = type;
    newVar.size = typeToSize(type);
    newVar.scope = currentScope;
    newVar.isMarkToDelete = 0;
    newVar.internalCounter = bk->internalCounter;
    bk->internalCounter += 4;

    printf("createVar: Creating the variable \"%s\" of type", varName);
    switch (type)
    {
    case 0:
        printf(" int");
        break;
    
    case 1:
        printf(" char");
        break;

    case 2:
        printf(" medium int");
        break;

    case 3:
        printf(" boolean");
        break;
    
    default:
        break;
    }
    printf("\n");
    
    // TODO
    // find the physical address and update the entry 'newVar'
    // put it in the page table

    freeSpaceNode *traverse = bk->freeSpaceListHead;
    int found = 0;
    while (traverse != NULL)
    {   
        // TODO check for bits and bytes
        if (traverse->end - traverse->start > (newVar.size + 3) / 4 * 4)
        {
            // first block big enough to fit
            newVar.physicalAddress = traverse->start;
            traverse->start += (newVar.size + 3) / 4 * 4;
            found++;
            break;
        }
        // TODO check for bits and bytes
        else if (traverse->end - traverse->start == (newVar.size + 3) / 4 * 4)
        {
            newVar.physicalAddress = traverse->start;
            traverse->used = 0;
            freeSpaceNode *previous = &(bk->freeSpaceList[traverse->prev]);
            freeSpaceNode *next = &(bk->freeSpaceList[traverse->next]);
            if (previous == NULL)
            {
                bk->freeSpaceListHead = next;
            }
            else
            {
                bk->freeSpaceList[previous->next] = *next;
                if (next != NULL)
                {
                    bk->freeSpaceList[next->prev] = *previous;
                }
            }
            found++;
            break;
        }
        else
        {
            traverse = &(bk->freeSpaceList[traverse->next]);
        }
    }
    if (found == 0)
    {
        printf("Cannot allocate this object due to less memory\n");
        newVar.physicalAddress = NULL;
    }

    memcpy(&(bk->pageTable[bk->pageTableIndex]), &newVar, sizeof(pageTableEntry));
    bk->pageTableIndex++;

    return;

}

void assignVar(char *varName, long long int value)
{

    for (int i = 0; i < bk->pageTableIndex; i++)
    {
        if (strcmp(varName, bk->pageTable[i].name) == 0)
        {
            if (bk->pageTable[i].isMarkToDelete)
            {
                printf("Cannot assign this variable, it is marked for deletion.\n");
                exit(1);
            }
            // Find if the variable can fit the value it is being assigned to

            printf("assignVar: Found variable \"%s\" in pageTable.\n", varName);
            int type = bk->pageTable[i].type;
            if (type == 0)
            {
                if (value > (1LL << 31) - 1 || value < -(1LL << 31))
                {
                    printf("assignVar: Cannot assign this variable, the value is out of range.\n");
                    exit(1);
                }
                int toAssign = value;
                // printf("Physical Addr: %p\n", bk->pageTable[i].physicalAddress);
                printf("assignVar: Assigning value \"%d\" in physicalAddress %p.\n", toAssign, bk->pageTable[i].physicalAddress);
                
                memcpy(bk->pageTable[i].physicalAddress, &toAssign, sizeof(int));
            }
            else if (type == 1)
            {
                if (value > (1LL << 7) - 1 || value < -(1 << 7))
                {
                    printf("assignVar: Cannot assign this variable, the value is out of range.\n");
                    exit(1);
                }
                char toAssign = value;
                printf("assignVar: Assigning value \"%c\" in physicalAddress %p.\n", toAssign, bk->pageTable[i].physicalAddress);
                memcpy(bk->pageTable[i].physicalAddress, &toAssign, sizeof(char));
            }
            else if (type == 2)
            {
                if (value > (1LL << 23) - 1 || value < -(1 << 23))
                {
                    printf("assignVar: Cannot assign this variable, the value is out of range.\n");
                    exit(1);
                }
                int toAssign = value;
                printf("assignVar: Assigning value \"%d\" in physicalAddress %p.\n", toAssign, bk->pageTable[i].physicalAddress);
                memcpy(bk->pageTable[i].physicalAddress, &toAssign, sizeof(int));
            }
            else
            {
                if (value > 1 || value < 0)
                {
                    printf("assignVar: Cannot assign this variable, the value is out of range.\n");
                    exit(1);
                }
                char toAssign = 1;
                printf("assignVar: Assigning value \"%d\" in physicalAddress %p.\n", toAssign, bk->pageTable[i].physicalAddress);
                memcpy(bk->pageTable[i].physicalAddress, &toAssign, sizeof(char));
            }

            return;
        }
    }

    printf("Cannot find this variable\n");

    return;
}

void getVar(char* varName, int *value, char *c, int* type)
{
    for (int i = 0; i < bk->pageTableIndex; i++)
    {
        if (strcmp(varName, bk->pageTable[i].name) == 0)
        {
            if (bk->pageTable[i].isMarkToDelete)
            {
                printf("Cannot assign this variable, it is marked for deletion.\n");
                exit(1);
            }
            // Find if the variable can fit the value it is being assigned to

            // TODO
            // Don't know how to handle little endian and big endian
            printf("getVar: Found variable \"%s\" in pageTable.\n", varName);

            int type_ = bk->pageTable[i].type;
            if (type_ == 0)
            {
                *value = *( (int*) bk->pageTable[i].physicalAddress);
                *type = 0;
            }
            else if (type_ == 1)
            {
                int temp = *( (char*) bk->pageTable[i].physicalAddress);
                *c = (char) temp;
                *type = 1;
            }
            else if (type_ == 2)
            {
                *value = *( (int*) bk->pageTable[i].physicalAddress);
                *type = 0;
            }
            else
            {
                *value = *( (int*) bk->pageTable[i].physicalAddress);
                *type = 0;
            }
            return;
        }
    }

    printf("Cannot find this variable\n");

    return;
}

void createArr(char *arrName, int type, int size)
{
    int numWords = ((size * typeToSize(type) + 31) / 32) * 32;
    pageTableEntry newArr;
    strcpy(newArr.name, arrName);
    newArr.type = type;
    newArr.size = typeToSize(type) * size;

    newArr.isMarkToDelete = 0;
    newArr.scope = currentScope;
    newArr.internalCounter = bk->internalCounter;
    bk->internalCounter += 4 * numWords;

    printf("createArr: Creating the variable \"%s\" of type", arrName);
    switch (type)
    {
    case 0:
        printf(" int");
        break;
    
    case 1:
        printf(" char");
        break;

    case 2:
        printf(" medium int");
        break;

    case 3:
        printf(" boolean");
        break;
    
    default:
        break;
    }
    printf("\n");

    // TODO
    // find the physical address and update the entry 'newArr'

    // put it in the page table
    freeSpaceNode *traverse = bk->freeSpaceListHead;
    int found = 0;
    while (traverse != NULL)
    {
        // TODO check for bits and bytes
        if (traverse->end - traverse->start > (newArr.size + 3) / 4 * 4)
        {
            // first block big enough to fit
            newArr.physicalAddress = traverse->start;
            traverse->start += (newArr.size + 3) / 4 * 4;
            found++;
            break;
        }
        else if (traverse->end - traverse->start == (newArr.size + 3) / 4 * 4)
        {
            newArr.physicalAddress = traverse->start;
            traverse->used = 0;
            freeSpaceNode *previous = &(bk->freeSpaceList[traverse->prev]);
            freeSpaceNode *next = &(bk->freeSpaceList[traverse->next]);
            if (previous == NULL)
            {
                bk->freeSpaceListHead = next;
            }
            else
            {
                bk->freeSpaceList[previous->next] = *next;
                if (next != NULL)
                {
                    bk->freeSpaceList[next->prev] = *previous;
                }
            }
            found++;
            break;
        }
        else
        {
            traverse = &(bk->freeSpaceList[traverse->next]);
        }
    }
    if (found == 0)
    {
        printf("Cannot allocate this object due to less memory\n");
        newArr.physicalAddress = NULL;
    }


    memcpy(&(bk->pageTable[bk->pageTableIndex]), &newArr, sizeof(pageTableEntry));
    bk->pageTableIndex++;

    return;
}

void assignArr(char *arrName, int index, long long int value)
{
    for (int i = 0; i < bk->pageTableIndex; i++)
    {
        if (strcmp(arrName, bk->pageTable[i].name) == 0)
        {
            if (bk->pageTable[i].isMarkToDelete)
            {
                printf("Cannot assign this array, it is marked for deletion.\n");
                exit(1);
            }

            printf("assignArr: Found array \"%s\" in pageTable, need to assign Index: %d.\n", arrName, index);

            int type = bk->pageTable[i].type;
            int typeSize = typeToSize(type);
            int size = bk->pageTable[i].size;
            int noOfItems = size / typeSize;

            if (index >= noOfItems)
            {
                printf("Cannot assign this array, index out of range.\n");
                exit(1);
            }


            if (type == 0)
            {
                if (value > (1LL << 31) - 1 || value < -(1LL << 31))
                {
                    printf("Cannot assign this array, the value is out of range.\n");
                    exit(1);
                }
                printf("%d\n", index* typeSize);
                int toAssign = value;
                memcpy(bk->pageTable[i].physicalAddress + index * typeSize, &toAssign, sizeof(int));
            }
            else if (type == 1)
            {
                if (value > (1LL << 7) - 1 || value < -(1 << 7))
                {
                    printf("Cannot assign this array, the value is out of range.\n");
                    exit(1);
                }
                char toAssign = value;
                memcpy(bk->pageTable[i].physicalAddress + index * typeSize, &toAssign, sizeof(char));
            }
            else if (type == 2)
            {
                if (value > (1LL << 23) - 1 || value < -(1 << 23))
                {
                    printf("Cannot assign this array, the value is out of range.\n");
                    exit(1);
                }
                int toAssign = value;
                memcpy(bk->pageTable[i].physicalAddress + index * typeSize, &toAssign, sizeof(int));
            }
            else
            {
                if (value > 1 || value < 0)
                {
                    printf("Cannot assign this array, the value is out of range.\n");
                    exit(1);
                }
                char toAssign = 1;
                memcpy(bk->pageTable[i].physicalAddress + index * typeSize, &toAssign, sizeof(char));
            }
            return;
        }
    }

    printf("Cannot find this array\n");

    return;
}

void getArr(char* arrName, int index, int *value, char *c, int* type)
{
    for (int i = 0; i < bk->pageTableIndex; i++)
    {
        if (strcmp(arrName, bk->pageTable[i].name) == 0)
        {
            if (bk->pageTable[i].isMarkToDelete)
            {
                printf("Cannot assign this array, it is marked for deletion.\n");
                exit(1);
            }

            printf("getArr: Found array \"%s\" in pageTable, need to access Index: %d.\n", arrName, index);

            int type_ = bk->pageTable[i].type;
            int typeSize = typeToSize(type_);
            int size = bk->pageTable[i].size;
            int noOfItems = size / typeSize;

            if (index >= noOfItems)
            {
                printf("Cannot assign this array, index out of range.\n");
                exit(1);
            }


            // TODO
            // Don't know how to handle little endian and big endian
            if (type_ == 0)
            {
                printf("Index: %d\n", index);
                *value = *( (int*) bk->pageTable[i].physicalAddress + index * typeSize);
                *type = 0;
            }
            else if (type_ == 1)
            {
                int temp = *( (int*) bk->pageTable[i].physicalAddress);
                *c = (char) temp;
                *type = 1;
            }
            else if (type_ == 2)
            {
                *value = *( (int*) bk->pageTable[i].physicalAddress);
                *type = 0;
            }
            else
            {
                *value = *( (int*) bk->pageTable[i].physicalAddress);
                *type = 0;
            }
            return;
        }
    }

    printf("Cannot find this array\n");

    return;
}

void freeElem()
{
    for (int i = 0; i < bk->pageTableIndex; i++)
    {
        if (bk->pageTable[i].isMarkToDelete)
        {
            char name[PAGE_TABLE_ENTRY_NAME_SIZE];
            void* physicalAddress;
            int type;
            int size;
            int scope;
            int internalCounter;
            int isMarkToDelete;

            // bk->pageTable[i].name = "\0";
            bk->pageTable[i].physicalAddress = NULL;
            bk->pageTable[i].type = -1;
            bk->pageTable[i].size = -1;
            bk->pageTable[i].scope = -1;
            bk->pageTable[i].internalCounter = 0;
            bk->pageTable[i].isMarkToDelete = 0;

            // TODO
            // Add to Free Space List
        }
    }
}

int typeToSize(int type)
{
    switch (type)
    {
    case 0:
        return 4;
        break;
    
    case 1:
        return 1;
        break;

    case 2:
        return 3;
        break;

    case 3:
        return 1;
        break;
    
    default:
        break;
    }
    return -1;
}

void initScope()
{
    currentScope = 0;
    printf("initScope: Initialized Scope to 0.\n");
}
void startScope()
{
    currentScope++;
    printf("startScope: Started new Scope with depth %d.\n", currentScope);
}

void endScope()
{
    currentScope--;
    printf("endScope: Ended Scope with depth %d.\n", currentScope+1);
}

