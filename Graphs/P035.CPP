#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

// Type aliases for clarity
using pii = pair<int, int>; // (weight, node) for Dijkstra
using Edge = tuple<int, int, int>; // (u, v, w)

const int INF = INT_MAX / 2;

// "Brute" solution using Dijkstra from each node
int brute(int n, const vector<Edge>& edge, int threshold) {
    vector<vector<pii>> adj(n);
    for (const auto& e : edge) {
        adj[get<0>(e)].push_back({get<1>(e), get<2>(e)});
        adj[get<1>(e)].push_back({get<0>(e), get<2>(e)});
    }

    auto dijkstra = [&](int node) {
        vector<int> dist(n, INF);
        dist[node] = 0;
        priority_queue<pii, vector<pii>, greater<pii>> q;
        q.push({0, node}); // {distance, node}

        while (!q.empty()) {
            int d = q.top().first;
            int u = q.top().second;
            q.pop();

            if (dist[u] < d) {
                continue;
            }

            for (auto& edge : adj[u]) {
                int v = edge.first;
                int w = edge.second;
                // Python code has a bug here: dist[v] < d+w should be dist[v] > d+w
                // I will correct the logic to be a standard Dijkstra
                if (dist[v] > d + w) {
                    dist[v] = d + w;
                    q.push({dist[v], v});
                }
            }
        }
        return dist;
    };

    int min_count = INF;
    int res = -1;
    for (int i = 0; i < n; ++i) {
        vector<int> dist = dijkstra(i);
        int count = 0;
        for (int k : dist) {
            if (k <= threshold) {
                count++;
            }
        }
        // Python code uses <=. If count is equal, it takes the higher index.
        if (count <= min_count) {
            min_count = count;
            res = i;
        }
    }
    return res;
}

// Optimal solution using Floyd-Warshall
int optimal(int n, const vector<Edge>& edge, int threshold) {
    vector<vector<int>> grid(n, vector<int>(n, INF));
    for (const auto& e : edge) {
        grid[get<0>(e)][get<1>(e)] = get<2>(e);
        grid[get<1>(e)][get<0>(e)] = get<2>(e);
    }
    for (int i = 0; i < n; ++i) {
        grid[i][i] = 0;
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] > grid[i][k] + grid[k][j]) {
                    grid[i][j] = grid[i][k] + grid[k][j];
                }
            }
        }
    }

    int min_count = INF;
    int res = -1;
    for (int i = 0; i < n; ++i) {
        int count = 0;
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] <= threshold && i != j) {
                count++;
            }
        }
        // Python code uses <=. If count is equal, it takes the higher index.
        if (count <= min_count) {
            min_count = count;
            res = i;
        }
    }
    return res;
}

int main() {
    int n = 4;
    vector<Edge> edges = {{0, 1, 3}, {1, 2, 1}, {2, 3, 4}, {0, 3, 7}};
    int threshold = 4;

    cout << "Brute (Dijkstra): " << brute(n, edges, threshold) << endl;
    cout << "Optimal (Floyd-Warshall): " << optimal(n, edges, threshold) << endl;

    return 0;
}