#include <iostream>
#include <vector>
#include "mergeSort.h"

/*
    * Implementación de Merge Sort 
    * Nota: El programa espera que el archivo de texto tenga el formato correcto.
    * 
    * Autores: 
      - Daniel Alfredo Barreras Meraz
      - Yair Salvador Beltrán Ríos
    * Matrículas:
      - A01254805
      - A01254673
    * Fecha: 12 de agosto de 2024
*/

/**
 * Función merge
 * Complejidad Temporal: O(n), donde n es el número total de elementos en ambos vectores de entrada.
 * Esta función siempre compara y fusiona todos los elementos de ambos vectores de entrada.
 * 
 * Análisis:
 * - Inicialización de variables: O(1)
 * - Bucle principal de fusión: O(n)
 * - Bucles de limpieza para elementos restantes: O(k) y O(m) en el peor caso
 * - Operaciones push_back: O(1) 
 * 
 * Complejidad Espacial: O(n), donde n es la suma de los tamaños de leftArr y rightArr
 */
std::vector<double> merge(const std::vector<double>& leftArr, const std::vector<double>& rightArr) {
    std::vector<double> mergedArr;
    size_t leftIndex = 0, rightIndex = 0;

    // O(n), donde n es la suma de los tamaños de leftArr y rightArr
    while (leftIndex < leftArr.size() && rightIndex < rightArr.size()) {
        if (leftArr[leftIndex] < rightArr[rightIndex]) {
            mergedArr.push_back(leftArr[leftIndex++]);
        } else {
            mergedArr.push_back(rightArr[rightIndex++]);
        }
    }

    // O(k), donde k es el número de elementos restantes en leftArr
    while (leftIndex < leftArr.size()) {
        mergedArr.push_back(leftArr[leftIndex++]);
    }

    // O(m), donde m es el número de elementos restantes en rightArr
    while (rightIndex < rightArr.size()) {
        mergedArr.push_back(rightArr[rightIndex++]);
    }

    return mergedArr;
}

/**
 * Función mergeSort
 * Análisis de Complejidad Temporal:
 * T(n) = 2T(n/2) + O(n)
 * Donde:
 *   - 2 es el número de subproblemas en cada paso de división
 *   - n/2 es el tamaño de cada subproblema
 *   - O(n) es el costo de dividir el problema y fusionar los resultados
 * 
 * Aplicando el Teorema Maestro:
 *   a = 2, b = 2, f(n) = O(n)
 *   log_b(a) = log_2(2) = 1
 *   f(n) = O(n^1)
 * 
 * Como f(n) = Θ(n^log_b(a)), estamos en el caso 2 del Teorema Maestro.
 * 
 * Por lo tanto, la complejidad temporal es:
 * Mejor Caso: O(n log n) - cuando el vector ya está ordenado o casi ordenado
 * Caso Promedio: O(n log n)
 * Peor Caso: O(n log n) - cuando el vector está en orden inverso o desordenado aleatoriamente
 * 
 * Complejidad Espacial: O(n) debido al espacio auxiliar utilizado en el paso de fusión
 */
std::vector<double> mergeSort(std::vector<double>& arr) {
    if (arr.size() <= 1) {
        return arr;  
    }

    size_t middle = arr.size() / 2;

    // 1. Dividir el vector en dos subvectores (Divide)
    std::vector<double> leftArr(arr.begin(), arr.begin() + middle);  
    std::vector<double> rightArr(arr.begin() + middle, arr.end());

    // 2. Ordenar recursivamente los subvectores (Conquer)
    leftArr = mergeSort(leftArr);  // T(n/2)
    rightArr = mergeSort(rightArr);  // T(n/2)

    // 3. Combinar los subvectores ordenados (Combine)
    return merge(leftArr, rightArr);
}


// Main function
int main() {
    int N;
    std::cin >> N;

    std::vector<double> arr(N);
    for (int i = 0; i < N; ++i) {
        std::cin >> arr[i];
    }

    std::cout << "Array original:" << std::endl;
    for (const auto& num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::vector<double> sortedArr = mergeSort(arr);

    std::cout << "Array ordenado:" << std::endl;
    for (const auto& num : sortedArr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}