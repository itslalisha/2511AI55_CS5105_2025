#include <bits/stdc++.h>
using namespace std;

// Solution class for minimum falling path sum with space optimization
class Solution {
public:
    // Function to find minimum falling path sum using bottom-up DP with O(m) space
    int minFallingPathSum(vector<vector<int>>& matrix) {
        int n = matrix.size();
        int m = matrix[0].size();

        // Initialize dp with last row of matrix
        vector<int> dp(matrix[n - 1]);

        // Iterate from second last row up to first row
        for (int row = n - 2; row >= 0; row--) {
            vector<int> curr(m, 0);
            for (int col = 0; col < m; col++) {
                int down = dp[col];
                int downLeft = (col > 0) ? dp[col - 1] : 1e9;
                int downRight = (col < m - 1) ? dp[col + 1] : 1e9;

                // Calculate min path sum for current cell
                curr[col] = matrix[row][col] + min({down, downLeft, downRight});
            }
            dp = curr;  // update dp for next iteration
        }

        // Find minimum in dp (first row after iteration)
        return *min_element(dp.begin(), dp.end());
    }
};

int main() {
    vector<vector<int>> matrix = {
        {1, 4, 3, 1},
        {2, 3, -1, -1},
        {1, 1, -1, 8}
    };

    Solution sol;
    cout << "Minimum Falling Path Sum: " << sol.minFallingPathSum(matrix) << endl;
    return 0;
}
/*
Complexity Analysis
Time Complexity: O(N*M), we call our recursive function for every element in the grid.
Space Complexity: O(N) , space used to store the 2 arrays having values of current and previous rows.
*/