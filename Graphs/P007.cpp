#include <bits/stdc++.h>
using namespace std;

int shortestPath(vector<vector<int>> &grid, pair<int,int> src, pair<int,int> dst) {
    int n = grid.size();
    if (n == 0) return -1;
    int m = grid[0].size();

    if (src.first < 0 || src.first >= n || src.second < 0 || src.second >= m) return -1;
    if (dst.first < 0 || dst.first >= n || dst.second < 0 || dst.second >= m) return -1;
    if (grid[src.first][src.second] == 0 || grid[dst.first][dst.second] == 0) return -1;

    const int INF = INT_MAX;
    vector<vector<int>> dist(n, vector<int>(m, INF));
    queue<pair<int,int>> q;

    dist[src.first][src.second] = 0;
    q.push(src);

    int dr[4] = {-1, 0, 1, 0};
    int dc[4] = {0, 1, 0, -1};

    while (!q.empty()) {
        pair<int,int> cur = q.front(); q.pop();
        int r = cur.first, c = cur.second;

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr >= 0 && nr < n && nc >= 0 && nc < m &&
                grid[nr][nc] == 1 && dist[r][c] != INF &&
                dist[r][c] + 1 < dist[nr][nc]) {

                dist[nr][nc] = dist[r][c] + 1;
                q.push({nr, nc});
            }
        }
    }

    return dist[dst.first][dst.second] == INF ? -1 : dist[dst.first][dst.second];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    // Input format:
    // n m
    // n lines of m integers (0/1)
    // sr sc dr dc
    cin >> n >> m;

    vector<vector<int>> grid(n, vector<int>(m));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            cin >> grid[i][j];

    int sr, sc, dr, dc;
    cin >> sr >> sc >> dr >> dc;

    int ans = shortestPath(grid, {sr, sc}, {dr, dc});
    cout << ans << "\n";
    return 0;
}
