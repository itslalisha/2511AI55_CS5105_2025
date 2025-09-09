#include <bits/stdc++.h>
using namespace std;

class Solution {
    void dfsHelper(int node, vector<vector<int>> &adj, vector<int>& vis, vector<int>& dfsorder) {
        vis[node] = 1;
        dfsorder.push_back(node);
        for (auto it : adj[node]) {
            if (!vis[it]) {
                dfsHelper(it, adj, vis, dfsorder);
            }
        }
    }

public:
    vector<int> dfs(vector<vector<int>>& adj) {
        int V = adj.size();
        vector<int> vis(V, 0);
        vector<int> dfsorder;
        for (int i = 0; i < V; i++) {
            if (!vis[i]) {
                dfsHelper(i, adj, vis, dfsorder);
            }
        }
        return dfsorder;
    }
};

int main() {
    int V, E;
    cin >> V >> E;  // number of vertices and edges

    vector<vector<int>> adj(V);
    for (int i = 0; i < E; i++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u); // comment this if graph is directed
    }

    Solution obj;
    vector<int> result = obj.dfs(adj);

    cout << "DFS order: ";
    for (int node : result) {
        cout << node << " ";
    }
    cout << endl;

    return 0;
}
