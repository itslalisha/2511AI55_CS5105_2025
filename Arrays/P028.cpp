#include <iostream>
#include <vector>
#include <utility> // For std::pair

using namespace std;

// Helper function to print a 1D vector
void printVector(const vector<int>& vec) {
    cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// "Brute" solution using a visited matrix
vector<int> brute_spiral(const vector<vector<int>>& matrix) {
    if (matrix.empty()) {
        return {};
    }
    int m = matrix.size();
    int n = matrix[0].size();
    vector<vector<bool>> visited(m, vector<bool>(n, false));
    vector<int> result;

    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    int dir_idx = 0;
    int x = 0, y = 0;

    for (int i = 0; i < m * n; ++i) {
        result.push_back(matrix[x][y]);
        visited[x][y] = true;
        int nx = x + directions[dir_idx].first;
        int ny = y + directions[dir_idx].second;

        if (0 <= nx && nx < m && 0 <= ny && ny < n && !visited[nx][ny]) {
            x = nx;
            y = ny;
        } else {
            dir_idx = (dir_idx + 1) % 4;
            x += directions[dir_idx].first;
            y += directions[dir_idx].second;
        }
    }
    return result;
}

// Optimal solution using boundary pointers
vector<int> optimal(const vector<vector<int>>& grid) {
    if (grid.empty()) {
        return {};
    }
    int row = grid.size();
    int col = grid[0].size();
    int left = 0, right = col - 1, top = 0, bottom = row - 1;
    vector<int> res;

    while (right >= left && top <= bottom) {
        // Move right
        for (int i = left; i <= right; ++i) {
            res.push_back(grid[top][i]);
        }
        top++;

        // Move down
        for (int i = top; i <= bottom; ++i) {
            res.push_back(grid[i][right]);
        }
        right--;

        // Move left (check if row still exists)
        if (top <= bottom) {
            for (int i = right; i >= left; --i) {
                res.push_back(grid[bottom][i]);
            }
            bottom--;
        }

        // Move up (check if col still exists)
        if (left <= right) {
            for (int i = bottom; i >= top; --i) {
                res.push_back(grid[i][left]);
            }
            left++;
        }
    }
    return res;
}

int main() {
    vector<vector<int>> matrix = {
      {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9}
    };

    // Neither function modifies the input, so no copies are needed.
    cout << "Brute:   ";
    printVector(brute_spiral(matrix));

    cout << "Optimal: ";
    printVector(optimal(matrix));

    return 0;
}