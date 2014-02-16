/*
 * Mark the specified bit as "set".
 *
 * Returns "false" if the bit is outside the range of the vector and we're
 * not allowed to expand.
 */
bool dvmSetBit(BitVector* pBits, int num)                                                  
{
    assert(num >= 0);
    if (num >= pBits->storageSize * (int)sizeof(u4) * 8) {
        if (!pBits->expandable)
            return false;

        int newSize = (num + 31) >> 5;
        assert(newSize > pBits->storageSize);
        pBits->storage = realloc(pBits->storage, newSize * sizeof(u4));
        memset(&pBits->storage[pBits->storageSize], 0x00,
            (newSize - pBits->storageSize) * sizeof(u4));
        pBits->storageSize = newSize;
    }

    pBits->storage[num >> 5] |= 1 << (num & 0x1f);
    return true;                  
}

/*
 * Determine whether or not the specified bit is set.
 */
bool dvmIsBitSet(const BitVector* pBits, int num)                                        
{
    assert(num >= 0 && num < (int) pBits->storageSize * (int)sizeof(u4) * 8); 

    int val = pBits->storage[num >> 5] & (1 << (num & 0x1f));
    return (val != 0); 
}



