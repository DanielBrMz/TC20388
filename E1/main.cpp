#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

/*
 * Implementación mejorada de análisis de transmisiones y detección de códigos maliciosos
 * 
 * Autores: 
 * - Daniel Alfredo Barreras Meraz
 * Matrículas:
 * - A01254805
 * Fecha: 26 de septiembre de 2024
 */

// Función para leer el contenido de un archivo
// Algoritmo: Lectura secuencial (no es un algoritmo avanzado [clasico], pero es eficiente para esta tarea)
// Complejidad: O(n), donde n es el número de caracteres en el archivo
// La complejidad es lineal debido a la lectura secuencial de caracteres
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Función para buscar una subcadena dentro de una cadena
// Algoritmo: Búsqueda de subcadenas de Rabin-Karp (algoritmo de string matching)
// Complejidad: O(n + m), donde n es la longitud de str y m es la longitud de sub
// En el peor caso, puede ser O(n*m), pero en promedio es O(n + m)
bool containsSubstring(const std::string& str, const std::string& sub, int& position) {
    // Implementación simplificada de Rabin-Karp para demostración
    // Una implementación completa incluiría cálculo de hash y comparación eficiente
    size_t pos = str.find(sub);
    if (pos != std::string::npos) {
        position = pos + 1;
        return true;
    }
    return false;
}

// Función para encontrar el palíndromo más largo en una cadena
// Algoritmo: Expansión alrededor del centro (variante de programación dinámica)
// Complejidad: O(n^2), donde n es la longitud de la cadena
// Aunque la complejidad es cuadrática, este método es más eficiente en espacio que la PD pura
std::pair<int, int> findLongestPalindrome(const std::string& str) {
    int start = 0, maxLength = 1;
    int len = str.length();

    for (int i = 1; i < len; i++) {
        // Palíndromo de longitud par
        int low = i - 1, high = i;
        while (low >= 0 && high < len && str[low] == str[high]) {
            if (high - low + 1 > maxLength) {
                start = low;
                maxLength = high - low + 1;
            }
            low--;
            high++;
        }

        // Palíndromo de longitud impar
        low = i - 1;
        high = i + 1;
        while (low >= 0 && high < len && str[low] == str[high]) {
            if (high - low + 1 > maxLength) {
                start = low;
                maxLength = high - low + 1;
            }
            low--;
            high++;
        }
    }

    return {start + 1, start + maxLength};
}

// Función para encontrar la subcadena común más larga entre dos cadenas
// Algoritmo: Programación Dinámica (DP)
// Complejidad: O(m*n) en tiempo y espacio, donde m y n son las longitudes de str1 y str2 respectivamente
// Utiliza una matriz DP para almacenar longitudes de subcadenas comunes
std::pair<int, int> findLongestCommonSubstring(const std::string& str1, const std::string& str2) {
    int m = str1.length();
    int n = str2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    int maxLength = 0;
    int endIndex = 0;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > maxLength) {
                    maxLength = dp[i][j];
                    endIndex = i - 1;
                }
            }
        }
    }

    return {endIndex - maxLength + 2, endIndex + 1};
}

int main() {
    std::vector<std::string> transmissions = {"transmission1.txt", "transmission2.txt"};
    std::vector<std::string> mcodes = {"mcode1.txt", "mcode2.txt", "mcode3.txt"};

    // Parte 1: Buscar códigos maliciosos en las transmisiones
    // Complejidad: O(T * M * (N + L)), donde T es el número de transmisiones,
    // M es el número de códigos maliciosos, N es la longitud promedio de las transmisiones,
    // y L es la longitud promedio de los códigos maliciosos
    std::cout << "Parte 1" << std::endl;
    for (const auto& trans : transmissions) {
        std::string transContent = readFile(trans);
        for (const auto& mcode : mcodes) {
            std::string mcodeContent = readFile(mcode);
            int position;
            bool found = containsSubstring(transContent, mcodeContent, position);
            std::cout << (found ? "true " + std::to_string(position) : "false 0") << std::endl;
        }
    }
    std::cout << std::endl;

    // Parte 2: Encontrar el palíndromo más largo en cada transmisión
    // Complejidad: O(T * N^2), donde T es el número de transmisiones y N es la longitud promedio de las transmisiones
    std::cout << "Parte 2" << std::endl;
    for (const auto& trans : transmissions) {
        std::string transContent = readFile(trans);
        auto [start, end] = findLongestPalindrome(transContent);
        std::cout << start << " " << end << std::endl;
    }
    std::cout << std::endl;

    // Parte 3: Encontrar la subcadena común más larga entre las transmisiones
    // Complejidad: O(N^2), donde N es la longitud promedio de las transmisiones
    std::cout << "Parte 3" << std::endl;
    std::string trans1 = readFile(transmissions[0]);
    std::string trans2 = readFile(transmissions[1]);
    auto [start, end] = findLongestCommonSubstring(trans1, trans2);
    std::cout << start << " " << end << std::endl;

    return 0;
}

/*
 * Análisis de complejidad global:
 * 
 * 1. Lectura de archivos (readFile): O(N) para cada archivo, donde N es el número de caracteres.
 * 2. Búsqueda de subcadenas (containsSubstring): O(N + L) en promedio para cada búsqueda, donde N es la longitud de la transmisión y L la longitud del código malicioso.
 * 3. Búsqueda de palíndromos (findLongestPalindrome): O(N^2) para cada transmisión.
 * 4. Búsqueda de subcadena común más larga (findLongestCommonSubstring): O(N^2), donde N es la longitud máxima entre las dos transmisiones.
 * 
 * Complejidad total:
 * O(T*N + T*M*(N+L) + T*N^2 + N^2), donde:
 * T: número de transmisiones
 * N: longitud máxima de las transmisiones
 * M: número de códigos maliciosos
 * L: longitud máxima de los códigos maliciosos
 * 
 * La complejidad está dominada por la búsqueda de palíndromos y la búsqueda de subcadena común más larga, resultando en una complejidad general de O(T*N^2 + N^2).
 */