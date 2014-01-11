
typedef struct SSARepresentation {
    int numUses;
    int *uses;
    bool *fpUse;
    int numDefs;
    int *defs;
    bool *fpDef;
} SSARepresentation;


#define ENCODE_REG_SUB(r,s)             ((s<<16) | r)
#define DECODE_REG(v)                   (v & 0xffff)
#define DECODE_SUB(v)                   (((unsigned int) v) >> 16)
