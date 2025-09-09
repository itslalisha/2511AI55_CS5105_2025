#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int countPaths(int n, vector<vector<int>>& roads) {
        int MOD = 1e9 + 7;
        
        vector<vector<pair<int, int>>> adj(n);
        for (int i = 0; i < (int)roads.size(); i++) {
            adj[roads[i][0]].push_back({roads[i][1], roads[i][2]});
            adj[roads[i][1]].push_back({roads[i][0], roads[i][2]});
        }
        
        vector<long long> dist(n, LLONG_MAX);
        vector<int> ways(n, 0);
        dist[0] = 0;
        ways[0] = 1;
        
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
        pq.push({0, 0});
        
        while(!pq.empty()) {
            long long distance = pq.top().first;
            int node = pq.top().second;
            pq.pop();
            
            if (distance > dist[node]) continue; // skip outdated entries
            
            for (auto nbr: adj[node]) {
                int adjNode = nbr.first;
                int wt = nbr.second;
                
                if (distance + wt < dist[adjNode]) {
                    dist[adjNode] = distance + wt;
                    pq.push({dist[adjNode], adjNode});
                    ways[adjNode] = ways[node];
                }
                else if (distance + wt == dist[adjNode]) {
                    ways[adjNode] = (ways[adjNode] + ways[node]) % MOD;
                }
            }
        }
        return ways[n-1];
    }
};

// ---------------- Driver Code ----------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cout << "Enter number of nodes and roads: ";
    cin >> n >> m;

    vector<vector<int>> roads(m, vector<int>(3));
    cout << "Enter edges (u v wt):\n";
    for (int i = 0; i < m; i++) {
        cin >> roads[i][0] >> roads[i][1] >> roads[i][2];
    }

    Solution obj;
    int result = obj.countPaths(n, roads);

    cout << "Number of ways to reach node " << n-1 << " = " << result << "\n";

    return 0;
}
