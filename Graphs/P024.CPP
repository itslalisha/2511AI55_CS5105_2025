#include <iostream>
#include <vector>
#include <numeric>   // For std::iota
#include <algorithm> // For std::next_permutation
#include <map>
#include <queue>

using namespace std;

// --- Brute Force ---
// Note: Python is_valid_order has (pos[v] > pos[u])
// This is different from the previous file.
bool is_valid_order(const vector<int>& order, const vector<pair<int, int>>& edges, const map<int, int>& pos) {
    for (const auto& edge : edges) {
        // Edge is (u, v) which means u depends on v (v -> u)
        if (pos.at(edge.second) > pos.at(edge.first)) { 
            return false;
        }
    }
    return true;
}

bool Brute(int N, int P, const vector<pair<int, int>>& array) {
    vector<int> vertices(N);
    iota(vertices.begin(), vertices.end(), 0);

    do {
        map<int, int> pos;
        for (int i = 0; i < N; ++i) {
            pos[vertices[i]] = i;
        }
        if (is_valid_order(vertices, array, pos)) {
            return true;
        }
    } while (next_permutation(vertices.begin(), vertices.end()));

    return false;
}

// --- Optimal (Kahn's Algorithm) ---
bool optimal(int N, int P, const vector<pair<int, int>>& array) {
    vector<int> indegree(N, 0);
    vector<vector<int>> adj(N);
    // Python code builds a reverse graph: (u,v) means v -> u
    for (const auto& p : array) {
        adj[p.second].push_back(p.first); // v -> u
        indegree[p.first]++;              // indegree of u
    }

    queue<int> q;
    for (int i = 0; i < N; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    int count = 0;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        count++;
        for (int nei : adj[node]) {
            indegree[nei]--;
            if (indegree[nei] == 0) {
                q.push(nei);
            }
        }
    }
    return count == N;
}

int main() {
    int N = 4;
    int P = 3;
    vector<pair<int, int>> array = {{1, 0}, {2, 1}, {3, 2}};
    
    cout << boolalpha; // To print "true" or "false"
    cout << "Brute: " << Brute(N, P, array) << endl;
    cout << "Optimal: " << optimal(N, P, array) << endl;
    
    return 0;
}