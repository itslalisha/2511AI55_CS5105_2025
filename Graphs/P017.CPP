#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// Helper to print matrix
void printMatrix(const vector<vector<char>>& grid) {
    for (const auto& row : grid) {
        for (char c : row) {
            cout << c << " ";
        }
        cout << endl;
    }
}

// --- Brute Force (DFS) ---
// Note: This logic is convoluted and error-prone, translated as-is.
bool dfs_brute(int r, int c, int row, int col, const vector<vector<char>>& grid, 
               vector<vector<bool>>& visited, int& count) {
    
    // Check for boundary
    if (r == 0 || c == 0 || r == row - 1 || c == col - 1) {
        return true; // Hit boundary
    }
    
    visited[r][c] = true;
    count++; // Increment count for this cell
    int dr[] = {0, 1, -1, 0};
    int dc[] = {1, 0, 0, -1};
    
    bool hit_boundary = false;
    for (int i = 0; i < 4; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (!visited[nr][nc] && grid[nr][nc] == '1') {
            if (dfs_brute(nr, nc, row, col, grid, visited, count)) {
                hit_boundary = true;
            }
        }
    }
    return hit_boundary;
}

int brute(const vector<vector<char>>& grid) {
    int row = grid.size();
    if (row == 0) return 0;
    int col = grid[0].size();
    vector<vector<bool>> visited(row, vector<bool>(col, false));
    int tot = 0;
    
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == '1' && !visited[i][j]) {
                int count = 0;
                if (!dfs_brute(i, j, row, col, grid, visited, count)) {
                    // Only add count if DFS did NOT hit a boundary
                    tot += count;
                }
            }
        }
    }
    return tot;
}

// --- Optimal (Boundary BFS) ---
int optimal(vector<vector<char>> grid) {
    int row = grid.size();
    if (row == 0) return 0;
    int col = grid[0].size();
    
    queue<pair<int, int>> q;
    vector<vector<bool>> visited(row, vector<bool>(col, false));
    int dr[] = {0, 1, -1, 0};
    int dc[] = {1, 0, 0, -1};

    // Add all boundary '1's to the queue
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if ((i == 0 || j == 0 || i == row - 1 || j == col - 1) && grid[i][j] == '1') {
                q.push({i, j});
                visited[i][j] = true;
            }
        }
    }

    // BFS from boundaries
    while (!q.empty()) {
        int r = q.front().first;
        int c = q.front().second;
        q.pop();

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr >= 0 && nr < row && nc >= 0 && nc < col && grid[nr][nc] == '1' && !visited[nr][nc]) {
                visited[nr][nc] = true;
                q.push({nr, nc});
            }
        }
    }

    // Count unvisited '1's
    int count = 0;
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == '1' && !visited[i][j]) {
                count++;
            }
        }
    }
    return count;
}

int main() {
    vector<vector<char>> grid = {
        {'0', '0', '0', '0'},
        {'1', '0', '1', '0'},
        {'0', '1', '1', '0'},
        {'0', '0', '0', '0'}
    };
    
    // Create a copy for optimal, as it modifies the grid
    vector<vector<char>> grid_brute = grid;
    vector<vector<char>> grid_optimal = grid;

    cout << "Brute: " << brute(grid_brute) << endl;
    cout << "Optimal: " << optimal(grid_optimal) << endl;

    return 0;
}