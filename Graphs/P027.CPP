#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <algorithm> // For std::min

using namespace std;

// --- Optimal (Kahn's Algorithm) ---
// Brute force is omitted as it's non-trivial and extremely inefficient
string optimal(const vector<string>& dict, int K) {
    map<char, set<char>> adj;
    map<char, int> indegree;
    
    // Initialize indegree for all possible K characters
    for (int i = 0; i < K; ++i) {
        indegree[char('a' + i)] = 0;
    }
    // Also add any other chars that might appear (though problem implies only K)
    for(const string& word : dict) {
        for(char c : word) {
            if(indegree.find(c) == indegree.end()) {
                indegree[c] = 0;
            }
        }
    }

    for (size_t i = 0; i < dict.size() - 1; ++i) {
        string w1 = dict[i];
        string w2 = dict[i + 1];
        int len = min(w1.length(), w2.length());
        bool mismatch_found = false;

        for (int j = 0; j < len; ++j) {
            if (w1[j] != w2[j]) {
                // if w2[j] is not already a neighbor of w1[j]
                if (adj[w1[j]].find(w2[j]) == adj[w1[j]].end()) {
                    adj[w1[j]].insert(w2[j]);
                    indegree[w2[j]]++;
                }
                mismatch_found = true;
                break;
            }
        }
        
        // Check for invalid prefix: "apple" vs "app"
        if (!mismatch_found && w1.length() > w2.length()) {
            return "";
        }
    }

    queue<char> q;
    for (auto const& [ch, degree] : indegree) {
        if (degree == 0) {
            q.push(ch);
        }
    }

    string order = "";
    while (!q.empty()) {
        char ch = q.front();
        q.pop();
        order += ch;
        
        if (adj.count(ch)) {
            for (char nei : adj[ch]) {
                indegree[nei]--;
                if (indegree[nei] == 0) {
                    q.push(nei);
                }
            }
        }
    }

    // Check for cycle
    if (order.length() < indegree.size()) {
        return "";
    }
    return order;
}

int main() {
    int N = 5;
    int K = 4;
    vector<string> dict = {"baa", "abcd", "abca", "cab", "cad"};
    
    // Brute force is omitted
    cout << "Brute: " << "" << endl; // Skipping brute as in Python
    cout << "Optimal: " << optimal(dict, K) << endl;
    
    return 0;
}