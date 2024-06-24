#include <iostream>
#include <vector>
#include "recursiveMergeSort.h"

/*
    * Implementación de Merge Sort 
    * Nota: El programa espera que el archivo de texto tenga el formato correcto.
    * 
    * Autor: Daniel Alfredo Barreras Meraz
    * Matrícula: A01254805
    * Fecha: 21 de septiembre de 2023
*/

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

    std::vector<double> sortedArr = recursiveMergeSort(arr);

    std::cout << "Array ordenado:" << std::endl;
    for (const auto& num : sortedArr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}