#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// --- Brute Force (DFS + Recursion Stack) ---
bool dfs_brute(int node, const vector<vector<int>>& adj, 
               vector<bool>& visited, vector<bool>& recStack) {
    visited[node] = true;
    recStack[node] = true;

    for (int nei : adj[node]) {
        if (!visited[nei]) {
            if (dfs_brute(nei, adj, visited, recStack)) {
                return true;
            }
        } else if (recStack[nei]) {
            // Node is visited and is in the current recursion stack
            return true;
        }
    }
    
    recStack[node] = false; // Remove from recursion stack
    return false;
}

bool brute(int V, const vector<pair<int, int>>& edge) {
    vector<vector<int>> adj(V);
    for (const auto& p : edge) {
        adj[p.first].push_back(p.second);
    }
    
    vector<bool> visited(V, false);
    vector<bool> recStack(V, false);

    for (int i = 0; i < V; ++i) {
        if (!visited[i]) {
            if (dfs_brute(i, adj, visited, recStack)) {
                return true;
            }
        }
    }
    return false;
}

// --- Optimal (Kahn's Algorithm) ---
bool optimal(int V, const vector<pair<int, int>>& edge) {
    vector<vector<int>> adj(V);
    vector<int> indegree(V, 0);
    for (const auto& p : edge) {
        adj[p.first].push_back(p.second);
        indegree[p.second]++;
    }

    queue<int> q;
    for (int i = 0; i < V; ++i) {
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
    
    // If count != V, topological sort failed, meaning there's a cycle.
    return count != V;
}

int main() {
    int V = 4;
    vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {2, 3}};

    cout << boolalpha; // Print "true" or "false"
    cout << "Brute: " << brute(V, edges) << endl;
    cout << "Optimal: " << optimal(V, edges) << endl;

    return 0;
}