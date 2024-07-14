#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

/*
 * Implementación de análisis de transmisiones y detección de códigos maliciosos
 * utilizando el algoritmo de Rabin-Karp para la búsqueda de subcadenas.
 * 
 * Autores: 
 * - Daniel Alfredo Barreras Meraz
 * Matrículas:
 * - A01254805
 * Fecha: 26 de septiembre de 2024
 */

// Función para leer el contenido de un archivo
// Algoritmo: Lectura secuencial
// Complejidad: O(n), donde n es el número de caracteres en el archivo
std::string readFile(const std::string& filename) {
    std::ifstream file(filename.c_str());  // Usar c_str() para compatibilidad
    std::string content;
    char ch;
    while (file.get(ch)) { 
        content.push_back(ch);
    }
    file.close();
    return content;
}

// Función para buscar una subcadena dentro de una cadena usando Rabin-Karp
// Algoritmo: Rabin-Karp
// Complejidad: O(n + m) en promedio, O(nm) en el peor caso
// donde n es la longitud de str y m es la longitud de sub
bool containsSubstring(const std::string& str, const std::string& sub, int& position) {
    /*
     * Elegí el algoritmo de Rabin-Karp en lugar de la fuerza bruta por varias 
     * razones. Primero, es eficiente, con una complejidad promedio de O(n + m), 
     * lo cual es ideal para buscar múltiples códigos maliciosos en transmisiones 
     * largas. Además, es adaptable y puede modificarse fácilmente para buscar 
     * varios patrones al mismo tiempo, en este caso fue modificado para solo 
     * considerar caracteres alfanuméricos [A-Z0-9] lo que es útil para este caso 
     * en específico. Implementar Rabin-Karp también me sirvió para demostrarme 
     * que puedo investigar y aprender el algoritmo más adecuado para resolver un 
     * problema de búsqueda de cadenas, lo cual es valioso en un contexto académico/
     * profesional. Por último, lo elegí porque es eficiente para manejar grandes 
     * volúmenes de datos, algo común en la detección de códigos maliciosos en el 
     * mundo real, y maneja bien las colisiones de hash con verificaciones adicionales.
     */

    const int BASE = 16; // Base para el sistema hexadecimal
    const int MOD = 1e9 + 7; // Un número primo grande

    int n = str.length(), m = sub.length();
    if (m > n) return false;

    // Función lambda para convertir un carácter a su valor numérico (me gusta usar lambdas lol)
    auto charToInt = [](char c) {
        return (c >= '0' && c <= '9') ? c - '0' : c - 'A' + 10;
    };

    // Calcular el hash del patrón y el hash inicial de la ventana
    long long patternHash = 0, windowHash = 0;
    long long h = 1;
    for (int i = 0; i < m - 1; i++) {
        h = (h * BASE) % MOD;
    }

    for (int i = 0; i < m; i++) {
        patternHash = (patternHash * BASE + charToInt(sub[i])) % MOD;
        windowHash = (windowHash * BASE + charToInt(str[i])) % MOD;
    }

    // Deslizar la ventana y comparar hashes
    for (int i = 0; i <= n - m; i++) {
        if (patternHash == windowHash) {
            // Verificar carácter por carácter en caso de colisión
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (str[i + j] != sub[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                position = i + 1; // +1 porque las posiciones empiezan en 1
                return true;
            }
        }

        // Calcular el hash para la siguiente ventana
        if (i < n - m) {
            windowHash = (BASE * (windowHash - charToInt(str[i]) * h) + charToInt(str[i + m])) % MOD;
            if (windowHash < 0) windowHash += MOD;
        }
    }

    return false;
}

// Función para encontrar el palíndromo más largo en una cadena
// Algoritmo: Expansión alrededor del centro
// Complejidad: O(n^2), donde n es la longitud de la cadena
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
// Complejidad: O(m*n), donde m y n son las longitudes de str1 y str2 respectivamente
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
            std::cout << (found ? "true " + std::to_string(position) : "false 0") << std::endl;
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