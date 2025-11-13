#include <iostream>
#include <vector>
#include <algorithm> // For std::swap and std::reverse

using namespace std;

// Helper function to print a 2D vector (matrix)
void printMatrix(const vector<vector<int>>& grid) {
    for (const auto& row : grid) {
        cout << "  [";
        for (size_t j = 0; j < row.size(); ++j) {
            cout << row[j];
            if (j < row.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

// Brute force solution (out-of-place)
vector<vector<int>> brute(const vector<vector<int>>& grid) {
    if (grid.empty()) return {};
    int n = grid.size();
    // Initialize result matrix with n rows and n cols, all 0s
    vector<vector<int>> res(n, vector<int>(n, 0)); 
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            res[j][n - 1 - i] = grid[i][j];
        }
    }
    return res;
}

// Optimal solution (in-place)
// Takes by reference (&) to modify the original vector
vector<vector<int>> optimal(vector<vector<int>>& grid) {
    if (grid.empty()) return {};
    int n = grid.size();

    // 1. Transpose the matrix
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            std::swap(grid[i][j], grid[j][i]);
        }
    }

    // 2. Reverse each row
    for (int i = 0; i < n; ++i) {
        std::reverse(grid[i].begin(), grid[i].end());
    }
    return grid;
}

int main() {
    vector<vector<int>> mat2 = {
      {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9}
    };
    
    // Create a copy for the optimal function, since it modifies in-place
    vector<vector<int>> mat2_optimal = mat2;

    // 'brute' takes by const reference, so it won't modify mat2
    cout << "Brute: " << endl;
    printMatrix(brute(mat2));

    // 'optimal' takes by reference and modifies mat2_optimal
    cout << "Optimal: " << endl;
    printMatrix(optimal(mat2_optimal));

    return 0;
}