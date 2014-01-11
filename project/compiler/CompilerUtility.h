#define ARENA_DEFAULT_SIZE 8100


typedef enum RegLocationType {
    kLocDalvikFrame = 0,
    kLocPhysReg,
    kLocRetval,          // Return region in interpState
    kLocSpill,
} RegLocationType;

typedef struct RegLocation {
    RegLocationType location:2;
    unsigned wide:1;
    unsigned fp:1;      // Hint for float/double
    u1 lowReg:6;        // First physical register
    u1 highReg:6;       // 2nd physical register (if wide)
    s2 sRegLow;         // SSA name for low Dalvik word
} RegLocation;

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
