#include "Graph.h"
#include "Mem.h"
#include <fstream>
#include <iostream>

int main() {
    MemoryManager mm; // Создаём свой менеджер памяти в main

    {
        std::ifstream file("graph.txt");
        if (!file.is_open()) {
            std::cerr << "Failed to open graph.txt\n";
            return 1;
        }

        Graph g(mm, file); // Граф и все списки соседей выделяются через mm

        std::cout << "Vertices: " << g.vertexCount() << "\n";
        std::cout << "Current bytes: " << mm.getStats().currentBytes << "\n";
        std::cout << "Peak bytes: " << mm.getStats().peakBytes << "\n";
    } // Здесь g уничтожается, вся память возвращается в mm

    // Проверяем, что утечек нет (currentBytes должно быть 0)
    std::cout << "After destroy current bytes: " << mm.getStats().currentBytes << "\n";
    return 0;
}