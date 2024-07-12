#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string readFile(const string& filename) {
    ifstream file(filename);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

bool containsSubstring(const string& str, const string& sub, int& position) {
    size_t pos = str.find(sub);
    if (pos != string::npos) {
        position = pos + 1;
        return true;
    }
    return false;
}

pair<int, int> findLongestPalindrome(const string& str) {
    int start = 0, maxLength = 1;
    int len = str.length();

    for (int i = 1; i < len; i++) {
        // Even length palindrome
        int low = i - 1, high = i;
        while (low >= 0 && high < len && str[low] == str[high]) {
            if (high - low + 1 > maxLength) {
                start = low;
                maxLength = high - low + 1;
            }
            low--;
            high++;
        }

        // Odd length palindrome
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

pair<int, int> findLongestCommonSubstring(const string& str1, const string& str2) {
    int m = str1.length();
    int n = str2.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
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
    vector<string> transmissions = {"transmission1.txt", "transmission2.txt"};
    vector<string> mcodes = {"mcode1.txt", "mcode2.txt", "mcode3.txt"};

    // Parte 1
    cout << "Parte 1" << endl;
    for (const auto& trans : transmissions) {
        string transContent = readFile(trans);
        for (const auto& mcode : mcodes) {
            string mcodeContent = readFile(mcode);
            int position;
            bool found = containsSubstring(transContent, mcodeContent, position);
            cout << (found ? "true " + to_string(position) : "false 0") << endl;
        }
    }
    cout << endl;

    // Parte 2
    cout << "Parte 2" << endl;
    for (const auto& trans : transmissions) {
        string transContent = readFile(trans);
        auto [start, end] = findLongestPalindrome(transContent);
        cout << start << " " << end << endl;
    }
    cout << endl;

    // Parte 3
    cout << "Parte 3" << endl;
    string trans1 = readFile(transmissions[0]);
    string trans2 = readFile(transmissions[1]);
    auto [start, end] = findLongestCommonSubstring(trans1, trans2);
    cout << start << " " << end << endl;

    return 0;
}