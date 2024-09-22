/*
 * Implementación de sistema de optimización de red de fibra óptica
 * Resuelve problemas de cableado óptimo, rutas de servicio y flujo máximo
 * para una empresa de servicios de Internet
 * 
 * Autor: [Tu Nombre]
 * Matrícula: [Tu Matrícula]
 * Fecha: [Fecha Actual]
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>

using namespace std;

// Estructura para representar una central
struct Central {
    char colonia;
    double x, y;
};

// Estructura para representar un punto nuevo
struct Punto {
    double x, y;
};

// Función para leer la matriz de entrada
// Algoritmo: Lectura secuencial de matriz
// Complejidad: O(n²), donde n es el número de colonias
vector<vector<int>> leerMatriz(ifstream& file, int n) {
    vector<vector<int>> matriz(n, vector<int>(n));
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            file >> matriz[i][j];
        }
    }
    return matriz;
}

// Función para encontrar el árbol de expansión mínima usando Prim
// Algoritmo: Prim con cola de prioridad
// Complejidad: O(E log V), donde E es número de aristas y V número de vértices
/*
 * Elegí el algoritmo de Prim por las siguientes razones:
 * 1. Es eficiente para grafos densos, como es este caso con matriz de adyacencia
 * 2. Garantiza la conectividad mínima entre todas las colonias
 * 3. Minimiza el costo total del cableado
 * 4. Es más simple de implementar con matriz de adyacencia que Kruskal
 */
vector<pair<char,char>> primMST(const vector<vector<int>>& grafo) {
    int n = grafo.size();
    vector<bool> visitado(n, false);
    vector<int> padre(n, -1);
    vector<int> clave(n, numeric_limits<int>::max());
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    
    clave[0] = 0;
    pq.push({0, 0});
    
    while(!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        
        visitado[u] = true;
        
        for(int v = 0; v < n; v++) {
            if(grafo[u][v] && !visitado[v] && grafo[u][v] < clave[v]) {
                padre[v] = u;
                clave[v] = grafo[u][v];
                pq.push({clave[v], v});
            }
        }
    }
    
    vector<pair<char,char>> resultado;
    for(int i = 1; i < n; i++) {
        resultado.push_back({static_cast<char>('A' + padre[i]), 
                           static_cast<char>('A' + i)});
    }
    return resultado;
}

// Función para resolver el problema del viajante (TSP)
// Algoritmo: Nearest Neighbor con vuelta al inicio
// Complejidad: O(n²), donde n es el número de colonias
/*
 * Elegí el algoritmo Nearest Neighbor para TSP por:
 * 1. Proporciona una solución aproximada en tiempo razonable
 * 2. Es fácil de implementar y entender
 * 3. Aunque no garantiza la solución óptima, da buenos resultados prácticos
 * 4. Es apropiado para el tamaño de problema planteado
 */
vector<char> tspNearestNeighbor(const vector<vector<int>>& grafo) {
    int n = grafo.size();
    vector<bool> visitado(n, false);
    vector<char> ruta;
    int actual = 0;
    
    ruta.push_back('A');
    visitado[0] = true;
    
    while(ruta.size() < n) {
        int siguiente = -1;
        int minDist = numeric_limits<int>::max();
        
        for(int i = 0; i < n; i++) {
            if(!visitado[i] && grafo[actual][i] < minDist) {
                minDist = grafo[actual][i];
                siguiente = i;
            }
        }
        
        actual = siguiente;
        visitado[siguiente] = true;
        ruta.push_back(static_cast<char>('A' + siguiente));
    }
    
    ruta.push_back('A');
    return ruta;
}

