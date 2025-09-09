#include <bits/stdc++.h>
using namespace std;


class Solution {
private:
    // BFS-based cycle detection for undirected graph using parent tracking
    bool detect(int src, const vector<vector<int>>& graph, vector<int>& vis) {
        vis[src] = 1;
        queue<pair<int,int>> q;
        q.push({src, -1});

        while (!q.empty()) {
            int node = q.front().first;
            int parent = q.front().second;
            q.pop();

            for (int nbr : graph[node]) {
                if (!vis[nbr]) {
                    vis[nbr] = 1;
                    q.push({nbr, node});
                } else if (parent != nbr) {
                    return true;
                }
            }
        }
        return false;
    }

public:
    // signature unchanged
    bool isCycle(int V, vector<vector<int>>& adj) {
        if (V <= 0) return false;

        // We'll build a proper 0-based adjacency list 'graph' of size V,
        // regardless of what form 'adj' has (edge list, 0-based adj list, or 1-based adj list).
        vector<vector<int>> graph(V);

        // Case A: adj.size() == V  -> most likely a 0-based adjacency list already
        if ((int)adj.size() == V) {
            for (int u = 0; u < V; ++u) {
                for (int v : adj[u]) {
                    if (v >= 0 && v < V) graph[u].push_back(v); // copy valid neighbors only
                }
            }
        }
        // Case B: adj.size() == V+1 -> likely a 1-based adjacency list (indices 1..V)
        else if ((int)adj.size() == V + 1) {
            for (int u = 1; u <= V; ++u) {
                for (int v : adj[u]) {
                    int u0 = u - 1, v0 = v - 1;
                    if (v0 >= 0 && v0 < V) {
                        graph[u0].push_back(v0);
                    }
                }
            }
        }
        // Case C: otherwise treat 'adj' as an edge list (E x 2) and infer 0/1-based
        else {
            int min_label = INT_MAX, max_label = INT_MIN;
            for (auto &e : adj) {
                if (e.size() >= 1) { min_label = min(min_label, e[0]); max_label = max(max_label, e[0]); }
                if (e.size() >= 2) { min_label = min(min_label, e[1]); max_label = max(max_label, e[1]); }
            }

            bool oneBased = (min_label >= 1 && max_label <= V); // common heuristic
            for (auto &e : adj) {
                if (e.size() >= 2) {
                    int u = e[0], v = e[1];
                    if (oneBased) { --u; --v; } // convert to 0-based
                    if (u >= 0 && u < V && v >= 0 && v < V) {
                        graph[u].push_back(v);
                        graph[v].push_back(u);
                    }
                }
            }
        }

        // now run the usual BFS cycle detection on 'graph'
        vector<int> vis(V, 0);
        for (int i = 0; i < V; ++i) {
            if (!vis[i]) {
                if (detect(i, graph, vis)) return true;
            }
        }
        return false;
    }
};




int main() {
    int V, E;
    cin >> V >> E;

    // store raw edges in edge-list form
    vector<vector<int>> edges(E, vector<int>(2));
    for (int i = 0; i < E; i++) {
        cin >> edges[i][0] >> edges[i][1];
    }

    Solution obj;
    bool ans = obj.isCycle(V, edges);  // we just pass edges directly

    if (ans) cout << "Graph contains cycle\n";
    else cout << "Graph does not contain cycle\n";

    return 0;
}
