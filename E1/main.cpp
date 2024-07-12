#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

/*
 * Implementación de análisis de transmisiones y detección de códigos maliciosos
 * Nota: El programa espera que los archivos de texto tengan el formato correcto.
 * 
 * Autores: 
 * - Daniel Alfredo Barreras Meraz
 * Matrículas:
 * - A01254805
 * Fecha: 26 de septiembre de 2024
 */

// Función para leer el contenido de un archivo
// Complejidad: O(n), donde n es el número de caracteres en el archivo
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Función para buscar una subcadena dentro de una cadena
// Complejidad: O(n*m), donde n es la longitud de str y m es la longitud de sub
bool containsSubstring(const std::string& str, const std::string& sub, int& position) {
    size_t pos = str.find(sub);
    if (pos != std::string::npos) {
        position = pos + 1;
        return true;
    }
    return false;
}

// Función para encontrar el palíndromo más largo en una cadena
// Complejidad: O(n^2), donde n es la longitud de la cadena
// Esta función utiliza el algoritmo de expansión alrededor del centro
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
// Complejidad: O(m*n), donde m y n son las longitudes de str1 y str2 respectivamente
// Esta función utiliza programación dinámica para resolver el problema
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
    std::cout << "Parte 1" << std::endl;
    for (const auto& trans : transmissions) {
        std::string transContent = readFile(trans);
        for (const auto& mcode : mcodes) {
            std::string mcodeContent = readFile(mcode);
            int position;
            bool found = containsSubstring(transContent, mcodeContent, position);
            std::cout << (found ? "true " + std::to_string(position) : "false") << std::endl;
        }
    }
    std::cout << std::endl;

    // Parte 2: Encontrar el palíndromo más largo en cada transmisión
    std::cout << "Parte 2" << std::endl;
    for (const auto& trans : transmissions) {
        std::string transContent = readFile(trans);
        auto [start, end] = findLongestPalindrome(transContent);
        std::cout << start << " " << end << std::endl;
    }
    std::cout << std::endl;

    // Parte 3: Encontrar la subcadena común más larga entre las transmisiones
    std::cout << "Parte 3" << std::endl;
    std::string trans1 = readFile(transmissions[0]);
    std::string trans2 = readFile(transmissions[1]);
    auto [start, end] = findLongestCommonSubstring(trans1, trans2);
    std::cout << start << " " << end << std::endl;

    return 0;
}

/*
 * Análisis de complejidad:
 * 
 * 1. Lectura de archivos (readFile): O(n) para cada archivo, donde n es el número de caracteres.
 * 2. Búsqueda de subcadenas (containsSubstring): O(n*m) para cada búsqueda, donde n es la longitud de la transmisión y m la longitud del código malicioso.
 * 3. Búsqueda de palíndromos (findLongestPalindrome): O(n^2) para cada transmisión, donde n es la longitud de la transmisión.
 * 4. Búsqueda de subcadena común más larga (findLongestCommonSubstring): O(m*n), donde m y n son las longitudes de las dos transmisiones.
 * 
 * Complejidad total:
 * O(T*N + T*M*N + T*N^2 + M*N), donde:
 * T: número de transmisiones
 * N: longitud promedio de las transmisiones
 * M: número de códigos maliciosos
 * 
 * La complejidad está dominada por la búsqueda de palíndromos y la búsqueda de subcadena común más larga.
 */