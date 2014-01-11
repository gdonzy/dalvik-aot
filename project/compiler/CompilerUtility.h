#define ARENA_DEFAULT_SIZE 8100

typedef struct ArenaMemBlock{
	int blockSize;
	int bytesAllocated;
	struct ArenaMemBlock * next;
	char ptr[0];
} ArenaMemBlock;

void dvmCompilerHeapInit(void);

void *dvmCompilerNew(int size,bool zero);

typedef struct GrowableList {
	int numAllocated;
	int numUsed;
	void **elemList;
} GrowableList;

void dvmInitGrowableList(GrowableList *gList, int initLength);  
void dvmInsertGrowableList(GrowableList *gList, void *elem);           
