#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

// Using long long for distances
using ll = long long;
const ll INF = LLONG_MAX / 2;
// {neighbor, weight}
using pii = pair<int, int>;
// {distance, node} for priority queue
using pli = pair<ll, int>; 

// Helper function to print a vector
void printVector(const vector<ll>& vec) {
    cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == INF) cout << "-1";
        else cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

vector<ll> dijkstras(int V, const vector<vector<pii>>& adj, int S) {
    vector<ll> dist(V, INF);
    dist[S] = 0;
    
    // Min-heap: {distance, node}
    priority_queue<pli, vector<pli>, greater<pli>> q;
    q.push({0, S});

    while (!q.empty()) {
        ll d = q.top().first;
        int u = q.top().second;
        q.pop();

        // Optimization: if we already found a shorter path
        if (d > dist[u]) {
            continue;
        }

        for (auto& edge : adj[u]) {
            int v = edge.first;
            int w = edge.second;
            if (dist[v] > d + w) {
                dist[v] = d + w;
                q.push({dist[v], v});
            }
        }
    }
    return dist;
}

int main() {
    int V = 5;
    vector<vector<pii>> adj = {
        {{1, 2}, {2, 4}},        
        {{0, 2}, {2, 1}, {3, 7}},
        {{0, 4}, {1, 1}, {4, 3}},
        {{1, 7}, {4, 2}},        
        {{2, 3}, {3, 2}}         
    };
    int S = 0;
    
    printVector(dijkstras(V, adj, S));
    
    return 0;
}