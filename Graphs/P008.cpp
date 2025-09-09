#include<bits/stdc++.h>
using namespace std;

//TC- ElogV
//n*m*4*log(n*m)
// SC- n*m

int MinimumEffort(int rows, int columns, vector<vector<int>> &heights) {
    if (heights.empty() || heights[0].empty()) return 0;
    int n = heights.size();
    int m = heights[0].size();

    const int INF = 1e9;
    using Node = pair<int, pair<int,int>>; // {effort, {r,c}}
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    vector<vector<int>> dist(n, vector<int>(m, INF));

    dist[0][0] = 0;
    pq.push({0, {0, 0}});

    int dr[4] = {-1, 0, 1, 0};
    int dc[4] = {0, 1, 0, -1};

    while (!pq.empty()) {
        Node it = pq.top(); pq.pop();
        int diff = it.first;
        int row = it.second.first;
        int col = it.second.second;

        if (diff > dist[row][col]) continue; // stale entry

        if (row == n - 1 && col == m - 1) return diff;

        for (int i = 0; i < 4; ++i) {
            int nr = row + dr[i];
            int nc = col + dc[i];
            if (nr < 0 || nr >= n || nc < 0 || nc >= m) continue;

            int newEffort = max(diff, abs(heights[row][col] - heights[nr][nc]));
            if (newEffort < dist[nr][nc]) {
                dist[nr][nc] = newEffort;
                pq.push({newEffort, {nr, nc}});
            }
        }
    }

    return dist[n-1][m-1] == INF ? -1 : dist[n-1][m-1];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int rows, cols;
    cin >> rows >> cols;

    vector<vector<int>> heights(rows, vector<int>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cin >> heights[i][j];
        }
    }

    int result = MinimumEffort(rows, cols, heights);
    cout << result << "\n";

    return 0;
}
