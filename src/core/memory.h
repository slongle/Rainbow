#ifndef __MEMORY_H
#define __MEMORY_H

#include <memory>
#include <list>
#include <algorithm>

#define ALLOCA(TYPE, COUNT) (TYPE*)alloca(COUNT * sizeof(TYPE))

#define ARENA_ALLOCA(ARENA, TYPE) new ((ARENA).Alloc(sizeof(TYPE))) TYPE

class MemoryArena {
public:
    MemoryArena(size_t m_blockSize = 1 << 18) :blockSize(m_blockSize) {}

    void *Alloc(size_t nBytes);

    void Reset();

private:
    const size_t blockSize;
    size_t currentBlockPos = 0, currentAllocSize = 0;
    uint8_t * currentBlock = nullptr;
    std::list<std::pair<size_t, uint8_t *>> usedBlocks, availableBlocks;
};

#endif // __MEMORY_H