#include <bits/stdc++.h>
using namespace std;

vector<int> shortestPath(int n, int m, vector<vector<int>>& edges) {
    vector<pair<int,int>> adj[n+1];

    // Build adjacency list
    for (auto it: edges) {
        adj[it[0]].push_back({it[1], it[2]});
        adj[it[1]].push_back({it[0], it[2]});
    }

    // Min-heap: {dist, node}
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    vector<int> dist(n+1, 1e9), parent(n+1);
    for (int i = 1; i <= n; i++) parent[i] = i;

    dist[1] = 0;
    pq.push({0, 1}); // push source

    while (!pq.empty()) {
        auto it = pq.top();
        pq.pop();

        int node = it.second;
        int dis = it.first;

        for (auto it: adj[node]) {
            int adjNode = it.first;
            int edw = it.second;

            if (dis + edw < dist[adjNode]) {
                dist[adjNode] = dis + edw;
                pq.push({dist[adjNode], adjNode});
                parent[adjNode] = node;
            }
        }
    }

    // If no path exists
    if (dist[n] == 1e9) return {-1};

    // Reconstruct path
    vector<int> path;
    int node = n;
    while (parent[node] != node) {
        path.push_back(node);
        node = parent[node];
    }
    path.push_back(1); // include source
    reverse(path.begin(), path.end());

    // Build final result: [weight, path...]
    vector<int> result;
    result.push_back(dist[n]);
    for (int v : path) result.push_back(v);

    return result;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    vector<vector<int>> edges(m, vector<int>(3));
    for (int i = 0; i < m; i++) {
        cin >> edges[i][0] >> edges[i][1] >> edges[i][2];
    }

    vector<int> ans = shortestPath(n, m, edges);

    // Print output
    for (int x : ans) cout << x << " ";
    cout << "\n";

    return 0;
}
