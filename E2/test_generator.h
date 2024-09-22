#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <random>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include "data_structures.h"

class TestGenerator {
private:
    std::mt19937 gen;
    
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
        // Validar tamaño de entrada
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
            
        for(int i = 0; i < size; i++) {
            testCase.distances[i][i] = 0;
            for(const auto& edge : sparseAdj[i]) {
                testCase.distances[i][edge.first] = edge.second;
                testCase.distances[edge.first][i] = edge.second;
            }
        }
        
        // Generar capacidades
        testCase.capacities = testCase.distances;
        
        // Calcular número de centrales
        // Asegurar al menos 1 central y evitar división por cero
        size_t numCentrals = std::max(1, std::min(20, size / std::max(1, 50)));
        
        // Generar centrales
        std::uniform_int_distribution<> coordDist(0, 1000);
        
        // Calcular tamaño de la cuadrícula
        // Asegurar que gridSize sea al menos 1
        size_t gridSize = std::max(1ul, static_cast<size_t>(std::sqrt(numCentrals)));
        int stepX = 1000 / std::max(1ul, gridSize);
        int stepY = 1000 / std::max(1ul, gridSize);
        
        for(size_t i = 0; i < gridSize && testCase.centrals.size() < numCentrals; i++) {
            for(size_t j = 0; j < gridSize && testCase.centrals.size() < numCentrals; j++) {
                int x = i * stepX + stepX/2 + coordDist(gen) % std::max(1, stepX/4);
                int y = j * stepY + stepY/2 + coordDist(gen) % std::max(1, stepY/4);
                char id = static_cast<char>('A' + testCase.centrals.size());
                testCase.centrals.push_back(Central(id, x, y));
            }
        }
        
        // Validar el caso generado
        if (!testCase.isValid()) {
            throw std::runtime_error("El caso generado no es válido");
        }
        
        return testCase;
    }
    
    void saveToFile(const NetworkCase& testCase, const std::string& filename) {
        std::ofstream file(filename.c_str()); // C++11 compatible
        if(!file.is_open()) {
            throw std::runtime_error("No se pudo crear el archivo de caso de prueba");
        }
        
        file << testCase.numNeighborhoods << "\n";
        
        // Escribir matriz de distancias de forma eficiente
        for(const auto& row : testCase.distances) {
            for(size_t j = 0; j < row.size(); ++j) {
                file << row[j];
                file.put(j < row.size() - 1 ? ' ' : '\n');
            }
        }
        
        // Escribir matriz de capacidades
        for(const auto& row : testCase.capacities) {
            for(size_t j = 0; j < row.size(); ++j) {
                file << row[j];
                file.put(j < row.size() - 1 ? ' ' : '\n');
            }
        }
        
        file << testCase.centrals.size() << "\n";
        for(const auto& central : testCase.centrals) {
            file << central.neighborhood << " " 
                 << central.x << " " 
                 << central.y << "\n";
        }
        
        file.close();
    }
    
    NetworkCase loadFromFile(const std::string& filename) {
        std::ifstream file(filename.c_str()); // C++11 compatible
        if(!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo");
        }
        
        NetworkCase testCase;
        file >> testCase.numNeighborhoods;
        
        if(file.fail() || testCase.numNeighborhoods <= 0) {
            throw std::runtime_error("Número de colonias inválido");
        }
        
        // Leer matrices de forma eficiente
        auto readMatrix = [&file](int size) {
            std::vector<std::vector<int>> matrix(
                size, std::vector<int>(size));
            for(int i = 0; i < size; i++) {
                for(int j = 0; j < size; j++) {
                    if(!(file >> matrix[i][j])) {
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
        for(int i = 0; i < numCentrals; i++) {
            char id;
            double x, y;
            if(!(file >> id >> x >> y)) {
                throw std::runtime_error("Error al leer central " + 
                                       std::to_string(i));
            }
            testCase.centrals.push_back(Central(id, x, y));
        }
        
        return testCase;
    }
};

#endif 