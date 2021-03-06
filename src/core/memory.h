#ifndef __MEMORY_H
#define __MEMORY_H

#include <memory>
#include <list>
#include <algorithm>

#define ALLOCA(TYPE, COUNT) (TYPE*)alloca((COUNT) * sizeof(TYPE))

#define ARENA_ALLOCA(ARENA, TYPE) new ((ARENA).Alloc(sizeof(TYPE))) TYPE

void* AllocAligned(const size_t size);

template<typename T>
T* AllocAligned(const size_t count) {
    return (T*)AllocAligned(count * sizeof(T));
}

void FreeAligned(void *ptr);

class MemoryArena {
public:
    MemoryArena(size_t m_blockSize = 1 << 18) :blockSize(m_blockSize) {}
    ~MemoryArena();

    void *Alloc(size_t nBytes);
    template<typename T>
    T* Alloc(size_t n = 1, bool runConstructor = true) {
        T* ret = (T*)Alloc(sizeof(T)*n);
        if (runConstructor) {
            for (size_t i = 0; i < n; i++)
                new (&ret[i]) T();
        }
        return ret;
    }

    void Reset();

private:
    const size_t blockSize;
    size_t currentBlockPos = 0, currentAllocSize = 0;
    uint8_t * currentBlock = nullptr;
    std::list<std::pair<size_t, uint8_t *>> usedBlocks, availableBlocks;
};

#endif // __MEMORY_H