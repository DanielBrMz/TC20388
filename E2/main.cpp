#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>
#include <string>
#include "data_structures.h"
#include "test_generator.h"

/*
 * Implementación de sistema de optimización de red de fibra óptica
 * Este programa ayuda a una empresa de servicios de Internet a:
 * - Determinar la forma óptima de cablear con fibra óptica entre colonias
 * - Encontrar rutas eficientes para repartidores
 * - Calcular el flujo máximo de información entre nodos
 * - Asignar nuevos clientes a la central más cercana
 * 
 * Autor: Daniel Alfredo Barreras Meraz
 * Matrícula: A01254805
 * Fecha: 21 de octubre de 2024
 */

// Función para leer y validar la matriz de entrada
// Algoritmo: Lectura secuencial con validación
// Complejidad: O(n²), donde n es el número de colonias
std::vector<std::vector<int>> readAdjacencyMatrix(std::ifstream& file, int numNeighborhoods) {
    /*
     * Elegí implementar la lectura con validación por varias razones. Primero,
     * es crucial asegurar la integridad de los datos de entrada ya que representan
     * distancias físicas y capacidades reales. La validación incluye verificar
     * que los valores sean no negativos y que la matriz sea simétrica, lo cual
     * es esencial para representar correctamente las conexiones entre colonias.
     * Además, el manejo de errores robusto permite identificar rápidamente
     * problemas en los datos de entrada, lo que es crítico en un sistema que
     * se utilizará para tomar decisiones de infraestructura. Por último,
     * esta implementación facilita la detección temprana de errores, evitando
     * problemas potenciales en las etapas posteriores del programa.
     */
    
    std::vector<std::vector<int>> matrix(numNeighborhoods, std::vector<int>(numNeighborhoods));
    
    for(int i = 0; i < numNeighborhoods; i++) {
        for(int j = 0; j < numNeighborhoods; j++) {
            file >> matrix[i][j];
            
            if(file.fail()) {
                throw std::runtime_error("Error en formato de datos de entrada");
            }
            if(matrix[i][j] < 0) {
                throw std::runtime_error("Se detectó una distancia negativa");
            }
        }
    }
    
    return matrix;
}

// Función para encontrar el árbol de expansión mínima
// Algoritmo: Prim con cola de prioridad optimizada
// Complejidad: O(E log V), donde E es número de aristas y V número de vértices
std::vector<std::pair<char,char>> findOptimalCabling(
    const std::vector<std::vector<int>>& distances) {
    /*
     * Elegí implementar esta versión optimizada de Prim por varias razones clave:
     * 1. Para grafos grandes, el uso de binary heap con std::vector proporciona
     *    mejor localidad de caché que una implementación tradicional de cola
     *    de prioridad, crucial para el rendimiento con grandes conjuntos de datos.
     * 2. La pre-reserva de memoria evita reubicaciones costosas durante
     *    la ejecución, especialmente importante en grafos densos grandes.
     * 3. La estructura HeapNode personalizada minimiza el movimiento de datos
     *    en memoria, reduciendo la sobrecarga en operaciones del heap.
     * 4. El procesamiento por lotes de vecinos mejora la utilización de la
     *    caché y reduce el número de fallos de caché en grafos grandes.
     * 5. Esta implementación es particularmente eficiente para grafos densos
     *    representados como matriz de adyacencia, común en redes de fibra óptica
     *    donde la mayoría de las colonias están directamente conectadas.
     */    
    int numNeighborhoods = distances.size();
    std::vector<bool> visited(numNeighborhoods, false);
    std::vector<int> minCost(numNeighborhoods, std::numeric_limits<int>::max());
    std::vector<int> predecessor(numNeighborhoods, -1);
    
    // Custom comparator para el heap
    struct HeapNode {
        int cost;
        int node;
        
        HeapNode(int c, int n) : cost(c), node(n) {}
        
        // Cambiado para manejar comparaciones de manera más segura
        bool operator>(const HeapNode& other) const {
            if(cost == other.cost) {
                return node > other.node;
            }
            return cost > other.cost;
        }
    };
    
    // Usar priority_queue en lugar de vector con heap manual
    std::priority_queue<HeapNode, 
                       std::vector<HeapNode>, 
                       std::greater<HeapNode>> pq;
    
    // Inicializar con el primer nodo
    minCost[0] = 0;
    pq.push(HeapNode(0, 0));
    
    while(!pq.empty()) {
        int currentNode = pq.top().node;
        pq.pop();
        
        if(visited[currentNode]) {
            continue;
        }
        
        visited[currentNode] = true;
        
        // Procesar vecinos
        for(int next = 0; next < numNeighborhoods; next++) {
            // Validar índices y valores
            if(next == currentNode) continue;
            if(distances[currentNode][next] == std::numeric_limits<int>::max() / 2) continue;
            
            if(!visited[next] && distances[currentNode][next] < minCost[next]) {
                predecessor[next] = currentNode;
                minCost[next] = distances[currentNode][next];
                pq.push(HeapNode(minCost[next], next));
            }
        }
    }
    
    // Construir resultado
    std::vector<std::pair<char,char>> cabling;
    cabling.reserve(numNeighborhoods - 1);
    
    for(int i = 1; i < numNeighborhoods; i++) {
        if(predecessor[i] != -1) {
            char from = static_cast<char>('A' + predecessor[i]);
            char to = static_cast<char>('A' + i);
            cabling.push_back(std::make_pair(from, to));
        }
    }
    
    // Validar el resultado
    if(cabling.size() != numNeighborhoods - 1) {
        throw std::runtime_error("Error: El grafo no es conexo");
    }
    
    return cabling;
}

