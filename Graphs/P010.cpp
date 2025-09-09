#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int makeConnected(int n, vector<vector<int>>& connections) {
        if ((int)connections.size() < n - 1) {
            return -1; // not enough cables
        }
        
        vector<int> parent(n);
        vector<int> rank(n, 0);
        
        // initialize DSU
        for (int i = 0; i < n; i++) parent[i] = i;
        
        function<int(int)> find = [&](int x) {
            if (parent[x] != x) parent[x] = find(parent[x]);
            return parent[x];
        };
        
        auto unite = [&](int x, int y) {
            int px = find(x), py = find(y);
            if (px == py) return false;
            if (rank[px] < rank[py]) swap(px, py);
            parent[py] = px;
            if (rank[px] == rank[py]) rank[px]++;
            return true;
        };
        
        // union all edges
        for (auto &edge : connections) {
            unite(edge[0], edge[1]);
        }
        
        // count distinct parents (components)
        int components = 0;
        for (int i = 0; i < n; i++) {
            if (find(i) == i) components++;
        }
        
        return components - 1;
    }
};

// ---------------- Driver Code ----------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cout << "Enter number of computers and connections: ";
    cin >> n >> m;

    vector<vector<int>> connections(m, vector<int>(2));
    cout << "Enter connections (u v):\n";
    for (int i = 0; i < m; i++) {
        cin >> connections[i][0] >> connections[i][1];
    }

    Solution obj;
    int result = obj.makeConnected(n, connections);

    cout << "Minimum operations needed = " << result << "\n";

    return 0;
}
