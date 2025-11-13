#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <algorithm> // For std::min, std::remove
#include <map>

using namespace std;

// Helper to print bridges
void printBridges(const string& title, const vector<pair<int, int>>& bridges) {
    cout << title << "[";
    for (size_t i = 0; i < bridges.size(); ++i) {
        cout << "(" << bridges[i].first << ", " << bridges[i].second << ")";
        if (i < bridges.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force (DFS) ---
void dfs_brute(int node, set<int>& visited, const map<int, list<int>>& adj) {
    visited.insert(node);
    if (adj.count(node)) {
        for (int nei : adj.at(node)) {
            if (visited.find(nei) == visited.end()) {
                dfs_brute(nei, visited, adj);
            }
        }
    }
}

// Helper to remove an edge from a list
void remove_edge(list<int>& l, int val) {
    l.remove(val);
}

vector<pair<int, int>> brute(int n, const vector<pair<int, int>>& connections) {
    map<int, list<int>> adj;
    for (const auto& conn : connections) {
        adj[conn.first].push_back(conn.second);
        adj[conn.second].push_back(conn.first);
    }

    vector<pair<int, int>> bridges;
    for (const auto& conn : connections) {
        int u = conn.first;
        int v = conn.second;

        // Remove edge
        remove_edge(adj[u], v);
        remove_edge(adj[v], u);

        set<int> visited;
        dfs_brute(0, visited, adj);
        
        if (visited.size() < n) {
            bridges.push_back({u, v});
        }

        // Add edge back
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    return bridges;
}

// --- Optimal (Tarjan's) ---
vector<int> tin, low;
vector<bool> visited;
int timer;
vector<pair<int, int>> bridges_res;

void dfs_optimal(int node, int parent, const vector<vector<int>>& adj) {
    visited[node] = true;
    tin[node] = low[node] = timer++;
    
    for (int nei : adj[node]) {
        if (nei == parent) {
            continue;
        }
        if (!visited[nei]) {
            dfs_optimal(nei, node, adj);
            low[node] = min(low[node], low[nei]);
            if (low[nei] > tin[node]) {
                bridges_res.push_back({node, nei});
            }
        } else {
            low[node] = min(low[node], tin[nei]);
        }
    }
}

vector<pair<int, int>> optimal(int n, const vector<pair<int, int>>& connections) {
    vector<vector<int>> adj(n);
    for (const auto& conn : connections) {
        adj[conn.first].push_back(conn.second);
        adj[conn.second].push_back(conn.first);
    }

    tin.assign(n, -1);
    low.assign(n, -1);
    visited.assign(n, false);
    timer = 0;
    bridges_res.clear();

    dfs_optimal(0, -1, adj);
    return bridges_res;
}

int main() {
    int n = 4;
    vector<pair<int, int>> connections = {{0, 1}, {1, 2}, {2, 0}, {1, 3}};
    
    printBridges("Brute: ", brute(n, connections));
    printBridges("Optimal: ", optimal(n, connections));
    return 0;
}