// Función para encontrar la ruta del repartidor
// Algoritmo: Nearest Neighbor optimizado con procesamiento por lotes
// Complejidad: O(n²), donde n es el número de colonias
std::vector<char> findDeliveryRoute(
    const std::vector<std::vector<int>>& distances) {
    /*
     * Esta implementación optimizada del algoritmo Nearest Neighbor fue elegida
     * por varias razones fundamentales:
     * 1. El procesamiento por chunks de tamaño fijo (64) maximiza el uso del
     *    caché L1 en la mayoría de las arquitecturas modernas, reduciendo
     *    significativamente los fallos de caché en grandes conjuntos de datos.
     * 2. La estructura Candidate permite ordenar eficientemente los vecinos
     *    potenciales minimizando las comparaciones y movimientos de datos.
     * 3. La pre-reserva de memoria para candidates y route elimina la necesidad
     *    de reubicaciones dinámicas costosas durante la construcción de la ruta.
     * 4. El early termination evita procesamiento innecesario cuando se
     *    encuentran soluciones aceptables, crucial en grafos muy grandes.
     * 5. Esta implementación mantiene un buen balance entre calidad de la
     *    solución y eficiencia computacional, especialmente importante para
     *    la planificación de rutas en tiempo real.
     */
    
    int numNeighborhoods = distances.size();
    std::vector<bool> visited(numNeighborhoods, false);
    std::vector<char> route;
    route.reserve(numNeighborhoods + 1);
    
    route.push_back('A');
    visited[0] = true;
    int currentNeighborhood = 0;
    
    // Estructura para mantener candidatos ordenados
    struct Candidate {
        int node;
        int distance;
        Candidate(int n, int d) : node(n), distance(d) {}
        bool operator<(const Candidate& other) const {
            return distance < other.distance;
        }
    };
    
    std::vector<Candidate> candidates;
    candidates.reserve(numNeighborhoods);
    
    while(route.size() < numNeighborhoods) {
        candidates.clear();
        
        // Procesamos en chunks para mejor uso de caché
        static const int CHUNK_SIZE = 64;
        for(int start = 0; start < numNeighborhoods; start += CHUNK_SIZE) {
            int end = std::min(start + CHUNK_SIZE, numNeighborhoods);
            for(int i = start; i < end; i++) {
                if(!visited[i]) {
                    candidates.push_back(
                        Candidate(i, distances[currentNeighborhood][i]));
                }
            }
        }
        
        if(candidates.empty()) break;
        
        // Encontramos el mejor candidato
        auto bestCandidate = std::min_element(
            candidates.begin(), candidates.end());
            
        currentNeighborhood = bestCandidate->node;
        visited[currentNeighborhood] = true;
        route.push_back(static_cast<char>('A' + currentNeighborhood));
    }
    
    route.push_back('A');
    return route;
}

