#include <iostream>
#include <vector>
#include <algorithm> // Not strictly needed for this file, but good practice

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

// Brute force solution using two marker arrays
vector<vector<int>> brute(vector<vector<int>> grid) {
    if (grid.empty()) return {};
    int row = grid.size();
    int col = grid[0].size();
    
    vector<int> rz(row, 1);
    vector<int> cz(col, 1);

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (grid[i][j] == 0) {
                rz[i] = 0;
                cz[j] = 0;
            }
        }
    }

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            if (rz[i] == 0 || cz[j] == 0) {
                grid[i][j] = 0;
            }
        }
    }
    return grid;
}

// Optimal solution using the first row/col as markers
vector<vector<int>> optimal(vector<vector<int>> grid) {
    if (grid.empty()) return {};
    int row = grid.size();
    int col = grid[0].size();

    bool fr = false, fc = false;

    // Check if first row has a zero
    for (int j = 0; j < col; ++j) {
        if (grid[0][j] == 0) {
            fr = true;
            break;
        }
    }
    // Check if first col has a zero
    for (int i = 0; i < row; ++i) {
        if (grid[i][0] == 0) {
            fc = true;
            break;
        }
    }

    // Use first row/col as markers for the rest of the matrix
    for (int i = 1; i < row; ++i) {
        for (int j = 1; j < col; ++j) {
            if (grid[i][j] == 0) {
                grid[i][0] = 0;
                grid[0][j] = 0;
            }
        }
    }

    // Set zeroes based on markers
    for (int i = 1; i < row; ++i) {
        for (int j = 1; j < col; ++j) {
            if (grid[i][0] == 0 || grid[0][j] == 0) {
                grid[i][j] = 0;
            }
        }
    }

    // Set zeroes for first row if needed
    if (fr) {
        for (int j = 0; j < col; ++j) {
            grid[0][j] = 0;
        }
    }
    // Set zeroes for first col if needed
    if (fc) {
        for (int i = 0; i < row; ++i) {
            grid[i][0] = 0;
        }
    }
    return grid;
}

int main() {
    vector<vector<int>> matrix = {
      {1, 1, 1},
      {1, 0, 1},
      {1, 1, 1}
    };

    // Create copies, as the Python code does with [row[:] for row in matrix]
    vector<vector<int>> matrix_brute = matrix;
    vector<vector<int>> matrix_optimal = matrix;

    cout << "Brute: " << endl;
    printMatrix(brute(matrix_brute));

    cout << "Optimal: " << endl;
    printMatrix(optimal(matrix_optimal));

    return 0;
}