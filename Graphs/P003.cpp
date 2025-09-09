#include <iostream>
#include <vector>
#include <queue>
using namespace std;
/* Rotting Oranges 
SC - O(N*M) + O(N*M) = O(N*M) {visit array, queue}  
TC - O(N*M) + O(N*M*4) = O(N*M) , N = no. of rows, M = no. of columns.
*/
class Solution {
public:
    int orangesRotting(vector<vector<int>>& grid) {
        if(grid.empty()) return 0;

        int n = grid.size();
        int m = grid[0].size();

        int countFresh = 0;
        // {{row, col}, time}
        queue<pair<pair<int, int>, int>> q;
        int visit[n][m];
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                if(grid[i][j] == 2) {
                    q.push({{i,j}, 0});
                    visit[i][j] = 1;
                }
                else {
                    visit[i][j] = 0;
                }
                if(grid[i][j] == 1) countFresh++;
            }
        }
        int time = 0;
        int drow[] = {-1,0,1,0};
        int dcol[] = {0,1,0,-1};

        while(!q.empty()) {
            int row = q.front().first.first;
            int col = q.front().first.second;
            int tm = q.front().second;
            time = max(time, tm);
            q.pop();
            for(int i = 0; i < 4; i++) {
                int nrow = row + drow[i];
                int ncol = col + dcol[i];
                if(nrow >= 0 && nrow < n && ncol >= 0 && ncol < m && visit[nrow][ncol] == 0 && grid[nrow][ncol] == 1) {
                    q.push({{nrow, ncol}, tm+1});
                    visit[nrow][ncol] = 1;
                    countFresh--;
                }
            } 
        }


        return countFresh ? -1 : time;
    }

};

class Solution {
private:
    bool isValid(int i,int j,int m,int n){
        return (i>=0 && i <m && j >= 0 && j <n);
    }

public:
    int orangesRotting(vector<vector<int>>& grid) {
        int m = grid.size();
        int n = grid[0].size();

        vector<vector<int>> vis(m , vector<int>(n));
        
        // {{r,c}, t}
        queue<pair<pair<int, int>, int>> q;
        int freshCount = 0;

        for(int i = 0; i<m; i++){
            for(int j = 0; j<n; j++){
                if(grid[i][j] == 2){
                    q.push({{i, j}, 0});
                    vis[i][j] = 1;
                } else{
                    vis[i][j] = 0;
                }

                if(grid[i][j] == 1) freshCount++;
            }
        }

        int directions[][2] = {{-1, 0}, {0,1}, {1, 0}, {0, -1}};
        int minTime = 0;

        while(!q.empty()){
            int r = q.front().first.first;
            int c = q.front().first.second;
            int t = q.front().second;
            q.pop();

            minTime = max(minTime, t);

            for(int i = 0; i<4; i++){
                int newRow = r + directions[i][0];
                int newCol = c + directions[i][1];

                if(isValid(newRow, newCol, m, n) && grid[newRow][newCol] == 1 && vis[newRow][newCol] == 0){
                    vis[newRow][newCol] = 1;
                    q.push({{newRow, newCol}, t + 1});
                    freshCount--;
                }
            }
        }

        return freshCount ? -1 : minTime;
    }
};