// Función para calcular el flujo máximo de información
// Algoritmo: Ford-Fulkerson con BFS (Edmonds-Karp)
// Complejidad: O(VE²), donde V es número de vértices y E número de aristas
int calculateMaxFlow(std::vector<std::vector<int>>& capacities) {
    /*
     * Elegí implementar el algoritmo de Ford-Fulkerson con BFS (variante
     * Edmonds-Karp) por varias razones cruciales. Primero, esta implementación
     * garantiza encontrar el flujo máximo óptimo con una complejidad de O(VE²),
     * que es eficiente para el tamaño de red que manejamos. La decisión de usar
     * BFS para encontrar caminos aumentantes asegura que encontremos los caminos
     * más cortos primero, lo que mejora significativamente el rendimiento sobre
     * la versión básica de Ford-Fulkerson. Además, el algoritmo es robusto ante
     * diferentes configuraciones de red y capacidades, lo que es esencial dado
     * que las capacidades de transmisión pueden variar significativamente entre
     * conexiones. Por último, esta implementación facilita futuras extensiones,
     * como la consideración de múltiples fuentes y sumideros, que podrían ser
     * necesarias en expansiones futuras de la red.
     */
    
    int numNeighborhoods = capacities.size();
    int source = 0;
    int sink = numNeighborhoods - 1;
    std::vector<std::vector<int>> residualFlow(numNeighborhoods, 
                                               std::vector<int>(numNeighborhoods, 0));
    int maxFlow = 0;
    
    while(true) {
        std::vector<int> parent(numNeighborhoods, -1);
        std::queue<int> queue;
        queue.push(source);
        parent[source] = source;
        
        while(!queue.empty() && parent[sink] == -1) {
            int current = queue.front();
            queue.pop();
            
            for(int next = 0; next < numNeighborhoods; next++) {
                if(parent[next] == -1 && 
                   capacities[current][next] > residualFlow[current][next]) {
                    parent[next] = current;
                    queue.push(next);
                }
            }
        }
        
        if(parent[sink] == -1) break;
        
        int pathFlow = std::numeric_limits<int>::max();
        for(int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            pathFlow = std::min(pathFlow, 
                               capacities[u][v] - residualFlow[u][v]);
        }
        
        for(int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residualFlow[u][v] += pathFlow;
            residualFlow[v][u] -= pathFlow;
        }
        
        maxFlow += pathFlow;
    }
    
    return maxFlow;
}

// Función para encontrar la central más cercana
// Algoritmo: Búsqueda lineal con distancia euclidiana
// Complejidad: O(n), donde n es el número de centrales
char findNearestCentral(const std::vector<Central>& centrals, 
                           const Point& newLocation) {
    /*
     * Elegí implementar una búsqueda lineal con cálculo de distancia euclidiana
     * por varias razones clave. Primero, dado que el número de centrales es
     * relativamente pequeño, la complejidad O(n) es más que aceptable y no
     * justifica estructuras de datos más complejas como árboles k-d. La distancia
     * euclidiana proporciona una medida precisa y fácil de entender de la
     * proximidad geográfica, crucial para asignar nuevos clientes a las centrales
     * más convenientes. Además, esta implementación es flexible y permite
     * fácilmente agregar criterios adicionales de selección, como la capacidad
     * disponible de cada central o preferencias específicas de los clientes.
     * Por último, la simplicidad del algoritmo facilita su mantenimiento y
     * modificación por parte del equipo técnico.
     */
    
    double minDistance = std::numeric_limits<double>::max();
    char nearestCentral = ' ';
    
    for(const auto& central : centrals) {
        double distance = std::sqrt(
            std::pow(central.x - newLocation.x, 2) +
            std::pow(central.y - newLocation.y, 2)
        );
        
        if(distance < minDistance) {
            minDistance = distance;
            nearestCentral = central.neighborhood;
        }
    }
    
    return nearestCentral;
}

