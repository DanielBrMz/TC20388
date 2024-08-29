#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <cmath>
#include "../Support/HashTable/HashTable.h"

// Función para convertir pesos a centavos
int pesosToCentavos(double pesos) {
    return static_cast<int>(std::round(pesos * 100));
}

/*
Complejidad Temporal:
- O(N * cambio), donde N es el número de denominaciones y cambio es la cantidad de cambio a dar en centavos.
- Las ejecuciones subsiguientes podrían ser O(1) si el resultado está en caché, pero esto depende del patrón de uso.

Complejidad Espacial:
- O(cambio) para el vector dp.
- O(N) para el vector resultado y la copia local de suministro.
- O(K) para la HashTable, donde K es el número de resultados únicos almacenados.

La optimización principal es usar un solo vector para dp en lugar de una matriz 2D,
lo que reduce significativamente el uso de memoria.
*/
std::vector<int> calcularCambioDinamico(const std::vector<int>& denominacionesCentavos, int cambioCentavos, 
                                        const std::vector<int>& suministroOriginal, HashTable<std::string, std::vector<int>>& cache) {
    std::string key = std::to_string(cambioCentavos);
    for (size_t i = 0; i < denominacionesCentavos.size(); ++i) {
        key += "_" + std::to_string(denominacionesCentavos[i]) + ":" + std::to_string(suministroOriginal[i]);
    }

    std::vector<int> resultado;
    if (cache.get(key, resultado)) {
        return resultado;
    }

    // Crear una copia local de suministro que podemos modificar
    std::vector<int> suministro = suministroOriginal;

    int N = denominacionesCentavos.size();
    std::vector<int> dp(cambioCentavos + 1, std::numeric_limits<int>::max() - 1);
    std::vector<int> moneda(cambioCentavos + 1, -1);
    dp[0] = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = denominacionesCentavos[i]; j <= cambioCentavos; ++j) {
            if (dp[j - denominacionesCentavos[i]] != std::numeric_limits<int>::max() - 1 &&
                dp[j - denominacionesCentavos[i]] + 1 < dp[j]) {
                dp[j] = dp[j - denominacionesCentavos[i]] + 1;
                moneda[j] = i;
            }
        }
    }

    resultado = std::vector<int>(N, 0);
    int restante = cambioCentavos;
    while (restante > 0 && moneda[restante] != -1) {
        int i = moneda[restante];
        if (suministro[i] > 0) {
            resultado[i]++;
            suministro[i]--;
            restante -= denominacionesCentavos[i];
        } else {
            break;
        }
    }

    cache.insert(key, resultado);
    return resultado;
}

// Función principal
int main() {
    int N;
    double P, Q;
    std::cin >> N;
    std::vector<double> denominacionesPesos(N);
    std::vector<int> denominacionesCentavos(N);
    std::vector<int> suministro(N);

    for (int i = 0; i < N; i++) {
        std::cin >> denominacionesPesos[i];
        denominacionesCentavos[i] = pesosToCentavos(denominacionesPesos[i]);
    }

    std::cin >> P >> Q;

    for (int i = 0; i < N; i++) {
        std::cin >> suministro[i];
    }

    int cambioCentavos = pesosToCentavos(Q - P);
    HashTable<std::string, std::vector<int>> cache;

    std::vector<int> resultado = calcularCambioDinamico(denominacionesCentavos, cambioCentavos, suministro, cache);

    for (int i = 0; i < N; i++) {
        std::cout << resultado[i] << std::endl;
    }

    return 0;
}