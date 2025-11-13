#include <iostream>
#include <vector>
#include <numeric> // For std::iota

using namespace std;

// Helper to print vector
void printVector(const string& title, const vector<int>& vec) {
    cout << title << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force (DFS) ---
void dfs(int r, int c, int n, int m, const vector<vector<int>>& matrix, vector<vector<bool>>& visited) {
    visited[r][c] = true;
    int dr[] = {0, 1, -1, 0};
    int dc[] = {1, 0, 0, -1};
    for (int i = 0; i < 4; ++i) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] == 1 && !visited[nr][nc]) {
            dfs(nr, nc, n, m, matrix, visited);
        }
    }
}

vector<int> brute(int n, int m, int k, const vector<pair<int, int>>& arr) {
    vector<vector<int>> matrix(n, vector<int>(m, 0));
    vector<int> res;
    
    for (const auto& p : arr) {
        int r = p.first;
        int c = p.second;
        
        // Handle case where op is repeated
        if (matrix[r][c] == 1) {
            res.push_back(res.empty() ? 0 : res.back());
            continue;
        }

        matrix[r][c] = 1;
        int count = 0;
        vector<vector<bool>> visited(n, vector<bool>(m, false));
        
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (!visited[i][j] && matrix[i][j] == 1) {
                    dfs(i, j, n, m, matrix, visited);
                    count++;
                }
            }
        }
        res.push_back(count);
    }
    return res;
}

// --- Optimal (DSU) ---
class DSU {
    vector<int> parent;
    vector<int> rank;
public:
    DSU(int V) {
        parent.resize(V);
        iota(parent.begin(), parent.end(), 0);
        rank.assign(V, 1);
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // Returns true if union was performed, false if already in same set
    bool unite(int x, int y) {
        int px = find(x);
        int py = find(y);
        if (px == py) return false;
        
        if (rank[px] > rank[py]) {
            parent[py] = px;
        } else if (rank[py] > rank[px]) {
            parent[px] = py;
        } else {
            parent[py] = px;
            rank[px]++;
        }
        return true;
    }
};

vector<int> optimal(int n, int m, int k, const vector<pair<int, int>>& arr) {
    DSU dsu(n * m);
    vector<int> res;
    vector<vector<int>> matrix(n, vector<int>(m, 0));
    int count = 0;
    int dr[] = {0, 1, -1, 0};
    int dc[] = {1, 0, 0, -1};

    for (const auto& p : arr) {
        int r = p.first;
        int c = p.second;

        // Handle case where op is repeated
        if (matrix[r][c] == 1) {
            res.push_back(count);
            continue;
        }

        matrix[r][c] = 1;
        count++;
        int id1 = r * m + c; // Correct 1D index
        
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            int id2 = nr * m + nc; // Correct 1D index
            
            if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] == 1) {
                if (dsu.unite(id1, id2)) {
                    count--;
                }
            }
        }
        res.push_back(count);
    }
    return res;
}

int main() {
    int n = 3, m = 3, k = 5;
    vector<pair<int, int>> ops = {{0, 0}, {0, 1}, {1, 2}, {2, 1}, {1, 1}};
    
    printVector("Brute: ", brute(n, m, k, ops));
    printVector("Optimal: ", optimal(n, m, k, ops));
    return 0;
}