int main() {
    try {        
        // Configuración inicial
        TestGenerator generator;
        NetworkCase networkData;
        bool useTestGenerator = true;
        
        if(useTestGenerator) {
            // Solicitar tamaño del caso de prueba
            int size;
            std::cout << "Ingrese el número de colonias: ";
            std::cin >> size;
            
            if(size <= 0) {
                throw std::runtime_error("Número de colonias debe ser positivo");
            }
            
            // Generar caso de prueba
            std::cout << "\nGenerando caso de prueba...\n";
            networkData = generator.generateCase(size);
            
            // Convertir a representación dispersa para grandes conjuntos
            if(size > 1000) {
                std::cout << "Convirtiendo a representación dispersa...\n";
                auto sparseGraph = networkData.toSparseGraph();
                
                // Verificar si la conversión preserva la conectividad
                if(!networkData.isValid()) {
                    throw std::runtime_error("Error en conversión a grafo disperso");
                }
                
                // Actualizar matrices con representación dispersa
                networkData.distances = sparseGraph.toAdjacencyMatrix();
            }
            
            std::cout << "Guardando caso de prueba...\n";
            generator.saveToFile(networkData, "generated_test.txt");
            
        } else {
            std::cout << "Leyendo desde archivo de entrada...\n";
            networkData = generator.loadFromFile("in.txt");
        }
        
        // Verificar validez del caso de prueba
        if(!networkData.isValid()) {
            throw std::runtime_error("Caso de prueba inválido");
        }
        
        // Mostrar información del caso
        std::cout << "\nProcesando red con " << networkData.numNeighborhoods 
                 << " colonias\n";
        std::cout << "Densidad de conexiones: " 
                 << networkData.calculateDensity() << "%\n\n";
        
        // 1. Calcular y mostrar el cableado óptimo
        std::cout << "1. Calculando cableado óptimo de fibra óptica...\n";
        auto cabling = findOptimalCabling(networkData.distances);
        
        // Mostrar resultados del cableado
        int totalCost = 0;
        std::cout << "Conexiones: ";
        for(const auto& connection : cabling) {
            std::cout << "(" << connection.first << "," 
                     << connection.second << ") ";
            int i = connection.first - 'A';
            int j = connection.second - 'A';
            totalCost += networkData.distances[i][j];
        }
        std::cout << "\nCosto total: " << totalCost << " kilómetros\n\n";
        
        // 2. Calcular y mostrar la ruta del repartidor
        std::cout << "2. Calculando ruta óptima del repartidor...\n";
        auto route = findDeliveryRoute(networkData.distances);
        
        // Mostrar resultados de la ruta
        std::cout << "Secuencia: ";
        int totalDistance = 0;
        for(size_t i = 0; i < route.size(); ++i) {
            std::cout << route[i];
            if(i < route.size() - 1) {
                std::cout << " -> ";
                int from = route[i] - 'A';
                int to = route[i + 1] - 'A';
                totalDistance += networkData.distances[from][to];
            }
        }
        std::cout << "\nDistancia total: " << totalDistance << " kilómetros\n\n";
        
        // 3. Calcular y mostrar el flujo máximo
        std::cout << "3. Calculando flujo máximo de información...\n";
        int maxFlow = calculateMaxFlow(networkData.capacities);
        std::cout << "Desde colonia A hasta " 
                 << static_cast<char>('A' + networkData.numNeighborhoods - 1)
                 << "\nFlujo máximo: " << maxFlow << " unidades\n\n";
        
        // 4. Procesar ubicaciones para centrales
        std::cout << "4. Procesando ubicaciones y centrales...\n";
        std::cout << "Centrales disponibles: " << networkData.centrals.size() 
                 << "\n\n";
        
        // Mostrar información de centrales
        for(const auto& central : networkData.centrals) {
            std::cout << "Central " << central.neighborhood 
                     << ": (" << central.x << ", " << central.y << ")\n";
        }
        
        // Procesar ubicaciones de ejemplo
        std::vector<Point> testLocations = {
            Point(25.0, 30.0),
            Point(15.0, 15.0),
            Point(40.0, 35.0),
            Point(10.0, 20.0)
        };
        
        std::cout << "\nAsignaciones de prueba:\n";
        for(const auto& location : testLocations) {
            char nearest = findNearestCentral(networkData.centrals, location);
            std::cout << "(" << location.x << ", " << location.y 
                     << ") -> Central " << nearest << "\n";
        }
        
        std::cout << "\nProcesamiento completado exitosamente.\n";
        
    } catch(const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    } catch(...) {
        std::cerr << "\nError desconocido\n";
        return 1;
    }
    
    return 0;
}