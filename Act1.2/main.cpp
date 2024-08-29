#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <functional>
#include <cmath>
#include "../Support/HashTable/HashTable.h"

template<typename Func, typename... Args>
double measureExecutionTime(Func func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    return duration.count();
}

std::string createCacheKey(double change, const std::vector<double>& denominations, const std::vector<int>& supply) {
    std::string key = std::to_string(change);
    for (size_t i = 0; i < denominations.size(); ++i) {
        key += "_" + std::to_string(denominations[i]) + ":" + std::to_string(supply[i]);
    }
    return key;
}

std::vector<int> calculateChange(const std::vector<double>& denominations, double change, 
                                 std::vector<int>& supply, HashTable<std::string, std::vector<int>>& cache) {
    // Round change to 2 decimal places to avoid floating-point precision issues
    change = std::round(change * 100.0) / 100.0;
    
    // Create cache key
    std::string key = createCacheKey(change, denominations, supply);

    // Check cache
    std::vector<int> result;
    if (cache.get(key, result)) {
        return result;
    }

    int N = denominations.size();
    result = std::vector<int>(N, 0);

    // Base case: if change is 0, return empty result
    if (change < 1e-9) {
        cache.insert(key, result);
        return result;
    }

    // Try to use each denomination
    for (int i = 0; i < N; ++i) {
        if (denominations[i] <= change && supply[i] > 0) {
            // Use this denomination
            supply[i]--;
            std::vector<int> subResult = calculateChange(denominations, change - denominations[i], supply, cache);
            supply[i]++; // Restore supply for backtracking

            // If a valid solution was found for the remaining change
            if (subResult[N-1] != -1) {
                result = subResult;
                result[i]++;
                
                // Cache this sub-solution
                cache.insert(key, result);
                return result;
            }
        }
    }

    // If no valid solution was found, cache this fact
    result[N-1] = -1; // Use last element as a flag for invalid solution
    cache.insert(key, result);
    return result;
}

int main() {
    int N;
    double P, Q;
    std::cin >> N;

    std::vector<double> denominations(N);
    std::vector<int> supply(N);

    for (int i = 0; i < N; i++) {
        std::cin >> denominations[i];
    }

    std::cin >> P >> Q;

    for (int i = 0; i < N; i++) {
        std::cin >> supply[i];
    }

    double change = Q - P;

    HashTable<std::string, std::vector<int>> cache;

    std::vector<double> executionTimes;
    std::vector<double> inputSizes;

    // Run the algorithm multiple times to measure its behavior
    for (int i = 0; i < 5; ++i) {
        double executionTime = measureExecutionTime([&]() {
            std::vector<int> result = calculateChange(denominations, change, supply, cache);

            if (i == 0) {  // Only print the result the first time
                if (result[N-1] == -1) {
                    std::cout << "No solution found" << std::endl;
                } else {
                    for (int j = 0; j < N; j++) {
                        if (result[j] > 0) {
                            std::cout << result[j] << " x " << denominations[j] << " pesos" << std::endl;
                        }
                    }
                }
            }
        });

        executionTimes.push_back(executionTime);
        inputSizes.push_back(change * (i + 1));  // Simulate an increase in input size

        std::cout << "Execution " << i+1 << " - Time: " << executionTime << " ms" << std::endl;
    }

    return 0;
}