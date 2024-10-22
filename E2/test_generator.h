#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <random>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "data_structures.h"

class TestGenerator {
private:
    std::mt19937 gen;

    double clamp(double value, double min, double max) {
        return std::min(std::max(value, min), max);
    }

    std::vector<std::vector<std::pair<int, int>>> generateSparseMatrix(int size) {
        std::uniform_int_distribution<> edgeDist(1, 100);
        std::uniform_real_distribution<> sparseProb(0, 1);

        std::vector<std::vector<std::pair<int, int>>> adj(size);

        // Crear un ciclo hamiltoniano base garantizado
        for (int i = 0; i < size; i++) {
            int weight = edgeDist(gen);
            int next = (i + 1) % size;
            adj[i].push_back({next, weight});
            adj[next].push_back({i, weight});

            // Agregar algunas conexiones cruzadas para mejor conectividad
            if (i < size - 2) {
                weight = edgeDist(gen);
                int cross = (i + 2) % size;
                adj[i].push_back({cross, weight});
                adj[cross].push_back({i, weight});
            }
        }

        // Agregar aristas adicionales con probabilidad controlada
        double baseProbability = 5.0 / size; // Ajustar según necesidad

        for (int i = 0; i < size; i++) {
            for (int j = i + 3; j < size; j++) {
                if (sparseProb(gen) < baseProbability) {
                    int weight = edgeDist(gen);
                    adj[i].push_back({j, weight});
                    adj[j].push_back({i, weight});
                }
            }
        }

        return adj;
    }

public:
    TestGenerator() : gen(std::random_device{}()) {}

    NetworkCase generateCase(int size) {
        // Validar tamaño
        if (size <= 0) {
            throw std::invalid_argument("El tamaño debe ser positivo");
        }

        NetworkCase testCase;
        testCase.numNeighborhoods = size;

        // Generar matriz dispersa
        auto sparseAdj = generateSparseMatrix(size);

        // Convertir a matriz de adyacencia
        testCase.distances = std::vector<std::vector<int>>(
            size, std::vector<int>(size, std::numeric_limits<int>::max() / 2));

        for (int i = 0; i < size; i++) {
            testCase.distances[i][i] = 0;
            for (const auto &edge : sparseAdj[i]) {
                testCase.distances[i][edge.first] = edge.second;
                testCase.distances[edge.first][i] = edge.second;
            }
        }

        // Generar capacidades
        testCase.capacities = testCase.distances;

        // Calcular número de centrales - Nueva fórmula
        // Para tamaños pequeños (≤20), usar al menos 3 centrales
        // Para tamaños medianos, usar aproximadamente 20% de las colonias
        // Para tamaños grandes, limitar a un máximo razonable
        size_t numCentrals;
        if (size <= 20) {
            numCentrals = std::max(3, size / 4);
        } else if (size <= 100) {
            numCentrals = size / 5; // 20% de las colonias
        } else {
            numCentrals = std::min(50, size / 4); // Máximo 50 centrales
        }

        // Generar centrales con distribución espacial mejorada
        std::uniform_int_distribution<> coordDist(0, 1000);

        // Calcular tamaño de la cuadrícula para distribución uniforme
        size_t gridSize = static_cast<size_t>(std::sqrt(numCentrals));
        int stepX = 1000 / (gridSize + 1); // +1 para evitar bordes
        int stepY = 1000 / (gridSize + 1);

        // Generar centrales en una cuadrícula con variación aleatoria
        for (size_t i = 0; i < gridSize && testCase.centrals.size() < numCentrals; i++) {
            for (size_t j = 0; j < gridSize && testCase.centrals.size() < numCentrals; j++) {
                // Añadir variación aleatoria dentro de cada celda de la cuadrícula
                int baseX = (i + 1) * stepX;
                int baseY = (j + 1) * stepY;
                int x = baseX + coordDist(gen) % (stepX / 2) - (stepX / 4);
                int y = baseY + coordDist(gen) % (stepY / 2) - (stepY / 4);

                char id = static_cast<char>('A' + testCase.centrals.size());
                testCase.centrals.push_back(Central(id, x, y));
            }
        }

        // Si aún faltan centrales, agregar en posiciones aleatorias
        while (testCase.centrals.size() < numCentrals) {
            int x = coordDist(gen);
            int y = coordDist(gen);
            char id = static_cast<char>('A' + testCase.centrals.size());
            testCase.centrals.push_back(Central(id, x, y));
        }

        return testCase;
    }

