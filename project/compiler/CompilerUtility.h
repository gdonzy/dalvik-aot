#define ARENA_DEFAULT_SIZE 8100

typedef struct ArenaMemBlock{
	int blockSize;
	int bytesAllocated;
	struct ArenaMemBlock * next;
	char ptr[0];
} ArenaMemBlock;

typedef struct GrowableList {
    u4 numAllocated;
    u4 numUsed;
    void **elemList;
} GrowableList;

#define GET_ELEM_N(LIST,TYPE,N) (((TYPE *)LIST->elemList)[N])

void dvmCompilerHeapInit(void);

void *dvmCompilerNew(int size,bool zero);

void dvmInitGrowableList(GrowableList *gList, int initLength);  
void dvmInsertGrowableList(GrowableList *gList, void *elem);           
