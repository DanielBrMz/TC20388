#include "recursiveMergeSort.h"
#include <vector>

/**
 * Función recursiveMerge
 * Complejidad Temporal: O(n), donde n es el número total de elementos en ambos vectores de entrada.
 * Esta función siempre compara y fusiona todos los elementos de ambos vectores de entrada.
 * 
 * Análisis:
 * - Inicialización de variables: O(1)
 * - Bucle principal de fusión: O(n)
 * - Bucles de limpieza para elementos restantes: O(n) en el peor caso
 * - Operaciones push_back: O(1) amortizado por operación
 * 
 * Complejidad Espacial: O(n), donde n es la suma de los tamaños de leftArr y rightArr
 */
std::vector<double> recursiveMerge(const std::vector<double>& leftArr, const std::vector<double>& rightArr) {
    std::vector<double> res;  // O(1)
    size_t leftIndex = 0, rightIndex = 0;  // O(1)

    // O(n), donde n es la suma de los tamaños de leftArr y rightArr
    while (leftIndex < leftArr.size() && rightIndex < rightArr.size()) {
        if (leftArr[leftIndex] < rightArr[rightIndex]) {
            res.push_back(leftArr[leftIndex++]);  // O(1) amortizado
        } else {
            res.push_back(rightArr[rightIndex++]);  // O(1) amortizado
        }
    }

    // O(k), donde k es el número de elementos restantes en leftArr
    while (leftIndex < leftArr.size()) {
        res.push_back(leftArr[leftIndex++]);  // O(1) amortizado
    }

    // O(m), donde m es el número de elementos restantes en rightArr
    while (rightIndex < rightArr.size()) {
        res.push_back(rightArr[rightIndex++]);  // O(1) amortizado
    }

    return res;  // O(1)
}

/**
 * Función recursiveMergeSort
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

std::vector<double> recursiveMergeSort(std::vector<double>& arr) {
    if (arr.size() <= 1) {
        return arr;  
    }

    size_t middle = arr.size() / 2;

    // 1. Dividir el vector en dos subvectores (Divide)
    std::vector<double> leftArr(arr.begin(), arr.begin() + middle);  // O(n/2)
    std::vector<double> rightArr(arr.begin() + middle, arr.end());  // O(n/2)

    // 2. Ordenar recursivamente los subvectores (Conquer)
    leftArr = recursiveMergeSort(leftArr);  // T(n/2)
    rightArr = recursiveMergeSort(rightArr);  // T(n/2)

    // 3. Combinar los subvectores ordenados (Combine)
    return recursiveMerge(leftArr, rightArr);
}