// Función para calcular el flujo máximo usando Ford-Fulkerson con BFS
// Algoritmo: Ford-Fulkerson con BFS (Edmonds-Karp)
// Complejidad: O(VE²), donde V es número de vértices y E número de aristas
/*
 * Elegí el algoritmo de Ford-Fulkerson con BFS por:
 * 1. Garantiza encontrar el flujo máximo exacto
 * 2. Es eficiente para redes con capacidades enteras
 * 3. La implementación con BFS (Edmonds-Karp) mejora la complejidad
 * 4. Es el estándar para problemas de flujo máximo
 */
int flujoMaximo(vector<vector<int>>& capacidades, int origen, int destino) {
    int n = capacidades.size();
    vector<vector<int>> flujo(n, vector<int>(n, 0));
    int maxFlujo = 0;
    
    while(true) {
        vector<int> padre(n, -1);
        queue<int> q;
        q.push(origen);
        padre[origen] = origen;
        
        while(!q.empty() && padre[destino] == -1) {
            int u = q.front();
            q.pop();
            
            for(int v = 0; v < n; v++) {
                if(padre[v] == -1 && capacidades[u][v] > flujo[u][v]) {
                    padre[v] = u;
                    q.push(v);
                }
            }
        }
        
        if(padre[destino] == -1) break;
        
        int incremento = numeric_limits<int>::max();
        for(int v = destino; v != origen; v = padre[v]) {
            int u = padre[v];
            incremento = min(incremento, capacidades[u][v] - flujo[u][v]);
        }
        
        for(int v = destino; v != origen; v = padre[v]) {
            int u = padre[v];
            flujo[u][v] += incremento;
            flujo[v][u] -= incremento;
        }
        
        maxFlujo += incremento;
    }
    
    return maxFlujo;
}

// Función para encontrar la central más cercana
// Algoritmo: Cálculo de distancia euclidiana
// Complejidad: O(n), donde n es el número de centrales
char centralMasCercana(const vector<Central>& centrales, const Punto& nuevo) {
    double minDist = numeric_limits<double>::max();
    char centralCercana = ' ';
    
    for(const auto& central : centrales) {
        double dist = sqrt(pow(central.x - nuevo.x, 2) + 
                         pow(central.y - nuevo.y, 2));
        if(dist < minDist) {
            minDist = dist;
            centralCercana = central.colonia;
        }
    }
    
    return centralCercana;
}

int main() {
    ifstream file("input.txt");
    int n;
    file >> n;
    
    // Leer matriz de distancias
    vector<vector<int>> distancias = leerMatriz(file, n);
    
    // Leer matriz de capacidades
    vector<vector<int>> capacidades = leerMatriz(file, n);
    
    // 1. Encontrar forma óptima de cablear
    cout << "1. Cableado óptimo:" << endl;
    vector<pair<char,char>> cableado = primMST(distancias);
    for(const auto& arco : cableado) {
        cout << "(" << arco.first << "," << arco.second << ") ";
    }
    cout << endl << endl;
    
    // 2. Encontrar ruta para repartidor
    cout << "2. Ruta para repartidor:" << endl;
    vector<char> ruta = tspNearestNeighbor(distancias);
    for(char colonia : ruta) {
        cout << colonia << " ";
    }
    cout << endl << endl;
    
    // 3. Calcular flujo máximo
    cout << "3. Flujo máximo de información:" << endl;
    int flujoMax = flujoMaximo(capacidades, 0, n-1);
    cout << flujoMax << endl << endl;
    
    // 4. Leer centrales y procesar consultas de cercanía
    int numCentrales;
    file >> numCentrales;
    vector<Central> centrales;
    for(int i = 0; i < numCentrales; i++) {
        Central central;
        file >> central.colonia >> central.x >> central.y;
        centrales.push_back(central);
    }
    
    // Ejemplo de uso para encontrar central más cercana
    Punto nuevoPunto = {25, 30};
    cout << "4. Central más cercana para punto (" 
         << nuevoPunto.x << "," << nuevoPunto.y << "): "
         << centralMasCercana(centrales, nuevoPunto) << endl;
    
    return 0;
}