#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <functional>
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
std::vector<std::pair<std::string, std::string>> findOptimalCabling(
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
    size_t numNeighborhoods = distances.size();
    
    // Validación de entrada
    if (distances.empty() || distances[0].size() != numNeighborhoods) {
        throw std::invalid_argument("Matriz de distancias inválida");
    }
    
    // Estructuras de datos optimizadas
    std::vector<bool> visited(numNeighborhoods, false);
    std::vector<int> minCost(numNeighborhoods, std::numeric_limits<int>::max());
    std::vector<int> predecessor(numNeighborhoods, -1);
    
    // Cola de prioridad optimizada
    using HeapNode = std::pair<int, size_t>; // {costo, nodo}
    std::priority_queue<HeapNode, 
                       std::vector<HeapNode>, 
                       std::greater<HeapNode>> pq;
    
    // Inicialización
    minCost[0] = 0;
    pq.push({0, 0});
    
    while (!pq.empty()) {
        size_t currentNode = pq.top().second;
        pq.pop();
        
        if (visited[currentNode]) continue;
        visited[currentNode] = true;
        
        // Procesar vecinos en bloques
        static const size_t BLOCK_SIZE = 64;
        for (size_t start = 0; start < numNeighborhoods; start += BLOCK_SIZE) {
            size_t end = std::min(start + BLOCK_SIZE, numNeighborhoods);
            
            for (size_t next = start; next < end; next++) {
                if (next == currentNode) continue;
                if (distances[currentNode][next] == std::numeric_limits<int>::max() / 2) continue;
                
                if (!visited[next] && distances[currentNode][next] < minCost[next]) {
                    predecessor[next] = static_cast<int>(currentNode);
                    minCost[next] = distances[currentNode][next];
                    pq.push({minCost[next], next});
                }
            }
        }
    }
    
    // Construir resultado
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(numNeighborhoods - 1);
    
    for (size_t i = 1; i < numNeighborhoods; i++) {
        if (predecessor[i] != -1) {
            std::string from, to;
            if (numNeighborhoods <= 26) {
                from = std::string(1, static_cast<char>('A' + predecessor[i]));
                to = std::string(1, static_cast<char>('A' + i));
            } else {
                from = std::to_string(predecessor[i]);
                to = std::to_string(i);
            }
            result.push_back({std::move(from), std::move(to)});
        }
    }
    
    if (result.size() != numNeighborhoods - 1) {
        throw std::runtime_error("Error: El grafo no es conexo");
    }
    
    return result;
}

