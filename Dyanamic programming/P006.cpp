#include<bits/stdc++.h>
using namespace std;
     int mazeObstaclesUtil(int i, int j, vector<vector<int>> &maze, vector<vector<int>> &dp) {
    // Base cases
  
    if (i == 0 && j == 0) return maze[i][j]; // If we reach the destination (0, 0), return value  at (0,0)
    if (i < 0 || j < 0) return INT_MAX; // If we go out of bounds, return max
    if (dp[i][j] != -1) return dp[i][j]; // If the result is already computed, return it

    // Recursive calls to explore paths from (i, j) to (0, 0)
    int up = mazeObstaclesUtil(i - 1, j, maze, dp);
    int left = mazeObstaclesUtil(i, j - 1, maze, dp);

    // Store the result in the DP table and return it
    return dp[i][j] =maze[i][j]+min(up,left);
}

 int minPathSum(vector<vector<int>>& grid) {
    int n=grid[0].size();
    int m=grid.size();    
    vector<vector<int>> dp(n, vector<int>(m, -1)); // DP table to memoize results
    return  mazeObstaclesUtil(n - 1, m - 1, grid, dp); // Start from the bottom-right corner
}
/*
TC-O(N*M)
SC_O(N)*/
int main() {
    vector<vector<int>> maze{
        {1, 3, 1},
        {2, 5, 1},
        {4, 2, 1}
    };

    // int n = maze.size();
    // int m = maze[0].size();

    cout << "min weight of the path from source to dest in given maze " << minPathSum(maze) << endl;
    return 0;
}
     