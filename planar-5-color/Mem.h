#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

class MemoryManager {
public:
    struct Stats {
        uint64_t allocCount = 0;          // Число выделений
        uint64_t freeCount = 0;           // Число освобождений
        uint64_t currentBytes = 0;        // Сейчас занято байт
        uint64_t peakBytes = 0;           // Пик занятых байт
        uint64_t totalAllocatedBytes = 0; // Всего выделено байт
    };

    explicit MemoryManager(size_t maxBytes = static_cast<size_t>(-1)) noexcept
        : _maxBytes(maxBytes),
        _blocks(nullptr),
        _blockCount(0),
        _blockCapacity(0),
        _stats{} {}

    ~MemoryManager() 
    {
        std::free(_blocks);
        _blocks = nullptr;
        _blockCount = 0;
        _blockCapacity = 0;
    }

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    size_t maxBytes() const noexcept { return _maxBytes; }

    void* allocMem(size_t sz) {
        if (sz == 0) return nullptr;

        if (_stats.currentBytes + static_cast<uint64_t>(sz) > static_cast<uint64_t>(_maxBytes)) { throw std::bad_alloc(); }

        if (!growBlocks(_blockCount + 1)) { throw std::bad_alloc(); }

        void* ptr = ::operator new(sz);
        _blocks[_blockCount++] = Block{ ptr, sz };

        ++_stats.allocCount;
        _stats.currentBytes += static_cast<uint64_t>(sz);
        _stats.totalAllocatedBytes += static_cast<uint64_t>(sz);
        _stats.peakBytes = std::max(_stats.peakBytes, _stats.currentBytes);

        return ptr;
    }

    void freeMem(void* ptr) noexcept {
        if (ptr == nullptr) return;

        size_t freedSize = 0;
        for (size_t i = 0; i < _blockCount; ++i) 
        {
            if (_blocks[i].ptr == ptr) 
            {
                freedSize = _blocks[i].size;
                _blocks[i] = _blocks[_blockCount - 1];
                --_blockCount;
                break;
            }
        }

        ::operator delete(ptr);

        ++_stats.freeCount;
        const uint64_t freed = static_cast<uint64_t>(freedSize);
        _stats.currentBytes = (freed <= _stats.currentBytes) ? (_stats.currentBytes - freed) : 0;
    }

    void resetStats() noexcept 
    {
        const uint64_t cur = _stats.currentBytes;
        _stats = Stats{};
        _stats.currentBytes = cur;
        _stats.peakBytes = cur;
    }

    const Stats& getStats() const noexcept { return _stats; }

private:
    struct Block {
        void* ptr;
        size_t size;
    };

    size_t _maxBytes;
    Block* _blocks;
    size_t _blockCount;
    size_t _blockCapacity;
    Stats _stats;

    bool growBlocks(size_t needed) noexcept 
    {
        if (needed <= _blockCapacity) return true;

        size_t newCap = (_blockCapacity == 0) ? 16 : _blockCapacity * 2;
        while (newCap < needed) newCap *= 2;

        void* raw = std::realloc(_blocks, newCap * sizeof(Block));
        if (!raw) return false;

        _blocks = static_cast<Block*>(raw);
        _blockCapacity = newCap;
        return true;
    }
};