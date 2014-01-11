#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../DEX.h"
#include "CompilerUtility.h"

static ArenaMemBlock *arenaHead = NULL;
static ArenaMemBlock *currentArena = NULL;
static int numArenaBlocks = 0;


void dvmCompilerHeapInit(void){
    arenaHead =
        (ArenaMemBlock *) malloc(sizeof(ArenaMemBlock) + ARENA_DEFAULT_SIZE);
    if (arenaHead == NULL) {
        printf("error:No memory left to create aot  heap memory\n");
        return ;
    }
    arenaHead->blockSize = ARENA_DEFAULT_SIZE;
    currentArena = arenaHead;
    currentArena->bytesAllocated = 0;
    currentArena->next = NULL;
    numArenaBlocks = 1;

    return ;
}


void * dvmCompilerNew(int size, bool zero)
{
    size = (size + 3) & ~3;
retry:
    /* Normal case - space is available in the current page */
    if (size + currentArena->bytesAllocated <= currentArena->blockSize) {
        void *ptr;
        ptr = &currentArena->ptr[currentArena->bytesAllocated];
        currentArena->bytesAllocated += size;
        if (zero) {
            memset(ptr, 0, size);
        }
        return ptr;
    } else {
        /*
         * See if there are previously allocated arena blocks before the last
         * reset
         */
        if (currentArena->next) {
            currentArena = currentArena->next;
            goto retry;
        }

        int  blockSize = (size < ARENA_DEFAULT_SIZE) ?
                          ARENA_DEFAULT_SIZE : size;
        /* Time to allocate a new arena */
        ArenaMemBlock *newArena = (ArenaMemBlock *)
            malloc(sizeof(ArenaMemBlock) + blockSize);
        if (newArena == NULL) {
            printf("error:Arena allocation failure");
            return NULL;
        }
        newArena->blockSize = blockSize;
        newArena->bytesAllocated = 0;
        newArena->next = NULL;
        currentArena->next = newArena;
        currentArena = newArena;
        numArenaBlocks++;
        if (numArenaBlocks > 10)
            printf("Total arena pages for JIT: %d", numArenaBlocks);
        goto retry;
    }
    return NULL;
}

void dvmInitGrowableList(GrowableList *gList, int initLength)
{
	gList->numAllocated = initLength; 
	gList->numUsed = 0; 
	gList->elemList = (void **) dvmCompilerNew(sizeof(void *) * initLength, true);
}

static void expandGrowableList(GrowableList *gList)  
{
	int newLength = gList->numAllocated;
	if (newLength < 128) {
		newLength <<= 1;   
	} else {
		newLength += 128;  
	}
	void *newArray = dvmCompilerNew(sizeof(void *) * newLength, true); 
	memcpy(newArray, gList->elemList, sizeof(void *) * gList->numAllocated); 
	gList->numAllocated = newLength;    
	gList->elemList = newArray;
}

void dvmInsertGrowableList(GrowableList *gList, void *elem)
{
	if (gList->numUsed == gList->numAllocated) { 
		expandGrowableList(gList);  
	} 
	gList->elemList[gList->numUsed++] = elem; 
}
