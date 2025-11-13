#include <iostream>
#include <vector>
#include <algorithm> // For std::min, std::max
#include <climits>   // For INT_MAX

using namespace std;

// Helper function to print a 2D vector (matrix)
void printMatrix(const vector<vector<int>>& grid) {
    for (const auto& row : grid) {
        cout << "  [";
        for (size_t j = 0; j < row.size(); ++j) {
            if (row[j] == INT_MAX / 2) {
                cout << "INF";
            } else {
                cout << row[j];
            }
            if (j < row.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

// Floyd-Warshall function (modifies matrix in-place)
void floyd_warshall(vector<vector<int>>& matrix) {
    int n = matrix.size();
    const int INF = INT_MAX / 2; // Use this to avoid overflow (INF + INF)

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (matrix[i][j] == -1 && i != j) {
                matrix[i][j] = INF;
            }
        }
        // matrix[i][i] is already 0 in the input
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (matrix[i][k] != INF && matrix[k][j] != INF) {
                    matrix[i][j] = min(matrix[i][j], matrix[i][k] + matrix[k][j]);
                }
            }
        }
    }

    // Convert INF back to -1 for output, matching Python logic
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (matrix[i][j] == INF && i != j) {
                matrix[i][j] = -1;
            }
        }
    }
    // The function returns void as it modifies in-place
}

int main() {
    vector<vector<int>> mat = {
        {0, 3, -1, 7},
        {-1, 0, 2, -1},
        {-1, -1, 0, 1},
        {6, -1, -1, 0}
    };

    floyd_warshall(mat);
    printMatrix(mat);

    return 0;
}