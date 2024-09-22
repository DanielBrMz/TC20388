/*
 * Generador de casos de prueba para el problema de optimización de red
 * Proporciona funciones para generar casos de prueba grandes
 * 
 * Autor: [Tu Nombre]
 * Matrícula: [Tu Matrícula]
 * Fecha: [Fecha Actual]
 */

#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <vector>
#include <random>
#include <string>

class TestGenerator {
private:
    std::mt19937 gen;
    
public:
    TestGenerator() : gen(std::random_device{}()) {}
    
    struct TestCase {
        int N;
        std::vector<std::vector<int>> distancias;
        std::vector<std::vector<int>> capacidades;
        std::vector<Central> centrales;
    };
    
    TestCase generarCasoPrueba(int N) {
        TestCase caso;
        caso.N = N;
        
        // Distribuciones para valores aleatorios
        std::uniform_int_distribution<> distDist(1, 100);
        std::uniform_int_distribution<> capDist(100, 1000);
        std::uniform_int_distribution<> coordDist(0, 1000);
        
        // Generar matriz de distancias
        caso.distancias = std::vector<std::vector<int>>(N, std::vector<int>(N, 0));
        for(int i = 0; i < N; i++) {
            for(int j = i; j < N; j++) {
                if(i == j) {
                    caso.distancias[i][j] = 0;
                } else {
                    int dist = distDist(gen);
                    caso.distancias[i][j] = dist;
                    caso.distancias[j][i] = dist;  // Matriz simétrica
                }
            }
        }
        
        // Generar matriz de capacidades
        caso.capacidades = std::vector<std::vector<int>>(N, std::vector<int>(N, 0));
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                if(i != j) {
                    caso.capacidades[i][j] = capDist(gen);
                }
            }
        }
        
        // Generar centrales (20% del tamaño de N)
        int numCentrales = N/5;
        for(int i = 0; i < numCentrales; i++) {
            char id;
            if(i < 26) {
                id = static_cast<char>('A' + i);
            } else {
                id = static_cast<char>('A' + (i % 26));
            }
            
            int x = coordDist(gen);
            int y = coordDist(gen);
            caso.centrales.push_back(Central(id, x, y));
        }
        
        return caso;
    }
    
    void guardarCasoPrueba(const TestCase& caso, const std::string& nombreArchivo) {
        std::ofstream archivo(nombreArchivo);
        
        archivo << caso.N << "\n";
        
        // Escribir matriz de distancias
        for(const auto& fila : caso.distancias) {
            for(size_t j = 0; j < fila.size(); ++j) {
                archivo << fila[j];
                if(j < fila.size() - 1) archivo << " ";
            }
            archivo << "\n";
        }
        
        // Escribir matriz de capacidades
        for(const auto& fila : caso.capacidades) {
            for(size_t j = 0; j < fila.size(); ++j) {
                archivo << fila[j];
                if(j < fila.size() - 1) archivo << " ";
            }
            archivo << "\n";
        }
        
        // Escribir información de centrales
        archivo << caso.centrales.size() << "\n";
        for(const auto& central : caso.centrales) {
            archivo << central.colonia << " " 
                   << central.x << " " 
                   << central.y << "\n";
        }
        
        archivo.close();
    }
};

#endif // TEST_GENERATOR_H