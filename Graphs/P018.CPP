#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm> // For std::min
#include <climits>   // For INT_MAX

using namespace std;

const int INF = INT_MAX;

// Helper for brute force
bool diff_by_oneword(const string& w1, const string& w2) {
    int diff = 0;
    for (size_t i = 0; i < w1.length(); ++i) {
        if (w1[i] != w2[i]) {
            diff++;
        }
        if (diff > 1) {
            return false;
        }
    }
    return diff == 1;
}

// --- Brute Force (DFS) ---
void dfs_brute(const string& word, int step, const string& targetWord, 
               const vector<string>& wordList, set<string>& visited, int& minStep) {
    if (word == targetWord) {
        minStep = min(minStep, step);
        return;
    }
    
    // Pruning: if we're already longer than the current min, stop.
    if (step >= minStep) {
        return;
    }

    for (const string& next : wordList) {
        if (diff_by_oneword(next, word) && visited.find(next) == visited.end()) {
            visited.insert(next);
            dfs_brute(next, step + 1, targetWord, wordList, visited, minStep);
            visited.erase(next); // Backtrack
        }
    }
}

int brute(const string& startWord, const string& targetWord, const vector<string>& wordList) {
    int minStep = INF;
    set<string> visited;
    visited.insert(startWord);
    dfs_brute(startWord, 1, targetWord, wordList, visited, minStep);
    return (minStep != INF) ? minStep : 0;
}

// --- Optimal (BFS) ---
int optimal(const string& startWord, const string& targetWord, const vector<string>& wordList) {
    unordered_set<string> wordSet(wordList.begin(), wordList.end());
    if (wordSet.find(targetWord) == wordSet.end()) {
        return 0;
    }

    queue<pair<string, int>> q; // {word, step}
    q.push({startWord, 1});
    
    if(wordSet.count(startWord)) {
        wordSet.erase(startWord);
    }

    while (!q.empty()) {
        string word = q.front().first;
        int step = q.front().second;
        q.pop();

        if (word == targetWord) {
            return step;
        }

        for (size_t i = 0; i < word.length(); ++i) {
            string originalWord = word;
            for (char ch = 'a'; ch <= 'z'; ++ch) {
                word[i] = ch;
                if (wordSet.count(word)) {
                    q.push({word, step + 1});
                    wordSet.erase(word); // Mark as visited
                }
            }
            word = originalWord; // Restore original word
        }
    }
    return 0; // Target not reachable
}

int main() {
    string startWord = "hit";
    string targetWord = "cog";
    vector<string> wordList = {"hot", "dot", "dog", "lot", "log", "cog"};
    
    cout << "Brute: " << brute(startWord, targetWord, wordList) << endl;
    cout << "Optimal: " << optimal(startWord, targetWord, wordList) << endl;

    return 0;
}