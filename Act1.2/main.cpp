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

std::vector<int> calcularCambioDinamico(const std::vector<int>& denominacionesCentavos, int cambioCentavos, 
                                        std::vector<int>& suministro) {
    int N = denominacionesCentavos.size();
    std::vector<int> dp(cambioCentavos + 1, std::numeric_limits<int>::max());
    std::vector<int> moneda(cambioCentavos + 1, -1);
    dp[0] = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = denominacionesCentavos[i]; j <= cambioCentavos; ++j) {
            if (dp[j - denominacionesCentavos[i]] != std::numeric_limits<int>::max() &&
                dp[j - denominacionesCentavos[i]] + 1 < dp[j] &&
                suministro[i] > 0) {
                dp[j] = dp[j - denominacionesCentavos[i]] + 1;
                moneda[j] = i;
            }
        }
    }

    std::vector<int> resultado(N, 0);
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

    return resultado;
}

int main() {
    int N;
    double P, Q;
    std::cin >> N;
    std::cout << "Número de denominaciones: " << N << std::endl;

    std::vector<double> denominacionesPesos(N);
    std::vector<int> denominacionesCentavos(N);
    std::vector<int> suministro(N);

    for (int i = 0; i < N; i++) {
        std::cin >> denominacionesPesos[i];
        denominacionesCentavos[i] = pesosToCentavos(denominacionesPesos[i]);
        std::cout << "Denominación " << i << ": " << denominacionesPesos[i] << " pesos (" 
                  << denominacionesCentavos[i] << " centavos)" << std::endl;
    }

    std::cin >> P >> Q;
    std::cout << "Precio: " << P << " pesos, Pagado: " << Q << " pesos" << std::endl;

    for (int i = 0; i < N; i++) {
        std::cin >> suministro[i];
        std::cout << "Suministro para denominación " << i << ": " << suministro[i] << std::endl;
    }

    int cambioCentavos = pesosToCentavos(Q - P);
    std::cout << "Cambio a dar: " << cambioCentavos << " centavos" << std::endl;

    std::vector<int> resultado = calcularCambioDinamico(denominacionesCentavos, cambioCentavos, suministro);

    std::cout << "Resultado final (número de monedas/billetes por denominación):" << std::endl;
    for (int i = 0; i < N; i++) {
        if (resultado[i] > 0) {
            std::cout << resultado[i] << " x " << denominacionesPesos[i] << " pesos" << std::endl;
        }
    }

    return 0;
}