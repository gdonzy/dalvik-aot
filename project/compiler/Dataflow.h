
typedef struct SSARepresentation {
    int numUses;
    int *uses;
    bool *fpUse;
    int numDefs;
    int *defs;
    bool *fpDef;
} SSARepresentation;

#define ENCODE_REG_SUB(r,s)             ((s<<16) | r)    
