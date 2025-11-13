#include <iostream>
#include <vector>
#include <numeric> // For std::iota
#include <algorithm> // For std::max
#include <set>

using namespace std;

// --- Brute Force (DFS) ---
void dfs_brute(int r, int c, int row, int col, const vector<vector<int>>& grid, 
               vector<vector<bool>>& visited, int& count) {
    visited[r][c] = true;
    count++;
    int dr[] = {0, 1, -1, 0};
    int dc[] = {1, 0, 0, -1};
    for (int i = 0; i < 4; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (nr >= 0 && nr < row && nc >= 0 && nc < col && grid[nr][nc] == 1 && !visited[nr][nc]) {
            dfs_brute(nr, nc, row, col, grid, visited, count);
        }
    }
}

int brute(vector<vector<int>> grid) {
    int row = grid.size();
    if (row == 0) return 0;
    int col = grid[0].size();
    int max_area = 0;
    bool has_zero = false;

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 0) {
                has_zero = true;
                grid[i][j] = 1;
                int count = 0;
                vector<vector<bool>> visited(row, vector<bool>(col, false));
                // Need to start DFS from the cell we just flipped
                dfs_brute(i, j, row, col, grid, visited, count); 
                grid[i][j] = 0;
                max_area = max(max_area, count);
            }
        }
    }

    // If no zeros were found, the whole grid is one island.
    if (!has_zero) {
        return row * col;
    }
    return max_area;
}

// --- Optimal (DSU) ---
class DSU {
public:
    vector<int> parent;
    vector<int> size;
    DSU(int V) {
        parent.resize(V);
        iota(parent.begin(), parent.end(), 0);
        size.assign(V, 1);
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    void unite(int x, int y) {
        int px = find(x);
        int py = find(y);
        if (px == py) {
            return;
        }
        if (size[px] >= size[py]) {
            parent[py] = px;
            size[px] += size[py];
        } else {
            parent[px] = py;
            size[py] += size[px];
        }
    }
};

int optimal(const vector<vector<int>>& grid) {
    int row = grid.size();
    if (row == 0) return 0;
    int col = grid[0].size();
    DSU dsu(row * col);
    int dr[] = {0, 1}; // Only check right and down
    int dc[] = {1, 0};

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 1) {
                for (int k = 0; k < 2; ++k) {
                    int nr = i + dr[k];
                    int nc = j + dc[k];
                    // Python code has a bug here: nr*row+nc should be nr*col+nc
                    if (nr >= 0 && nr < row && nc >= 0 && nc < col && grid[nr][nc] == 1) {
                         dsu.unite(i * col + j, nr * col + nc);
                    }
                }
            }
        }
    }

    int max_area = 0;
    int dr_all[] = {0, 1, -1, 0};
    int dc_all[] = {1, 0, 0, -1};

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 0) {
                set<int> seen_parents;
                int area = 1; // Start with the 0 cell we are flipping
                for (int k = 0; k < 4; ++k) {
                    int nr = i + dr_all[k];
                    int nc = j + dc_all[k];
                    if (nr >= 0 && nr < row && nc >= 0 && nc < col && grid[nr][nc] == 1) {
                        int root = dsu.find(nr * col + nc);
                        if (seen_parents.find(root) == seen_parents.end()) {
                            area += dsu.size[root];
                            seen_parents.insert(root);
                        }
                    }
                }
                max_area = max(max_area, area);
            }
        }
    }

    // If max_area is still 0, it means no 0s were found.
    if (max_area == 0) {
        return row * col; 
    }
    return max_area;
}

int main() {
    vector<vector<int>> grid = {
        {1, 0, 1, 1},
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {1, 0, 1, 1}
    };
    
    // Create copy for brute, as it modifies the grid
    vector<vector<int>> grid_brute = grid;

    cout << "Brute Force: " << brute(grid_brute) << endl;
    cout << "Optimal: " << optimal(grid) << endl;
    return 0;
}