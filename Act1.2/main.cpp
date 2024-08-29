#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <cmath>
#include <chrono>
#include <functional>
#include "../Support/HashTable/HashTable.h"

// Función para convertir pesos a centavos
int pesosToCents(double pesos) {
    return static_cast<int>(std::round(pesos * 100));
}

// Función para medir el tiempo de ejecución
template<typename Func, typename... Args>
double measureExecutionTime(Func func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    return duration.count();
}

std::vector<int> calculateDynamicChange(const std::vector<int>& denominationsCents, int changeCents, 
                                        std::vector<int>& supply, HashTable<std::string, std::vector<int>>& cache) {
    // Clave para el caché
    std::string key = std::to_string(changeCents);
    for (size_t i = 0; i < denominationsCents.size(); ++i) {
        key += "_" + std::to_string(denominationsCents[i]) + ":" + std::to_string(supply[i]);
    }

    // Verificar si el resultado está en caché
    std::vector<int> result;
    if (cache.get(key, result)) {
        return result;
    }

    int N = denominationsCents.size();
    result = std::vector<int>(N, 0);
    int remaining = changeCents;

    // Solución lineal usando un enfoque voraz
    for (int i = 0; i < N && remaining > 0; ++i) {
        int count = std::min(remaining / denominationsCents[i], supply[i]);
        result[i] = count;
        remaining -= count * denominationsCents[i];
    }

    // Si no se pudo dar el cambio completo, optimizamos con programación dinámica
    if (remaining > 0) {
        std::vector<int> dp(changeCents + 1, std::numeric_limits<int>::max());
        std::vector<int> coin(changeCents + 1, -1);
        dp[0] = 0;

        for (int i = 0; i < N; ++i) {
            for (int j = denominationsCents[i]; j <= changeCents; ++j) {
                if (dp[j - denominationsCents[i]] != std::numeric_limits<int>::max() &&
                    dp[j - denominationsCents[i]] + 1 < dp[j] &&
                    supply[i] > result[i]) {
                    dp[j] = dp[j - denominationsCents[i]] + 1;
                    coin[j] = i;
                }
            }
        }

        result = std::vector<int>(N, 0);
        remaining = changeCents;
        while (remaining > 0 && coin[remaining] != -1) {
            int i = coin[remaining];
            if (supply[i] > 0) {
                result[i]++;
                supply[i]--;
                remaining -= denominationsCents[i];
            } else {
                break;
            }
        }
    }

    // Almacenar el resultado en caché
    cache.insert(key, result);
    return result;
}

int main() {
    int N;
    double P, Q;
    std::cin >> N;

    std::vector<double> denominationsPesos(N);
    std::vector<int> denominationsCents(N);
    std::vector<int> supply(N);

    for (int i = 0; i < N; i++) {
        std::cin >> denominationsPesos[i];
        denominationsCents[i] = pesosToCents(denominationsPesos[i]);
    }

    std::cin >> P >> Q;

    for (int i = 0; i < N; i++) {
        std::cin >> supply[i];
    }

    int changeCents = pesosToCents(Q - P);

    HashTable<std::string, std::vector<int>> cache;

    std::vector<double> executionTimes;
    std::vector<int> inputSizes;

    // Ejecutamos el algoritmo varias veces para medir su comportamiento
    for (int i = 0; i < 5; ++i) {
        double executionTime = measureExecutionTime([&]() {
            std::vector<int> result = calculateDynamicChange(denominationsCents, changeCents, supply, cache);

            if (i == 0) {  // Solo imprimimos el resultado la primera vez
                for (int j = 0; j < N; j++) {
                    if (result[j] > 0) {
                        std::cout << result[j] << " x " << denominationsPesos[j] << " pesos" << std::endl;
                    }
                }
            }
        });

        executionTimes.push_back(executionTime);
        inputSizes.push_back(changeCents * (i + 1));  // Simulamos un aumento en el tamaño de entrada

        std::cout << "Ejecucion " << i+1 << " - Tiempo: " << executionTime << " ms" << std::endl;
    }

    return 0;
}