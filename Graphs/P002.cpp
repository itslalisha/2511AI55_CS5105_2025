#include <bits/stdc++.h>
using namespace std;

class Solution {
  private:
    void dfs(int node, vector<vector<int>> &adjLs, vector<int> &vis) {
        vis[node] = 1;
        for (auto it : adjLs[node]) {
            if (!vis[it]) {
                dfs(it, adjLs, vis);
            }
        }
    }

  public:
    int numProvinces(vector<vector<int>> adj, int V) {
        // adjacency list
        vector<vector<int>> adjLs(V);

        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (adj[i][j] == 1 && i != j) {
                    adjLs[i].push_back(j);
                }
            }
        }

        vector<int> vis(V, 0);
        int cnt = 0;

        for (int i = 0; i < V; i++) {
            if (!vis[i]) {
                cnt++;
                dfs(i, adjLs, vis);
            }
        }
        return cnt;
    }
};

int main() {
    int V;
    cin >> V;

    vector<vector<int>> adj(V, vector<int>(V));
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            cin >> adj[i][j];
        }
    }

    Solution sol;
    cout << sol.numProvinces(adj, V) << endl;

    return 0;
}
