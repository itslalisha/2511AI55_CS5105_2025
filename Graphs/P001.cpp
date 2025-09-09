#include <bits/stdc++.h>
using namespace std;

vector<int> bfs(int V, vector<vector<int>>& adj) {
    vector<int> bfsOrder;            // stores BFS traversal
    vector<int> vis(V, 0);           // visited array
    queue<int> q;

    // Run BFS for all components
    for(int start = 0; start < V; start++) {
        if(!vis[start]) {
            q.push(start);
            vis[start] = 1;

            while(!q.empty()) {
                int node = q.front();
                q.pop();
                bfsOrder.push_back(node);

                for(auto it : adj[node]) {
                    if(!vis[it]) {
                        vis[it] = 1;
                        q.push(it);
                    }
                }
            }
        }
    }

    return bfsOrder;
}

int main() {
    int V, E;
    cout << "Enter number of vertices and edges: ";
    cin >> V >> E;

    vector<vector<int>> adj(V);

    cout << "Enter " << E << " edges (u v):\n";
    for(int i = 0; i < E; i++) {
        int u, v;
        cin >> u >> v;
        // assuming undirected graph
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    vector<int> result = bfs(V, adj);

    cout << "BFS Traversal: ";
    for(int node : result) cout << node << " ";
    cout << endl;

    return 0;
}
