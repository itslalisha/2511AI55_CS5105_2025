#include <iostream>
#include <vector>
#include <tuple>
#include <climits> // For LLONG_MAX

using namespace std;

// Using long long for distances
using ll = long long;
const ll INF = LLONG_MAX / 2;
// (u, v, w)
using Edge = tuple<int, int, int>;

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

vector<ll> bellmanford(int V, const vector<Edge>& edge, int S) {
    vector<ll> dist(V, INF);
    dist[S] = 0;

    // Relax V-1 times
    for (int i = 0; i < V - 1; ++i) {
        for (const auto& e : edge) {
            int u = get<0>(e);
            int v = get<1>(e);
            int w = get<2>(e);
            if (dist[u] != INF && dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
            }
        }
    }

    // Check for negative cycles
    for (const auto& e : edge) {
        int u = get<0>(e);
        int v = get<1>(e);
        int w = get<2>(e);
        if (dist[u] != INF && dist[v] > dist[u] + w) {
            return {-1}; // Negative cycle detected
        }
    }
    
    return dist;
}

int main() {
    int V = 5;
    vector<Edge> edges = {
        {0, 1, -1}, {0, 2, 4}, {1, 2, 3}, {1, 3, 2},
        {1, 4, 2}, {3, 2, 5}, {3, 1, 1}, {4, 3, -3}
    };
    int S = 0;

    printVector("Shortest distances: ", bellmanford(V, edges, S));
    
    return 0;
}