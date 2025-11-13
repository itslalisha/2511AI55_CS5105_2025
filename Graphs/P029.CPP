#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm> // For std::min

using namespace std;

// Using long long for distances
using ll = long long;
const ll INF = LLONG_MAX / 2;
using Edge = tuple<int, int, int>; // (u, v, w)
using pii = pair<int, int>; // (neighbor, weight)

// Helper function to print a vector
void printVector(const string& title, const vector<ll>& vec) {
    cout << title << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == INF) cout << "-1";
        else cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force (DFS) ---
// This is not guaranteed shortest, just relaxes along DFS paths
void dfs_brute(int node, ll dist, const vector<vector<pii>>& adj, vector<ll>& min_dist) {
    for (auto& edge : adj[node]) {
        int nei = edge.first;
        int w = edge.second;
        if (min_dist[nei] > dist + w) {
            min_dist[nei] = dist + w;
            dfs_brute(nei, dist + w, adj, min_dist);
        }
    }
}

vector<ll> brute(int V, const vector<Edge>& edge) {
    vector<vector<pii>> adj(V);
    for (const auto& e : edge) {
        adj[get<0>(e)].push_back({get<1>(e), get<2>(e)});
    }
    
    vector<ll> min_dist(V, INF);
    min_dist[0] = 0;
    dfs_brute(0, 0, adj, min_dist);
    return min_dist;
}

// --- Optimal (Topological Sort) ---
vector<ll> optimal(int V, const vector<Edge>& edge) {
    vector<vector<pii>> adj(V);
    vector<int> indegree(V, 0);
    for (const auto& e : edge) {
        adj[get<0>(e)].push_back({get<1>(e), get<2>(e)});
        indegree[get<1>(e)]++;
    }

    queue<int> q;
    for (int i = 0; i < V; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    vector<int> topo;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        topo.push_back(node);
        for (auto& edge : adj[node]) {
            int nei = edge.first;
            indegree[nei]--;
            if (indegree[nei] == 0) {
                q.push(nei);
            }
        }
    }

    vector<ll> mindist(V, INF);
    // Assuming source is 0, but a robust implementation
    // would find the source (or use the one given)
    mindist[0] = 0; 
    
    for (int i : topo) {
        if (mindist[i] != INF) {
            for (auto& edge : adj[i]) {
                int nei = edge.first;
                int w = edge.second;
                if (mindist[nei] > mindist[i] + w) {
                    mindist[nei] = mindist[i] + w;
                }
            }
        }
    }
    return mindist;
}

int main() {
    int V = 6;
    vector<Edge> edges = {
        {0, 1, 2}, {0, 4, 1},
        {1, 2, 3}, {4, 2, 2},
        {4, 5, 4}, {2, 3, 6},
        {5, 3, 1}
    };

    printVector("Brute Force:", brute(V, edges));
    printVector("Optimal TopoSort:", optimal(V, edges));

    return 0;
}