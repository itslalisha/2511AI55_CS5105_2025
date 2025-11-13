#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm> // For std::min, std::reverse
#include <climits>   // For INT_MAX

using namespace std;

// --- Brute Force (DFS) ---
// Note: This is extremely inefficient and will time out on most non-trivial cases.
bool diff_by_oneword(const string& w1, const string& w2) {
    int diff = 0;
    for (size_t i = 0; i < w1.length(); ++i) {
        if (w1[i] != w2[i]) diff++;
    }
    return diff == 1;
}

void dfs_brute(const string& word, const string& targetWord, const vector<string>& wordList,
               set<string>& visited, vector<string>& path, 
               int& minStep, vector<vector<string>>& minList) {
    
    if (word == targetWord) {
        if (path.size() < minStep) {
            minStep = path.size();
            minList.clear();
            minList.push_back(path);
        } else if (path.size() == minStep) {
            minList.push_back(path);
        }
        return;
    }

    // Pruning
    if (path.size() >= minStep) {
        return;
    }

    for (const string& next : wordList) {
        if (diff_by_oneword(next, word) && visited.find(next) == visited.end()) {
            visited.insert(next);
            path.push_back(next);
            dfs_brute(next, targetWord, wordList, visited, path, minStep, minList);
            path.pop_back();    // Backtrack
            visited.erase(next); // Backtrack
        }
    }
}

vector<vector<string>> brute(const string& startWord, const string& targetWord, const vector<string>& wordList) {
    int minStep = INT_MAX;
    vector<vector<string>> minList;
    set<string> visited;
    vector<string> path;

    visited.insert(startWord);
    path.push_back(startWord);
    dfs_brute(startWord, targetWord, wordList, visited, path, minStep, minList);
    
    return minList;
}

// --- Optimal (BFS + DFS Backtracking) ---
vector<vector<string>> res_opt;
unordered_map<string, vector<string>> parents;

void backtrack_dfs(const string& word, const string& startWord, vector<string>& path) {
    if (word == startWord) {
        res_opt.push_back(path);
        reverse(res_opt.back().begin(), res_opt.back().end());
        return;
    }
    for (const string& p : parents[word]) {
        path.push_back(p);
        backtrack_dfs(p, startWord, path);
        path.pop_back(); // Backtrack
    }
}

vector<vector<string>> optimal(const string& startWord, const string& targetWord, const vector<string>& wordList) {
    unordered_set<string> wordSet(wordList.begin(), wordList.end());
    if (wordSet.find(targetWord) == wordSet.end()) return {};

    res_opt.clear();
    parents.clear();

    queue<string> q;
    q.push(startWord);
    unordered_set<string> visited;
    visited.insert(startWord);
    bool found = false;

    while (!q.empty() && !found) {
        unordered_set<string> next_level_visited;
        int level_size = q.size();
        
        for (int i = 0; i < level_size; ++i) {
            string word = q.front();
            q.pop();

            string originalWord = word;
            for (size_t j = 0; j < word.length(); ++j) {
                for (char ch = 'a'; ch <= 'z'; ++ch) {
                    word[j] = ch;
                    if (wordSet.count(word)) {
                        if (word == targetWord) {
                            found = true;
                        }
                        
                        // If not visited at all, add to next level
                        if (visited.find(word) == visited.end()) {
                            if (next_level_visited.find(word) == next_level_visited.end()) {
                                q.push(word);
                                next_level_visited.insert(word);
                            }
                            parents[word].push_back(originalWord);
                        } 
                        // If visited on this level, just add parent
                        else if (next_level_visited.count(word)) {
                             parents[word].push_back(originalWord);
                        }
                    }
                }
                word = originalWord; // Restore
            }
        }
        visited.insert(next_level_visited.begin(), next_level_visited.end());
    }

    if (found) {
        vector<string> path = {targetWord};
        backtrack_dfs(targetWord, startWord, path);
    }
    return res_opt;
}

// Helper to print results
void printResults(const vector<vector<string>>& results) {
    for (const auto& row : results) {
        for (size_t i = 0; i < row.size(); ++i) {
            cout << row[i];
            if (i < row.size() - 1) cout << "->";
        }
        cout << endl;
    }
}

int main() {
    string startWord = "hit";
    string targetWord = "cog";
    vector<string> wordList = {"hot", "dot", "dog", "lot", "log", "cog"};

    cout << "Brute: \n";
    printResults(brute(startWord, targetWord, wordList));
    
    cout << "\nOptimal: \n";
    printResults(optimal(startWord, targetWord, wordList));

    return 0;
}