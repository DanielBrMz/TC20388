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
    
    /*
     * Elegí usar Mersenne Twister como generador de números aleatorios por:
     * 1. Mayor calidad estadística que rand()
     * 2. Período muy largo, ideal para generar muchos datos
     * 3. Eficiencia en la generación de números
     * 4. Resultados consistentes entre plataformas
     */
    
public:
    TestGenerator() : gen(std::random_device{}()) {}
    
    NetworkCase generateCase(int size) {
        /*
         * Elegí generar los casos de prueba con estas características por:
         * 1. Distancias realistas para una ciudad (1-100 km)
         * 2. Capacidades que reflejan variaciones significativas (100-1000)
         * 3. Distribución uniforme de centrales (20% del total de colonias)
         * 4. Coordenadas en un espacio manejable (0-1000)
         */
        
        NetworkCase testCase;
        testCase.numNeighborhoods = size;
        
        // Configurar distribuciones para valores aleatorios
        std::uniform_int_distribution<> distDist(1, 100);     // Distancias: 1-100 km
        std::uniform_int_distribution<> capDist(100, 1000);   // Capacidades: 100-1000
        std::uniform_int_distribution<> coordDist(0, 1000);   // Coordenadas: 0-1000
        
        // Generar matriz de distancias simétrica
        testCase.distances = std::vector<std::vector<int>>(
            size, std::vector<int>(size, 0));
            
        for(int i = 0; i < size; i++) {
            for(int j = i; j < size; j++) {
                if(i == j) {
                    testCase.distances[i][j] = 0;
                } else {
                    int dist = distDist(gen);
                    testCase.distances[i][j] = dist;
                    testCase.distances[j][i] = dist;
                }
            }
        }
        
        // Generar matriz de capacidades
        testCase.capacities = std::vector<std::vector<int>>(
            size, std::vector<int>(size, 0));
            
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                if(i != j) {
                    testCase.capacities[i][j] = capDist(gen);
                }
            }
        }
        
        // Generar centrales (20% del total de colonias)
        int numCentrals = size/5;
        for(int i = 0; i < numCentrals; i++) {
            char id = static_cast<char>('A' + (i % 26));
            double x = coordDist(gen);
            double y = coordDist(gen);
            testCase.centrals.push_back(Central(id, x, y));
        }
        
        return testCase;
    }
    
    void saveToFile(const NetworkCase& testCase, const std::string& filename) {
        std::ofstream file(filename);
        if(!file.is_open()) {
            throw std::runtime_error("No se pudo crear el archivo de caso de prueba");
        }
        
        file << testCase.numNeighborhoods << "\n";
        
        // Escribir matriz de distancias
        for(const auto& row : testCase.distances) {
            for(size_t j = 0; j < row.size(); ++j) {
                file << row[j];
                if(j < row.size() - 1) file << " ";
            }
            file << "\n";
        }
        
        // Escribir matriz de capacidades
        for(const auto& row : testCase.capacities) {
            for(size_t j = 0; j < row.size(); ++j) {
                file << row[j];
                if(j < row.size() - 1) file << " ";
            }
            file << "\n";
        }
        
        // Escribir información de centrales
        file << testCase.centrals.size() << "\n";
        for(const auto& central : testCase.centrals) {
            file << central.neighborhood << " " 
                 << central.x << " " 
                 << central.y << "\n";
        }
        
        file.close();
    }
    
    NetworkCase loadFromFile(const std::string& filename) {
        NetworkCase testCase;
        std::ifstream file(filename);
        
        if(!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo de caso de prueba");
        }
        
        // Leer número de colonias
        file >> testCase.numNeighborhoods;
        if(file.fail() || testCase.numNeighborhoods <= 0) {
            throw std::runtime_error("Número de colonias inválido en archivo");
        }
        
        // Leer matriz de distancias
        testCase.distances = readMatrix(file, testCase.numNeighborhoods);
        
        // Leer matriz de capacidades
        testCase.capacities = readMatrix(file, testCase.numNeighborhoods);
        
        // Leer centrales
        int numCentrals;
        file >> numCentrals;
        
        for(int i = 0; i < numCentrals; i++) {
            char id;
            double x, y;
            file >> id >> x >> y;
            
            if(file.fail()) {
                throw std::runtime_error("Error al leer datos de central " + 
                                       std::to_string(i + 1));
            }
            
            testCase.centrals.push_back(Central(id, x, y));
        }
        
        file.close();
        return testCase;
    }
    
private:
    std::vector<std::vector<int>> readMatrix(std::ifstream& file, int size) {
        std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
        
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                file >> matrix[i][j];
                if(file.fail()) {
                    throw std::runtime_error("Error al leer matriz en posición (" + 
                                           std::to_string(i) + "," + 
                                           std::to_string(j) + ")");
                }
                if(matrix[i][j] < 0) {
                    throw std::runtime_error("Valor negativo no permitido en matriz");
                }
            }
        }
        
        return matrix;
    }
};

#endif 