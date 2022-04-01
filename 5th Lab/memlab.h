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

int currentScope;

typedef struct freeSpaceNode
{
    int start, end;
    int prev, next;
    int used;

} freeSpaceNode;

// freeSpaceNode* freeSpaceList;

void addToFreeSpaceList(freeSpaceNode newNode);

// void delFromFreeSpaceList();

void mergeNodes();

void getFreeMemory();

void mergeFreeSpaceList();

void printFreeSpaceList();

void compact();

typedef struct pageTableEntry
{

    char name[PAGE_TABLE_ENTRY_NAME_SIZE];
    void* physicalAddress;
    int type;
    int size;
    int scope;
    int internalCounter;
    int isMarkToDelete;

} pageTableEntry;

// pageTableEntry* pageTable;



void addPageTableEntry();

void markPageTableEntryToDelete();

typedef struct BookkeepingSegment
{
    freeSpaceNode freeSpaceList[FREE_SPACE_LIST_SIZE];
    freeSpaceNode *freeSpaceListHead;

    pageTableEntry pageTable[PAGE_TABLE_SIZE];

    // Current Size of Page Table
    int pageTableIndex;

    // The Internal Counter to ensure Word Alignment
    long long internalCounter;

    void* startMem;

    void* endMem;

    int usableWords;

} BookkeepingSegment;

BookkeepingSegment* bk;

// Functions

// createMem
void createMem(int MBs);

// createVar
void createVar(char *varName, int type);

// assignVar
void assignVar(char *varName, long long int value);

// createArr
void createArr(char *arrName, int size, int type);

// assignArr
void assignArr(char *arrName, int index, long long int value);

// freeElem
void freeElem();

int typeToSize(int type);

void initScope();

void startScope();

void endScope();

#endif