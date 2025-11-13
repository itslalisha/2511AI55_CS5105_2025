#include <iostream>
#include <vector>
#include <set>
#include <numeric> // For std::iota

using namespace std;

// --- Brute Force (DFS) ---
void dfs_brute(int r, int c, int row, int col, const vector<vector<int>>& grid, 
               vector<vector<bool>>& visited) {
    visited[r][c] = true;
    int dr[] = {1, 0, -1, 0, 1, 1, -1, -1};
    int dc[] = {0, 1, 0, -1, 1, -1, 1, -1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (nr >= 0 && nr < row && nc >= 0 && nc < col && 
            grid[nr][nc] == 1 && !visited[nr][nc]) {
            dfs_brute(nr, nc, row, col, grid, visited);
        }
    }
}

int brute(const vector<vector<int>>& grid) {
    int row = grid.size();
    if (row == 0) return 0;
    int col = grid[0].size();
    vector<vector<bool>> visited(row, vector<bool>(col, false));
    int c = 0;
    
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 1 && !visited[i][j]) {
                dfs_brute(i, j, row, col, grid, visited);
                c++;
            }
        }
    }
    return c;
}

// --- Optimal (DSU) ---
class DSU {
    vector<int> parent;
    vector<int> rank;
public:
    DSU(int V) {
        parent.resize(V);
        iota(parent.begin(), parent.end(), 0); // Fill with 0, 1, 2, ...
        rank.assign(V, 1);
    }

    int find(int u) {
        if (parent[u] != u) {
            parent[u] = find(parent[u]); // Path compression
        }
        return parent[u];
    }

    void unite(int u, int v) {
        int pu = find(u);
        int pv = find(v);
        if (pu == pv) return;

        if (rank[pu] > rank[pv]) {
            parent[pv] = pu;
        } else if (rank[pv] > rank[pu]) {
            parent[pu] = pv;
        } else {
            parent[pv] = pu;
            rank[pu]++;
        }
    }
};

int optimal(const vector<vector<int>>& grid) {
    int row = grid.size();
    if (row == 0) return 0;
    int col = grid[0].size();
    DSU dsu(row * col);
    
    int dr[] = {1, 0, -1, 0, 1, 1, -1, -1};
    int dc[] = {0, 1, 0, -1, 1, -1, 1, -1};

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 1) {
                for (int k = 0; k < 8; ++k) {
                    int nr = i + dr[k];
                    int nc = j + dc[k];
                    if (nr >= 0 && nr < row && nc >= 0 && nc < col && grid[nr][nc] == 1) {
                        dsu.unite(i * col + j, nr * col + nc);
                    }
                }
            }
        }
    }

    set<int> res;
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 1) {
                res.insert(dsu.find(i * col + j));
            }
        }
    }
    return res.size();
}

int main() {
    vector<vector<int>> grid = {
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0},
        {1, 1, 0, 1}
    };
    
    cout << "Brute: " << brute(grid) << endl;
    cout << "Optimal: " << optimal(grid) << endl;

    return 0;
}