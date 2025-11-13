#include <iostream>
#include <vector>
#include <queue>
#include <climits>   // For INT_MAX
#include <algorithm> // For std::min

using namespace std;

const int INF = INT_MAX;

// Helper function to print a vector
void printVector(const string& title, const vector<int>& vec) {
    cout << title << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force (DFS) ---
// This is not guaranteed to be shortest, just *a* path distance
void dfs_brute(int node, int target, int dist, vector<bool>& visited, 
               const vector<vector<int>>& adj, vector<int>& distmin) {
    if (node == target) {
        distmin[node] = min(distmin[node], dist);
        return;
    }
    visited[node] = true;
    for (int nei : adj[node]) {
        if (!visited[nei]) {
            dfs_brute(nei, target, dist + 1, visited, adj, distmin);
        }
    }
    visited[node] = false; // Backtrack
}

vector<int> brute(int n, const vector<pair<int, int>>& edge) {
    vector<vector<int>> adj(n);
    for (const auto& p : edge) {
        adj[p.first].push_back(p.second);
        adj[p.second].push_back(p.first);
    }
    
    vector<int> distmin(n, INF);
    for (int i = 0; i < n; ++i) {
        vector<bool> visited(n, false);
        distmin[i] = INF; // Reset dist for each target
        dfs_brute(0, i, 0, visited, adj, distmin);
    }

    for (int i = 0; i < n; ++i) {
        if (distmin[i] == INF) {
            distmin[i] = -1;
        }
    }
    return distmin;
}

// --- Optimal (BFS) ---
vector<int> optimal(int n, const vector<pair<int, int>>& edge) {
    vector<vector<int>> adj(n);
    for (const auto& p : edge) {
        adj[p.first].push_back(p.second);
        adj[p.second].push_back(p.first);
    }
    
    vector<int> distmin(n, INF);
    queue<pair<int, int>> q; // {node, distance}

    q.push({0, 0});
    distmin[0] = 0;

    while (!q.empty()) {
        int node = q.front().first;
        int dist = q.front().second;
        q.pop();

        for (int nei : adj[node]) {
            if (distmin[nei] > dist + 1) {
                distmin[nei] = dist + 1;
                q.push({nei, dist + 1});
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (distmin[i] == INF) {
            distmin[i] = -1;
        }
    }
    return distmin;
}

int main() {
    int V = 6;
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}};

    printVector("Brute : ", brute(V, edges));
    printVector("Optimal :", optimal(V, edges));

    return 0;
}