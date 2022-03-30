#ifndef MEMLAB_H
#define MEMLAB_H

// Data Structures

#define FREE_SPACE_LIST_SIZE 50

#define PAGE_TABLE_ENTRY_NAME_SIZE 20
#define PAGE_TABLE_SIZE 500

#define INT 0
#define CHAR 1
#define MED_INT 2
#define BOOL 3

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

#endif