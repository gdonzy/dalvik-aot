
typedef struct SSARepresentation {
    int numUses;
    int *uses;
    bool *fpUse;
    int numDefs;
    int *defs;
    bool *fpDef;
} SSARepresentation;
