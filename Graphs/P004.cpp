#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> nearest(vector<vector<int>> &grid){

int n=grid.size();
int m=grid[0].size();
vector<vector<int>>dist(n,vector<int>(m, -1));
queue<pair<int,int>>q;

//push cell which having 1
for(int i=0; i<n; i++){
    for(int j=0; j<m; j++){
        if(grid[i][j]==1){
            dist[i][j]=0;
            q.push({i,j});
        }
    }
}

//direction
int dx[]={-1,0,1,0};
int dy[]={0,1,0,-1};


while(!q.empty()){
    pair<int,int> p = q.front();
   int x = p.first;
   int y = p.second;

    q.pop();
   
for(int dir=0;dir<4;dir++){
  int nx=x+dx[dir];
  int ny=y+dy[dir];
    if(nx>=0&&nx<n&&ny>=0&&ny<m&&dist[nx][ny]==-1){
        dist[nx][ny]=dist[x][y]+1;
        q.push({nx,ny});
    }

}
}
return dist;
}



int main(){
    int n,m;
    cout<<"enter roes and col: ";
    cin>>n>>m;
    vector<vector<int>> grid(n,vector<int>(m));
    cout<<"enter grid values(either 0 or 1):\n";
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            cin>>grid[i][j];
        }
    }
    vector<vector<int>> ans = nearest(grid);

    cout<<"Distance Matrix:\n";
    for(int i=0; i<n; i++){
        for(int j=0;j<m;j++){
            cout<<ans[i][j]<<" ";
        }
        cout<<"\n";
    }
    return 0;
}