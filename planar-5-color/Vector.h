#pragma once

#include "Mem.h"
#include <cstddef>
#include <new>
#include <utility>

template <typename T>
class Vector {
private:
    MemoryManager* _mm;
    T* _data;
    size_t _size;
    size_t _capacity;
    void grow(size_t newCap) 
    {
        T* newBuf = static_cast<T*>(_mm->allocMem(newCap * sizeof(T)));
        for (size_t i = 0; i < _size; ++i) 
        {
            new (newBuf + i) T(std::move(_data[i]));
            _data[i].~T();
        }
        if (_data != nullptr) { _mm->freeMem(_data); }
        _data = newBuf;
        _capacity = newCap;
    }
public:
    explicit Vector(MemoryManager& mm) noexcept
        : _mm(&mm), _data(nullptr), _size(0), _capacity(0) {}

    ~Vector() 
    {
        clear();
        if (_data != nullptr) { _mm->freeMem(_data); }
    }

    Vector(const Vector&) = delete;
    Vector& operator=(const Vector&) = delete;

    Vector(Vector&& other) noexcept
        : _mm(other._mm), _data(other._data), _size(other._size), _capacity(other._capacity) 
    {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }
    Vector& operator=(Vector&& other) noexcept 
    {
        if (this == &other) return *this;
        clear();
        if (_data != nullptr) _mm->freeMem(_data);

        _mm = other._mm;
        _data = other._data;
        _size = other._size;
        _capacity = other._capacity;

        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
        return *this;
    }
    void push_back(const T& value) 
    {
        if (_size == _capacity) { grow(_capacity == 0 ? 4 : _capacity * 2); }
        new (_data + _size) T(value);
        ++_size;
    }
    void push_back(T&& value) 
    {
        if (_size == _capacity) { grow(_capacity == 0 ? 4 : _capacity * 2); }
        new (_data + _size) T(std::move(value));
        ++_size;
    }
    void pop_back() noexcept 
    {
        if (_size == 0) return;
        --_size;
        _data[_size].~T();
    }
    void clear() noexcept 
    {
        for (size_t i = 0; i < _size; ++i) { _data[i].~T(); }
        _size = 0;
    }
    T& back() noexcept { return _data[_size - 1]; }
    const T& back() const noexcept { return _data[_size - 1]; }
    T& operator[](size_t idx) noexcept { return _data[idx]; }
    const T& operator[](size_t idx) const noexcept { return _data[idx]; }
    size_t size() const noexcept { return _size; }
    bool empty() const noexcept { return _size == 0; }
    MemoryManager& memoryManager() const noexcept { return *_mm; }
};