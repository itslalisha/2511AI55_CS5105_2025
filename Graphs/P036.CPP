#include <iostream>
#include <vector>
#include <queue>
#include <tuple>

using namespace std;

// Type aliases
using pii = pair<int, int>; // (neighbor, weight)
using Edge = tuple<int, int, int>; // (weight, source, destination)

void prims(int V, const vector<Edge>& edges) {
    vector<vector<pii>> adj(V);
    for (const auto& e : edges) {
        adj[get<0>(e)].push_back({get<1>(e), get<2>(e)});
        adj[get<1>(e)].push_back({get<0>(e), get<2>(e)});
    }

    // Min-heap: (weight, source, destination)
    priority_queue<Edge, vector<Edge>, greater<Edge>> q;
    q.push({0, -1, 0}); // (weight, parent, node). -1 for root's parent.
    
    long long wt = 0;
    vector<bool> visited(V, false);
    vector<pii> mst; // Store (source, destination)

    while (!q.empty()) {
        auto [w, s, e] = q.top();
        q.pop();

        if (visited[e]) {
            continue;
        }
        
        // s != -1 check to avoid adding the initial (-1, 0) edge
        if (s != -1) { 
            mst.push_back({s, e});
        }
        wt += w;
        visited[e] = true;

        for (auto& edge : adj[e]) {
            int nei = edge.first;
            int d = edge.second;
            if (!visited[nei]) {
                q.push({d, e, nei});
            }
        }
    }

    cout << "Weight: " << wt << endl;
    cout << "MST: " << endl;
    for (const auto& p : mst) {
        cout << "  (" << p.first << ", " << p.second << ")" << endl;
    }
}

int main() {
    int V = 5;
    vector<Edge> edges = {
        {0, 1, 2}, {0, 3, 6}, {1, 2, 3}, {1, 3, 8},
        {1, 4, 5}, {2, 4, 7}, {3, 4, 9}
    };
    prims(V, edges);
    return 0;
}