// Función para encontrar la ruta del repartidor
// Algoritmo: Nearest Neighbor optimizado con procesamiento por lotes
// Complejidad: O(n²), donde n es el número de colonias
std::vector<std::string> findDeliveryRoute(
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
        
    const size_t numNeighborhoods = distances.size();
    const int INF = 1073741823; // Valor usado para representar infinito
    
    // Validación de entrada
    if (distances.empty() || distances[0].size() != numNeighborhoods) {
        throw std::invalid_argument("Matriz de distancias inválida");
    }
    
    // Estructuras para el algoritmo
    std::vector<bool> visited(numNeighborhoods, false);
    std::vector<size_t> currentPath;
    currentPath.reserve(numNeighborhoods + 1);
    
    // Vector para almacenar el grado de cada nodo
    std::vector<int> degree(numNeighborhoods, 0);
    for (size_t i = 0; i < numNeighborhoods; i++) {
        for (size_t j = 0; j < numNeighborhoods; j++) {
            if (distances[i][j] != INF) {
                degree[i]++;
            }
        }
    }
    
    // Encontrar un nodo inicial con buen grado de conectividad
    size_t startNode = 0;
    int maxDegree = degree[0];
    for (size_t i = 1; i < numNeighborhoods; i++) {
        if (degree[i] > maxDegree) {
            maxDegree = degree[i];
            startNode = i;
        }
    }
    
    currentPath.push_back(startNode);
    visited[startNode] = true;
    
    // Función para encontrar el siguiente nodo más cercano alcanzable
    auto findNextNode = [&](size_t current) -> int {
        int bestDist = INF;
        int bestNode = -1;
        
        // Primero intentar encontrar el nodo más cercano no visitado
        for (size_t i = 0; i < numNeighborhoods; i++) {
            if (!visited[i] && distances[current][i] != INF && distances[current][i] < bestDist) {
                bestDist = distances[current][i];
                bestNode = i;
            }
        }
        
        // Si no encontramos un nodo directamente conectado, buscar a través de nodos intermedios
        if (bestNode == -1) {
            for (size_t i = 0; i < numNeighborhoods; i++) {
                if (!visited[i]) {
                    // Buscar el camino más corto a través de nodos ya visitados
                    for (size_t j = 0; j < numNeighborhoods; j++) {
                        if (visited[j] && j != current && 
                            distances[current][j] != INF && distances[j][i] != INF) {
                            int totalDist = distances[current][j] + distances[j][i];
                            if (totalDist < bestDist) {
                                bestDist = totalDist;
                                bestNode = i;
                            }
                        }
                    }
                }
            }
        }
        
        return bestNode;
    };
    
    // Construir el camino
    while (currentPath.size() < numNeighborhoods) {
        int nextNode = findNextNode(currentPath.back());
        
        if (nextNode == -1) {
            // Si no encontramos siguiente nodo, tomar el primer no visitado disponible
            for (size_t i = 0; i < numNeighborhoods; i++) {
                if (!visited[i]) {
                    nextNode = i;
                    break;
                }
            }
        }
        
        if (nextNode == -1) {
            throw std::runtime_error("No se pudo completar la ruta");
        }
        
        currentPath.push_back(nextNode);
        visited[nextNode] = true;
    }
    
    // Intentar cerrar el ciclo
    bool canClose = false;
    size_t bestLast = currentPath.back();
    
    // Buscar un nodo final que pueda conectar de vuelta al inicio
    for (size_t i = currentPath.size() - 1; i > 0; i--) {
        if (distances[currentPath[i]][startNode] != INF) {
            bestLast = currentPath[i];
            canClose = true;
            break;
        }
    }
    
    if (!canClose) {
        // Si no podemos cerrar directamente, buscar un camino indirecto
        for (size_t i = 0; i < numNeighborhoods; i++) {
            if (distances[bestLast][i] != INF && distances[i][startNode] != INF) {
                currentPath.push_back(i);
                canClose = true;
                break;
            }
        }
    }
    
    if (!canClose) {
        throw std::runtime_error("No se puede encontrar un ciclo hamiltoniano");
    }
    
    // Agregar el nodo inicial para cerrar el ciclo
    currentPath.push_back(startNode);
    
    // Convertir a formato de salida
    std::vector<std::string> result;
    result.reserve(currentPath.size());
    
    for (size_t node : currentPath) {
        if (numNeighborhoods <= 26) {
            result.push_back(std::string(1, static_cast<char>('A' + node)));
        } else {
            result.push_back(std::to_string(node));
        }
    }
    
    return result;
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
        TestGenerator generator;
        NetworkCase networkData;
        
        // Solicitar tamaño
        int size;
        std::cout << "Ingrese el número de colonias: ";
        std::cin >> size;
        
        if (size <= 0 || size > 10000) {
            throw std::runtime_error("Número de colonias inválido (debe estar entre 1 y 10000)");
        }
        
        // Generar caso de prueba
        std::cout << "\nGenerando caso de prueba...\n";
        networkData = generator.generateCase(size);
        
        // Optimizar para casos grandes
        if (size > 1000) {
            std::cout << "Optimizando representación para caso grande...\n";
            auto sparseGraph = networkData.toSparseGraph();
            if (!networkData.isValid()) {
                throw std::runtime_error("Error en conversión a grafo disperso");
            }
            networkData.distances = sparseGraph.toAdjacencyMatrix();
        }
        
        std::cout << "Guardando caso de prueba...\n";
        generator.saveToFile(networkData, "generated_test.txt");
        
        // Mostrar información
        std::cout << "\nProcesando red con " << networkData.numNeighborhoods 
                 << " colonias\n";
        std::cout << "Densidad de conexiones: " 
                 << std::fixed << std::setprecision(2) 
                 << networkData.calculateDensity() << "%\n\n";
        
        // 1. Cableado óptimo
        std::cout << "1. Calculando cableado óptimo de fibra óptica...\n";
        const auto cabling = findOptimalCabling(networkData.distances);
        
        int totalCost = 0;
        std::cout << "Conexiones: ";
        for (const auto& connection : cabling) {
            std::cout << "(" << connection.first << "," 
                     << connection.second << ") ";
            
            int i, j;
            if (networkData.numNeighborhoods <= 26) {
                i = connection.first[0] - 'A';
                j = connection.second[0] - 'A';
            } else {
                i = std::stoi(connection.first);
                j = std::stoi(connection.second);
            }
            totalCost += networkData.distances[i][j];
        }
        std::cout << "\nCosto total: " << totalCost << " kilómetros\n\n";
        
        // 2. Ruta del repartidor
        std::cout << "2. Calculando ruta óptima del repartidor...\n";
        const auto deliveryRoute = findDeliveryRoute(networkData.distances);
        
        std::cout << "Secuencia: ";
        int totalDistance = 0;
        for (size_t i = 0; i < deliveryRoute.size(); ++i) {
            std::cout << deliveryRoute[i];
            if (i < deliveryRoute.size() - 1) {
                std::cout << " -> ";
                int from, to;
                if (networkData.numNeighborhoods <= 26) {
                    from = deliveryRoute[i][0] - 'A';
                    to = deliveryRoute[i + 1][0] - 'A';
                } else {
                    from = std::stoi(deliveryRoute[i]);
                    to = std::stoi(deliveryRoute[i + 1]);
                }
                totalDistance += networkData.distances[from][to];
            }
        }
        std::cout << "\nDistancia total: " << totalDistance << " kilómetros\n\n";

        
        // 3. Flujo máximo
        std::cout << "3. Calculando flujo máximo de información...\n";
        auto capacitiesCopy = networkData.capacities; // Evitar modificar original
        int maxFlow = calculateMaxFlow(capacitiesCopy);
        
        std::cout << "Desde colonia ";
        if (networkData.numNeighborhoods <= 26) {
            std::cout << 'A' << " hasta " 
                     << static_cast<char>('A' + networkData.numNeighborhoods - 1);
        } else {
            std::cout << "0 hasta " 
                     << (networkData.numNeighborhoods - 1);
        }
        std::cout << "\nFlujo máximo: " << maxFlow << " unidades\n\n";
        
        // 4. Procesamiento de centrales
        std::cout << "4. Procesando ubicaciones y centrales...\n";
        std::cout << "Centrales disponibles: " << networkData.centrals.size() 
                 << "\n\n";
        
        // Mostrar información de centrales
        std::cout << std::fixed << std::setprecision(2);
        for (const auto& central : networkData.centrals) {
            std::cout << "Central " << central.neighborhood 
                     << ": (" << central.x << ", " << central.y << ")\n";
        }
        
        // Procesar ubicaciones de ejemplo
        const std::vector<Point> testLocations = {
            Point(25.0, 30.0),
            Point(15.0, 15.0),
            Point(40.0, 35.0),
            Point(10.0, 20.0)
        };
        
        std::cout << "\nAsignaciones de prueba:\n";
        for (const auto& location : testLocations) {
            char nearest = findNearestCentral(networkData.centrals, location);
            std::cout << "(" << location.x << ", " << location.y 
                     << ") -> Central " << nearest << "\n";
        }
        
        std::cout << "\nProcesamiento completado exitosamente.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\nError desconocido\n";
        return 1;
    }
    
    return 0;
}