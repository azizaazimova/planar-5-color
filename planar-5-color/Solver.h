#pragma once
#include "Graph.h"

class Solver {
private:
	MemoryManager& _mm;
	char* colors;
public:
	explicit Solver(MemoryManager& mm) noexcept : _mm(mm) {};
	void Kempe();
	void Chiba();
};