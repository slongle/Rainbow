#include "memory.h"
#include <cstddef>

#define RAINBOW_L1_CACHE_LINE_SIZE 64

void* AllocAligned(const size_t size) { 
#if defined(RAINBOW_HAVE__ALIGNED_MALLOC)
    return _aligned_malloc(size, RAINBOW_L1_CACHE_LINE_SIZE);
#elif defined(RAINBOW_HAVE_POSIX_MEMALIGN)
    void *ptr;
    if (posix_memalign(&ptr, RAINBOW_L1_CACHE_LINE_SIZE, size) != 0) ptr = nullptr;
    return ptr;
#else
    return memalign(RAINBOW_L1_CACHE_LINE_SIZE, size);
#endif
}

void FreeAligned(void *ptr) {
    if (!ptr) return;
#if defined(RAINBOW_HAVE__ALIGNED_MALLOC)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

MemoryArena::~MemoryArena() {
    FreeAligned(currentBlock);
    for (auto &block : usedBlocks) FreeAligned(block.second);
    for (auto &block : availableBlocks) FreeAligned(block.second);
}

void* MemoryArena::Alloc(size_t nBytes) {
    size_t align = 16;
    nBytes = (nBytes + (align - 1)) & (~(align - 1));

    if (currentBlockPos + nBytes > currentAllocSize) {
        if (currentBlock) {
            usedBlocks.emplace_back(currentAllocSize, currentBlock);
            currentBlock = nullptr;
        }

        for (auto& availableBlock : availableBlocks) {
            if (availableBlock.first > nBytes) {
                currentAllocSize = availableBlock.first;
                currentBlockPos = 0;
                currentBlock = availableBlock.second;
                break;
            }
        }

        if (!currentBlock) {
            currentAllocSize = std::max(blockSize, nBytes);
            currentBlockPos = 0;
            currentBlock = AllocAligned<uint8_t>(currentAllocSize);
        }
    }

    void *ret = currentBlock + currentBlockPos;
    currentBlockPos += nBytes;
    return ret;
}

void MemoryArena::Reset() {
    currentBlockPos = currentAllocSize = 0;
    currentBlock = nullptr;
    availableBlocks.splice(availableBlocks.begin(), usedBlocks);
}
