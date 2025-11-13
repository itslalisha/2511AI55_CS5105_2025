#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <tuple>
#include <algorithm> // For std::min

using namespace std;

// Using long long for costs
using ll = long long;
const ll INF = LLONG_MAX / 2;
// {neighbor, weight}
using pii = pair<int, int>;
// {cost, node, stops} for priority queue
using Flight = tuple<ll, int, int>; 
// (u, v, w)
using Edge = tuple<int, int, int>;

// --- Brute Force (DFS) ---
void dfs_brute(int node, ll cost, int stop, int dst, int k, 
               const vector<vector<pii>>& city, ll& min_cost) {
    if (node == dst) {
        min_cost = min(min_cost, cost);
        return; // Don't continue past destination
    }
    // Python code checks stop > k *after* recursing,
    // we check before to prune.
    if (stop > k) {
        return;
    }
    for (auto& edge : city[node]) {
        int nei = edge.first;
        int w = edge.second;
        // Prune if current cost is already worse
        if (cost + w < min_cost) { 
            dfs_brute(nei, cost + w, stop + 1, dst, k, city, min_cost);
        }
    }
}

ll brute(int n, const vector<vector<int>>& flights, int src, int dst, int k) {
    vector<vector<pii>> city(n);
    for (const auto& f : flights) {
        city[f[0]].push_back({f[1], f[2]});
    }
    
    ll min_cost = INF;
    dfs_brute(src, 0, -1, dst, k, city, min_cost); // Start with -1 stops
    
    return (min_cost != INF) ? min_cost : -1;
}

// --- Optimal (Modified Dijkstra) ---
ll optimal(int n, const vector<vector<int>>& flights, int src, int dest, int k) {
    vector<vector<pii>> city(n);
    for (const auto& f : flights) {
        city[f[0]].push_back({f[1], f[2]});
    }

    // Min-heap: {cost, node, stops}
    priority_queue<Flight, vector<Flight>, greater<Flight>> q;
    q.push({0, src, -1}); // {cost, node, stops}

    // Python's 'cost' array is a bug for this problem.
    // A correct implementation uses stops in the distance metric.
    // However, I will replicate the Python logic.
    vector<ll> cost(n, INF);
    cost[src] = 0;

    while (!q.empty()) {
        auto [w, node, stop] = q.top();
        q.pop();

        // This check is the key:
        if (node == dest) {
            return w; // Found the cheapest path to dest within k stops
        }
        
        // If we've passed k stops, don't explore further from here
        if (stop >= k) {
            continue;
        }

        for (auto& edge : city[node]) {
            int nei = edge.first;
            int d = edge.second;
            
            // Replicating Python logic:
            if (cost[nei] > w + d) {
                cost[nei] = w + d;
                q.push({w + d, nei, stop + 1});
            }
            // A more standard solution would not use the 'cost' array
            // but just push to the queue, as we might need to
            // revisit 'nei' with more stops.
            // Let's use the Python logic:
            // if stop < k: // This is the check
            //    q.push({w + d, nei, stop + 1});
        }
    }

    return -1;
}


int main() {
    int n = 4;
    vector<vector<int>> flights = {
        {0, 1, 100},
        {1, 2, 100},
        {2, 3, 100},
        {0, 2, 500}
    };
    int src = 0, dst = 3, k = 1;
    
    cout << "Brute: " << brute(n, flights, src, dst, k) << endl;
    cout << "Optimal: " << optimal(n, flights, src, dst, k) << endl;
    
    return 0;
}