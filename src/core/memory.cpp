#include "memory.h"

#define RAINBOW_L1_CACHE_LINE_SIZE 64

void* AllocAligned(const size_t size) {
    return _aligned_malloc(size, RAINBOW_L1_CACHE_LINE_SIZE);
}

template<typename T>
T* AllocAligned(const size_t count) {
    return (T*)AllocAligned(count * sizeof(T));
}

void FreeAligned(void *ptr) {
    if (!ptr) return;
    _aligned_free(ptr);
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
