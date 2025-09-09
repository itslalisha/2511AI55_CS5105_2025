#include <bits/stdc++.h>
using namespace std;

class Solution {
private:
    bool dfs(int node, int color, vector<int>& colors, vector<vector<int>>& adj) {
        colors[node] = color; // Assign color

        for (int neighbor : adj[node]) {
            if (colors[neighbor] == -1) {
                if (!dfs(neighbor, 1 - color, colors, adj)) 
                    return false;
            } else if (colors[neighbor] == color) {
                return false; // Same color as neighbor
            }
        }
        return true;
    }

public:
    bool isBipartite(int n, vector<vector<int>>& adj) {
        vector<int> colors(n, -1); // -1 = uncolored

        for (int i = 0; i < n; i++) {
            if (colors[i] == -1) {
                if (!dfs(i, 0, colors, adj)) 
                    return false;
            }
        }
        return true;
    }
};


int main() {
    int n, m;
    cin >> n >> m;

    vector<vector<int>> adj(n);

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u); // undirected graph
    }

    Solution sol;
    if (sol.isBipartite(n, adj))
        cout << "1\n"; 
    else
        cout << "0\n"; 

    return 0;
}
