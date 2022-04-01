#ifndef MEMLAB_H
#define MEMLAB_H

// Data Structures
#include <string.h>

#define FREE_SPACE_LIST_SIZE 50

#define PAGE_TABLE_ENTRY_NAME_SIZE 20
#define PAGE_TABLE_SIZE 500

#define INT 0
#define CHAR 1
#define MED_INT 2
#define BOOL 3

// Main Memory Variable
void *mem;

// Current Size of Page Table
int pageTableIndex;

// The Internal Counter to ensure Word Alignment
int internalCounter;

typedef struct freeSpaceNode
{

    int start, end;
    int prev, next;

} freeSpaceNode;

// freeSpaceNode freeSpaceList[FREE_SPACE_LIST_SIZE];
freeSpaceNode* freeSpaceList;

void addToFreeSpaceList();

void mergeFreeSpaceList();

void compact();

typedef struct pageTableEntry
{

    char name[PAGE_TABLE_ENTRY_NAME_SIZE];
    unsigned int physicalAddress;
    int type;
    int size;
    int internalCounter;
    int isMarkToDelete;

} pageTableEntry;

// pageTableEntry pageTable[PAGE_TABLE_SIZE];
pageTableEntry* pageTable;

void addPageTableEntry();

void markPageTableEntryToDelete();

// Functions

// createMem
void createMem(int MBs);

// createVar
void createVar();

// assignVar
void assignVar();

// createArr
void createArr();

// assignArr
void assignArr();

// freeElem
void freeElem();

inline int typeToSize(int type)
{
    switch (type)
    {
    case 0:
        return 32;
        break;
    
    case 1:
        return 8;
        break;

    case 2:
        return 24;
        break;

    case 3:
        return 1;
        break;
    
    default:
        break;
    }
    return -1;
}

#endif