#include <iostream>
#include <vector>
#include <queue>
#include <algorithm> // For std::sort

using namespace std;

// Helper function to print a vector
void printVector(const string& title, const vector<int>& vec) {
    cout << title << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force (DFS Cycle Detection) ---
// Returns true if the node is safe
bool dfs_brute(int node, const vector<vector<int>>& adj, vector<int>& state) {
    // state 0: unvisited, 1: visiting (in path), 2: visited (safe)
    if (state[node] == 1) return false; // Cycle detected
    if (state[node] == 2) return true;  // Already known safe

    state[node] = 1; // Mark as visiting
    for (int nei : adj[node]) {
        if (!dfs_brute(nei, adj, state)) {
            return false; // Neighbor leads to a cycle
        }
    }
    state[node] = 2; // Mark as visited and safe
    return true;
}

vector<int> brute(int V, const vector<vector<int>>& adj) {
    vector<int> res;
    vector<int> state(V, 0); // 0=unvisited, 1=path, 2=visited
    for (int i = 0; i < V; ++i) {
        if (dfs_brute(i, adj, state)) {
            res.push_back(i);
        }
    }
    return res;
}

// --- Optimal (Kahn's on Reversed Graph) ---
vector<int> optimal(int V, const vector<vector<int>>& adj) {
    vector<vector<int>> revgraph(V);
    vector<int> outdegree(V, 0);
    for (int u = 0; u < V; ++u) {
        for (int v : adj[u]) {
            revgraph[v].push_back(u);
            outdegree[u]++;
        }
    }

    queue<int> q;
    for (int i = 0; i < V; ++i) {
        if (outdegree[i] == 0) {
            q.push(i); // Start with terminal nodes
        }
    }

    vector<bool> is_safe(V, false);
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        is_safe[node] = true;

        for (int nei : revgraph[node]) {
            outdegree[nei]--;
            if (outdegree[nei] == 0) {
                q.push(nei);
            }
        }
    }

    vector<int> res;
    for (int i = 0; i < V; ++i) {
        if (is_safe[i]) {
            res.push_back(i);
        }
    }
    // The Python 'optimal' doesn't sort, but the 'brute' one does implicitly.
    // Adding sort to match Python 'brute' output order.
    sort(res.begin(), res.end()); 
    return res;
}

int main() {
    int V = 7;
    vector<vector<int>> adj = {{1, 2}, {2, 3}, {5}, {0}, {5}, {6}, {}};
    
    printVector("Brute: ", brute(V, adj));
    printVector("Optimal: ", optimal(V, adj));
    
    return 0;
}