    std::vector<Point> generateTestLocations(
        const std::vector<Central> &centrals,
        size_t numLocations = 10) {

        // Encontrar los límites del área basados en las centrales
        double minX = 1000, maxX = 0, minY = 1000, maxY = 0;
        for (const auto &central : centrals) {
            minX = std::min(minX, central.x);
            maxX = std::max(maxX, central.x);
            minY = std::min(minY, central.y);
            maxY = std::max(maxY, central.y);
        }

        // Expandir el área un 20%
        double expandX = (maxX - minX) * 0.2;
        double expandY = (maxY - minY) * 0.2;
        minX = std::max(0.0, minX - expandX);
        maxX = std::min(1000.0, maxX + expandX);
        minY = std::max(0.0, minY - expandY);
        maxY = std::min(1000.0, maxY + expandY);

        // Distribuciones para generar coordenadas
        std::uniform_real_distribution<> distX(minX, maxX);
        std::uniform_real_distribution<> distY(minY, maxY);
        std::normal_distribution<> nearDist(0, 50.0);

        std::vector<Point> locations;
        locations.reserve(numLocations);

        // Generar diferentes tipos de ubicaciones
        for (size_t i = 0; i < numLocations; ++i) {
            switch (i % 4) {
            case 0: // Ubicación aleatoria
                locations.emplace_back(distX(gen), distY(gen));
                break;

            case 1: {
                const auto& central = centrals[i % centrals.size()];
                double newX = clamp(central.x + nearDist(gen), 0.0, 1000.0);
                double newY = clamp(central.y + nearDist(gen), 0.0, 1000.0);
                locations.emplace_back(newX, newY);
                break;
            }

            case 2: { // En los bordes
                std::bernoulli_distribution edge_choice(0.5);
                if (edge_choice(gen)) {
                    locations.emplace_back(
                        edge_choice(gen) ? minX : maxX,
                        distY(gen));
                } else {
                    locations.emplace_back(
                        distX(gen),
                        edge_choice(gen) ? minY : maxY);
                }
                break;
            }

            case 3: { // En áreas sin centrales
                bool validLocation = false;
                Point p;
                int attempts = 0;
                while (!validLocation && attempts < 100) {
                    p = Point(distX(gen), distY(gen));
                    validLocation = true;
                    for (const auto &central : centrals) {
                        if (p.distanceTo(Point(central.x, central.y)) < 100.0) {
                            validLocation = false;
                            break;
                        }
                    }
                    attempts++;
                }
                if (validLocation) {
                    locations.push_back(p);
                } else {
                    // Si no encontramos ubicación después de 100 intentos, usar una aleatoria
                    locations.emplace_back(distX(gen), distY(gen));
                }
                break;
            }
            }
        }

        return locations;
    }

    void saveToFile(const NetworkCase &testCase, const std::string &filename) {
        std::ofstream file(filename.c_str()); // C++11 compatible
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo crear el archivo de caso de prueba");
        }

        file << testCase.numNeighborhoods << "\n";

        // Escribir matriz de distancias de forma eficiente
        for (const auto &row : testCase.distances) {
            for (size_t j = 0; j < row.size(); ++j) {
                file << row[j];
                file.put(j < row.size() - 1 ? ' ' : '\n');
            }
        }

        // Escribir matriz de capacidades
        for (const auto &row : testCase.capacities) {
            for (size_t j = 0; j < row.size(); ++j) {
                file << row[j];
                file.put(j < row.size() - 1 ? ' ' : '\n');
            }
        }

        file << testCase.centrals.size() << "\n";
        for (const auto &central : testCase.centrals) {
            file << central.neighborhood << " "
                 << central.x << " "
                 << central.y << "\n";
        }

        file.close();
    }

    NetworkCase loadFromFile(const std::string &filename) {
        std::ifstream file(filename.c_str()); // C++11 compatible
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo");
        }

        NetworkCase testCase;
        file >> testCase.numNeighborhoods;

        if (file.fail() || testCase.numNeighborhoods <= 0) {
            throw std::runtime_error("Número de colonias inválido");
        }

        // Leer matrices de forma eficiente
        auto readMatrix = [&file](int size) {
            std::vector<std::vector<int>> matrix(
                size, std::vector<int>(size));
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (!(file >> matrix[i][j])) {
                        throw std::runtime_error("Error al leer matriz");
                    }
                }
            }
            return matrix;
        };

        testCase.distances = readMatrix(testCase.numNeighborhoods);
        testCase.capacities = readMatrix(testCase.numNeighborhoods);

        int numCentrals;
        file >> numCentrals;

        testCase.centrals.reserve(numCentrals);
        for (int i = 0; i < numCentrals; i++) {
            char id;
            double x, y;
            if (!(file >> id >> x >> y)) {
                throw std::runtime_error("Error al leer central " +
                                         std::to_string(i));
            }
            testCase.centrals.push_back(Central(id, x, y));
        }

        return testCase;
    }
};

#endif
