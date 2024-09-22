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
    
    // Método helper para generar matriz dispersa
    std::vector<std::vector<std::pair<int, int>>> generateSparseMatrix(int size) {
        std::uniform_int_distribution<> edgeDist(1, 100);
        std::uniform_real_distribution<> sparseProb(0, 1);
        
        std::vector<std::vector<std::pair<int, int>>> adj(size);
        size_t maxEdges = static_cast<size_t>(std::sqrt(size));
        
        // Aseguramos que el grafo esté conectado primero
        for(int i = 0; i < size - 1; i++) {
            int weight = edgeDist(gen);
            adj[i].push_back(std::make_pair(i + 1, weight));
            adj[i + 1].push_back(std::make_pair(i, weight));
        }
        
        // Agregamos aristas adicionales de forma dispersa
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                if(i != j && sparseProb(gen) < 0.1 && 
                   adj[i].size() < maxEdges) {
                    int weight = edgeDist(gen);
                    adj[i].push_back(std::make_pair(j, weight));
                    adj[j].push_back(std::make_pair(i, weight));
                }
            }
        }
        
        return adj;
    }
    
public:
    TestGenerator() : gen(std::random_device{}()) {}
    
    NetworkCase generateCase(int size) {
        NetworkCase testCase;
        testCase.numNeighborhoods = size;
        
        // Generamos una matriz dispersa para el grafo
        auto sparseAdj = generateSparseMatrix(size);
        
        // Convertimos a matriz de adyacencia
        testCase.distances = std::vector<std::vector<int>>(
            size, std::vector<int>(size, std::numeric_limits<int>::max() / 2));
            
        for(int i = 0; i < size; i++) {
            testCase.distances[i][i] = 0;
            for(const auto& edge : sparseAdj[i]) {
                testCase.distances[i][edge.first] = edge.second;
                testCase.distances[edge.first][i] = edge.second;
            }
        }
        
        // Generamos capacidades similares a las distancias
        testCase.capacities = testCase.distances;
        
        // Generamos centrales de forma distribuida
        size_t numCentrals = static_cast<size_t>(std::min(20, size / 50));
        std::vector<Point> centroids;
        std::uniform_int_distribution<> coordDist(0, 1000);
        centroids.reserve(numCentrals);
        
        // Distribuimos las centrales en una cuadrícula
        size_t gridSize = static_cast<size_t>(std::sqrt(numCentrals));
        int stepX = 1000 / gridSize;
        int stepY = 1000 / gridSize;
        
        for(size_t i = 0; i < gridSize && centroids.size() < numCentrals; i++) {
            for(size_t j = 0; j < gridSize && centroids.size() < numCentrals; j++) {
                int x = i * stepX + stepX/2 + coordDist(gen) % (stepX/4);
                int y = j * stepY + stepY/2 + coordDist(gen) % (stepY/4);
                centroids.push_back(Point(x, y));
            }
        }
        
        for(size_t i = 0; i < centroids.size(); i++) {
            char id = static_cast<char>('A' + (i % 26));
            testCase.centrals.push_back(Central(id, centroids[i].x, centroids[i].y));
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