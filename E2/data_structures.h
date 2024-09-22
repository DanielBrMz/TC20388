#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <string>

// Estructura para representar una central de servicio
struct Central {
    char neighborhood;
    double x, y;

    Central(char n, double coordX, double coordY)
        : neighborhood(n), x(coordX), y(coordY) {}
};

// Estructura para representar un punto en el espacio 2D
struct Point {
    double x, y;

    Point(double coordX, double coordY)
        : x(coordX), y(coordY) {}
};

// Estructura para almacenar un caso de prueba completo
struct NetworkCase {
    int numNeighborhoods;
    std::vector<std::vector<int>> distances;
    std::vector<std::vector<int>> capacities;
    std::vector<Central> centrals;
};

#endif