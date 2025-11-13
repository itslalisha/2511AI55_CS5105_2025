#include <iostream>
#include <vector>
#include <set>
#include <algorithm> // For std::min
#include <numeric>   // For std::accumulate

using namespace std;

// Helper to print result
void printSet(const string& title, const set<int>& s) {
    cout << title << "[";
    for (auto it = s.begin(); it != s.end(); ++it) {
        cout << *it;
        if (next(it) != s.end()) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force (DFS) ---
void dfs_brute(int u, vector<bool>& visited, int removed, int V, const vector<vector<int>>& adj) {
    visited[u] = true;
    for (int v : adj[u]) {
        if (!visited[v] && v != removed) {
            dfs_brute(v, visited, removed, V, adj);
        }
    }
}

vector<int> brute(int V, const vector<vector<int>>& adj) {
    set<int> result_set;
    for (int removed = 0; removed < V; ++removed) {
        vector<bool> visited(V, false);
        int start = (removed == 0) ? 1 : 0;
        
        dfs_brute(start, visited, removed, V, adj);
        
        int count = 0;
        for(bool v : visited) {
            if(v) count++;
        }
        
        // We also need to exclude the 'removed' node itself from the count
        if (visited[removed]) count--; 

        if (count < V - 1) {
            result_set.insert(removed);
        }
    }
    vector<int> result(result_set.begin(), result_set.end());
    return result.empty() ? vector<int>{-1} : result;
}


// --- Optimal (Tarjan's) ---
vector<int> tin, low;
vector<bool> visited_opt;
int timer_opt;
set<int> res_opt;

void dfs_optimal(int node, int parent, int V, const vector<vector<int>>& adj) {
    visited_opt[node] = true;
    tin[node] = low[node] = timer_opt++;
    int children = 0;

    for (int nei : adj[node]) {
        if (nei == parent) {
            continue;
        }
        if (!visited_opt[nei]) {
            dfs_optimal(nei, node, V, adj);
            low[node] = min(low[node], low[nei]);
            if (parent != -1 && low[nei] >= tin[node]) {
                res_opt.insert(node);
            }
            children++;
        } else {
            low[node] = min(low[node], tin[nei]);
        }
    }
    if (parent == -1 && children > 1) {
        res_opt.insert(node);
    }
}

vector<int> optimal(int V, const vector<vector<int>>& adj) {
    tin.assign(V, -1);
    low.assign(V, -1);
    visited_opt.assign(V, false);
    timer_opt = 0;
    res_opt.clear();

    dfs_optimal(0, -1, V, adj);
    
    return vector<int>(res_opt.begin(), res_opt.end());
}

int main() {
    int V = 5;
    vector<vector<int>> adj(V);
    adj[0] = {1, 2};
    adj[1] = {0, 2};
    adj[2] = {0, 1, 3, 4};
    adj[3] = {2, 4};
    adj[4] = {2, 3};

    vector<int> brute_res = brute(V, adj);
    cout << "Brute: [";
    for(size_t i=0; i<brute_res.size(); ++i) {
        cout << brute_res[i];
        if(i < brute_res.size()-1) cout << ", ";
    }
    cout << "]" << endl;

    vector<int> opt_res = optimal(V, adj);
     cout << "Optimal: [";
    for(size_t i=0; i<opt_res.size(); ++i) {
        cout << opt_res[i];
        if(i < opt_res.size()-1) cout << ", ";
    }
    cout << "]" << endl;

    return 0;
}