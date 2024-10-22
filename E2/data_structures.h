#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <string>
#include <limits>
#include <cmath>

// Estructura para representar una arista en el grafo disperso
struct Edge {
    int from, to, weight;
    
    Edge(int f = 0, int t = 0, int w = 0) 
        : from(f), to(t), weight(w) {}
    
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// Estructura para el grafo disperso
struct SparseGraph {
    std::vector<std::vector<Edge>> adj;
    int vertices;
    
    explicit SparseGraph(int v = 0) : vertices(v) {
        adj.resize(v);
    }
    
    void addEdge(int from, int to, int weight) {
        adj[from].push_back(Edge(from, to, weight));
        adj[to].push_back(Edge(to, from, weight));
    }
    
    std::vector<std::vector<int>> toAdjacencyMatrix() const {
        std::vector<std::vector<int>> matrix(
            vertices, 
            std::vector<int>(vertices, std::numeric_limits<int>::max() / 2)
        );
        
        // Inicializar diagonal
        for(int i = 0; i < vertices; ++i) {
            matrix[i][i] = 0;
        }
        
        // Poblar matriz con aristas existentes
        for(int i = 0; i < vertices; ++i) {
            for(const Edge& e : adj[i]) {
                matrix[e.from][e.to] = e.weight;
            }
        }
        
        return matrix;
    }
};

// Estructura para representar un punto en el espacio 2D
struct Point {
    double x, y;
    
    Point() : x(0), y(0) {}
    Point(double coordX, double coordY) : x(coordX), y(coordY) {}
    
    double distanceTo(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// Estructura para representar una central
struct Central {
    char neighborhood;
    double x, y;
    
    Central() : neighborhood(' '), x(0), y(0) {}
    Central(char n, double coordX, double coordY) 
        : neighborhood(n), x(coordX), y(coordY) {}
        
    double distanceTo(const Point& p) const {
        double dx = x - p.x;
        double dy = y - p.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// Estructura para caso de prueba completo
struct NetworkCase {
    int numNeighborhoods;
    std::vector<std::vector<int>> distances;
    std::vector<std::vector<int>> capacities;
    std::vector<Central> centrals;
    
    NetworkCase() : numNeighborhoods(0) {}
    
    // Método para convertir a grafo disperso
    SparseGraph toSparseGraph() const {
        SparseGraph graph(numNeighborhoods);
        
        // Convertir solo aristas con peso válido
        for(int i = 0; i < numNeighborhoods; ++i) {
            for(int j = i + 1; j < numNeighborhoods; ++j) {
                if(distances[i][j] != std::numeric_limits<int>::max() / 2 &&
                   distances[i][j] > 0) {
                    graph.addEdge(i, j, distances[i][j]);
                }
            }
        }
        
        return graph;
    }
    
    // Método para validar el caso de prueba
    bool isValid() const {
        // Verificar tamaño básico
        if(numNeighborhoods <= 0) return false;
        
        // Convertir a size_t para comparaciones seguras
        size_t n = static_cast<size_t>(numNeighborhoods);
        
        // Verificar matrices
        if(distances.size() != n ||
           capacities.size() != n) return false;
           
        // Verificar dimensiones y valores válidos
        for(size_t i = 0; i < n; ++i) {
            if(distances[i].size() != n ||
               capacities[i].size() != n) return false;
               
            // Verificar diagonal y simetría
            for(size_t j = 0; j < n; ++j) {
                if(distances[i][j] < 0) return false;
                if(i == j && distances[i][j] != 0) return false;
                if(distances[i][j] != distances[j][i]) return false;
            }
        }
        
        // Verificar conectividad básica
        std::vector<bool> visited(numNeighborhoods, false);
        std::vector<int> stack;
        stack.push_back(0);
        visited[0] = true;
        int visitCount = 1;
        
        while(!stack.empty()) {
            int current = stack.back();
            stack.pop_back();
            
            for(int i = 0; i < numNeighborhoods; ++i) {
                if(!visited[i] && distances[current][i] != std::numeric_limits<int>::max() / 2) {
                    visited[i] = true;
                    stack.push_back(i);
                    visitCount++;
                }
            }
        }
        
        // El grafo debe ser conexo
        if(visitCount != numNeighborhoods) return false;
        
        // Verificar centrales
        if(centrals.empty()) return false;
        
        return true;
    }
    
    // Método para calcular la densidad del grafo
    double calculateDensity() const {
        int edges = 0;
        for(int i = 0; i < numNeighborhoods; ++i) {
            for(int j = i + 1; j < numNeighborhoods; ++j) {
                if(distances[i][j] != std::numeric_limits<int>::max() / 2) {
                    edges++;
                }
            }
        }
        return (200.0 * edges) / (numNeighborhoods * (numNeighborhoods - 1));
    }
};

#endif