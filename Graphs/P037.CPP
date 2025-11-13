#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm> // For std::sort
#include <numeric>   // For std::iota

using namespace std;

// Type aliases
using Edge = tuple<int, int, int>; // (u, v, w)
using pii = pair<int, int>; // (u, v) for MST

class DSU {
    vector<int> parent;
    vector<int> rank;
public:
    DSU(int V) {
        parent.resize(V);
        iota(parent.begin(), parent.end(), 0); // Fills parent with 0, 1, 2, ...
        rank.assign(V, 1);
    }

    int find(int u) {
        if (parent[u] != u) {
            parent[u] = find(parent[u]); // Path compression
        }
        return parent[u];
    }

    bool unite(int u, int v) {
        int pu = find(u);
        int pv = find(v);
        if (pu == pv) {
            return false; // Already in the same set
        }
        if (rank[pu] > rank[pv]) {
            parent[pv] = pu;
        } else if (rank[pv] > rank[pu]) {
            parent[pu] = pv;
        } else {
            parent[pv] = pu;
            rank[pu]++;
        }
        return true;
    }
};

void kruskal(int V, vector<Edge>& edges) {
    // Sort edges by weight (the 3rd element, index 2)
    sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return get<2>(a) < get<2>(b);
    });

    int edge_count = 0;
    vector<pii> mst;
    long long wt = 0;
    DSU dsu(V);

    for (const auto& e : edges) {
        int u = get<0>(e);
        int v = get<1>(e);
        int w = get<2>(e);

        if (dsu.unite(u, v)) {
            mst.push_back({u, v});
            wt += w;
            edge_count++;
        }
        if (edge_count == V - 1) {
            break;
        }
    }

    cout << "MST weight: " << wt << endl;
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

    kruskal(V, edges);
    return 0;
}