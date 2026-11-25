#pragma once

#include "Vector.h"
#include <cstddef>

struct Vertex {
    size_t id = 0;               // Индекс / номер вершины
    int color = -1;              // Цвет (-1 = не покрашена, 0..4 = цвета)
    bool removed = false;        // Снята ли вершина на этапе редукции
    int visitMark = 0;           // Метка посещения для обходов / цепей Кемпе
    Vector<size_t> neighbors;    // Списки индексов смежных вершин

    explicit Vertex(MemoryManager& mm, size_t vertexId = 0) noexcept
        : id(vertexId), neighbors(mm) {
    }

    size_t degree() const noexcept {
        return neighbors.size();
    }

    void resetState() noexcept {
        color = -1;
        removed = false;
        visitMark = 0;
    }
};

class Graph {
public:
    explicit Graph(MemoryManager& mm) noexcept
        : _mm(&mm), _vertices(mm) {
    }

    // Создаёт n вершин с индексами 0..n-1
    void resize(size_t n) {
        _vertices.clear();
        for (size_t i = 0; i < n; ++i) {
            _vertices.push_back(Vertex(*_mm, i));
        }
    }

    size_t addVertex() {
        const size_t idx = _vertices.size();
        _vertices.push_back(Vertex(*_mm, idx));
        return idx;
    }

    // Неориентированное ребро: записываем соседей в обе вершины
    void addEdge(size_t u, size_t v) {
        if (u >= _vertices.size() || v >= _vertices.size() || u == v) {
            return;
        }
        _vertices[u].neighbors.push_back(v);
        _vertices[v].neighbors.push_back(u);
    }

    // Сбрасывает цвета, отметки снятия и обходов перед новым запуском алгоритма
    void resetStates() noexcept {
        for (size_t i = 0; i < _vertices.size(); ++i) {
            _vertices[i].resetState();
        }
    }

    Vertex& vertex(size_t idx) noexcept { return _vertices[idx]; }
    const Vertex& vertex(size_t idx) const noexcept { return _vertices[idx]; }

    size_t vertexCount() const noexcept { return _vertices.size(); }
    MemoryManager& memoryManager() const noexcept { return *_mm; }

private:
    MemoryManager* _mm;
    Vector<Vertex> _vertices;
};