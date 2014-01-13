
#define INVALID_SREG (-1)
#define INVALID_REG (-1)

typedef struct LIR {
    int offset;
    struct LIR *next;
    struct LIR *prev;
    struct LIR *target;
} LIR;


typedef struct MIR {
    DecodedInstruction dalvikInsn;
//    unsigned int width;
    unsigned int offset;
    struct MIR *prev;
    struct MIR *next;
    struct SSARepresentation *ssaRep;
//    int OptimizationFlags;
//    int seqNum;
//    union {
//        // Used by the inlined insn from the callee to find the mother method
//        const Method *calleeMethod;
//        // Used by the inlined invoke to find the class and method pointers
//        CallsiteInfo *callsiteInfo;
//    } meta;
} MIR;

typedef struct BasicBlock {
//    int id;
//    int visited;
    u8 startOffset;
//    const Method *containingMethod;     // For blocks from the callee
//    BBType blockType;
//    bool needFallThroughBranch;         // For blocks ended due to length limit
//    bool isFallThroughFromInvoke;       // True means the block needs alignment
    MIR *firstMIRInsn;
    MIR *lastMIRInsn;
    
    LIR *firstLIRInsn;//donzy
    LIR *lastLIRInsn;//donzy
    u4 *codeBuffer;
    int sizeOfBuffer;
//    struct BasicBlock *fallThrough;
//    struct BasicBlock *taken;
    struct BasicBlock *next;            // Serial link for book keeping purposes
//    struct BasicBlockDataFlow *dataFlowInfo;
} BasicBlock;

typedef struct CompilationUnit {
//    int numInsts;
    int id_code;//donzy
    int numBlocks;
    BasicBlock *firstBB; //donzy
    BasicBlock *lastBB;//donzy
    BasicBlock *debugBB;
    CodeItem *pCodeItem;//donzy
    struct CompilationUnit *next;//donzy
//    BasicBlock **blockList;
//    const Method *method;
//    const JitTraceDescription *traceDesc;
//    LIR *firstLIRInsn;
//    LIR *lastLIRInsn;
//    LIR *wordList;
//    LIR *chainCellOffsetLIR;
//    GrowableList pcReconstructionList;
//    int headerSize;                     // bytes before the first code ptr
//    int dataOffset;                     // starting offset of literal pool
//    int totalSize;                      // header + code size
//    AssemblerStatus assemblerStatus;    // Success or fix and retry
//    int assemblerRetries;               // How many times tried to fix assembly
//    unsigned char *codeBuffer;
//    void *baseAddr;
//    bool printMe;
//    bool allSingleStep;
//    bool executionCount;                // Add code to count trace executions
//    bool hasLoop;                       // Contains a loop
//    bool hasInvoke;                     // Contains an invoke instruction
//    bool heapMemOp;                     // Mark mem ops for self verification
//    bool wholeMethod;
//    int numChainingCells[kChainingCellGap];
//    LIR *firstChainingLIR[kChainingCellGap];
//    LIR *chainingCellBottom;
      struct RegisterPool *regPool;
//    int optRound;                       // round number to tell an LIR's age
//    jmp_buf *bailPtr;
//    JitInstructionSetType instructionSet;
//    /* Number of total regs used in the whole cUnit after SSA transformation */
      int numSSARegs;
//    /* Map SSA reg i to the Dalvik[15..0]/Sub[31..16] pair. */
      GrowableList *ssaToDalvikMap;
//
//    /* The following are new data structures to support SSA representations */
//    /* Map original Dalvik reg i to the SSA[15..0]/Sub[31..16] pair */
      int *dalvikToSSAMap;                // length == method->registersSize
//    BitVector *isConstantV;             // length == numSSAReg
//    int *constantValues;                // length == numSSAReg
//
//    /* Data structure for loop analysis and optimizations */
//    struct LoopAnalysis *loopAnalysis;
//
//    /* Map SSA names to location */
      RegLocation *regLocation;
//    int sequenceNumber;
//
//    /*
//     * Set to the Dalvik PC of the switch instruction if it has more than
//     * MAX_CHAINED_SWITCH_CASES cases.
//     */
//    const u2 *switchOverflowPad;
} CompilationUnit;

typedef struct CompilationUnitList{
	CompilationUnit *header;
	CompilationUnit *tail;
}CompilationUnitList;

void cUnitInit(void);
void dumpDecodedInstruction(const DecodedInstruction *pDecInsn,int insnIdx);
void dvmCompilerAppendLIR(BasicBlock *bb, LIR *lir);
