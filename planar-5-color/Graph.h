#pragma once

#include "Mem.h"
#include "Vector.h"
#include <cstddef>
#include <istream>
#include <stdexcept>

class Vertex {
private:
    size_t _id;
    int _color;
    bool _removed;
    int _visitMark;
    Vector<size_t> _neighbors;
public:
    explicit Vertex(MemoryManager& mm, size_t vertexId = 0) noexcept
        : _id(vertexId),
        _color(-1),
        _removed(false),
        _visitMark(0),
        _neighbors(mm) {
    }

    size_t id() const noexcept { return _id; }

    int color() const noexcept { return _color; }
    void setColor(int c) noexcept { _color = c; }
    bool isColored() const noexcept { return _color >= 0; }

    bool isRemoved() const noexcept { return _removed; }
    void setRemoved(bool r) noexcept { _removed = r; }

    int visitMark() const noexcept { return _visitMark; }
    void setVisitMark(int mark) noexcept { _visitMark = mark; }

    bool hasNeighbor(size_t neighborId) const noexcept {
        for (size_t i = 0; i < _neighbors.size(); ++i) {
            if (_neighbors[i] == neighborId) {
                return true;
            }
        }
        return false;
    }

    void addNeighbor(size_t neighborId) {
        if (!hasNeighbor(neighborId)) {
            _neighbors.push_back(neighborId);
        }
    }

    size_t degree() const noexcept { return _neighbors.size(); }
    size_t neighborCount() const noexcept { return _neighbors.size(); }
    size_t neighbor(size_t idx) const noexcept { return _neighbors[idx]; }

    void resetState() noexcept {
        _color = -1;
        _removed = false;
        _visitMark = 0;
    }
};

class Graph {
private:
    Vector<Vertex> _vertices;
public:
    explicit Graph(MemoryManager& mm) noexcept
        : _vertices(mm) {
    }

    explicit Graph(MemoryManager& mm, std::istream& in)
        : _vertices(mm) {
        loadFromStream(in);
    }

    void loadFromStream(std::istream& in) {
        size_t n = 0;
        if (!(in >> n)) {
            throw std::runtime_error("Graph::loadFromStream: failed to read vertex count");
        }

        resize(n);

        size_t u = 0;
        size_t v = 0;
        while (in >> u >> v) {
            addEdge(u, v);
        }
    }

    void resize(size_t n) {
        _vertices.clear();
        for (size_t i = 0; i < n; ++i) {
            _vertices.push_back(Vertex(_vertices.memoryManager(), i));
        }
    }

    size_t addVertex() {
        const size_t idx = _vertices.size();
        _vertices.push_back(Vertex(_vertices.memoryManager(), idx));
        return idx;
    }

    bool hasEdge(size_t u, size_t v) const noexcept {
        if (u >= _vertices.size() || v >= _vertices.size()) {
            return false;
        }
        return _vertices[u].hasNeighbor(v);
    }

    void addEdge(size_t u, size_t v) {
        if (u >= _vertices.size() || v >= _vertices.size() || u == v) {
            return;
        }
        _vertices[u].addNeighbor(v);
        _vertices[v].addNeighbor(u);
    }

    // Число соседей, которые ещё не сняты на этапе редукции
    size_t activeDegree(size_t u) const noexcept {
        const Vertex& v = _vertices[u];
        size_t count = 0;
        for (size_t i = 0; i < v.neighborCount(); ++i) {
            if (!_vertices[v.neighbor(i)].isRemoved()) {
                ++count;
            }
        }
        return count;
    }

    void resetStates() noexcept {
        for (size_t i = 0; i < _vertices.size(); ++i) {
            _vertices[i].resetState();
        }
    }

    Vertex& vertex(size_t idx) noexcept { return _vertices[idx]; }
    const Vertex& vertex(size_t idx) const noexcept { return _vertices[idx]; }

    size_t vertexCount() const noexcept { return _vertices.size(); }
    MemoryManager& memoryManager() const noexcept { return _vertices.memoryManager(); }

};