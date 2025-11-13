#include <iostream>
#include <vector>
#include <numeric>   // For std::iota
#include <algorithm> // For std::next_permutation
#include <map>
#include <queue>

using namespace std;

// Helper function to print a vector
void printVector(const string& title, const vector<int>& vec) {
    cout << title << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force ---
bool is_valid_order(const vector<int>& order, const vector<pair<int, int>>& edges, const map<int, int>& pos) {
    for (const auto& edge : edges) {
        if (pos.at(edge.first) > pos.at(edge.second)) {
            return false;
        }
    }
    return true;
}

vector<int> Brute(int V, const vector<pair<int, int>>& edges) {
    vector<int> vertices(V);
    iota(vertices.begin(), vertices.end(), 0); // Fill with 0, 1, 2, ...

    do {
        map<int, int> pos;
        for (int i = 0; i < V; ++i) {
            pos[vertices[i]] = i;
        }
        if (is_valid_order(vertices, edges, pos)) {
            return vertices;
        }
    } while (next_permutation(vertices.begin(), vertices.end()));

    return {}; // Return empty vector if no valid order found
}

// --- Optimal (Kahn's Algorithm) ---
vector<int> optimal(int V, const vector<pair<int, int>>& edge) {
    vector<int> indegree(V, 0);
    vector<vector<int>> adj(V);
    for (const auto& p : edge) {
        adj[p.first].push_back(p.second);
        indegree[p.second]++;
    }

    queue<int> q;
    for (int i = 0; i < V; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    vector<int> res;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        res.push_back(node);
        for (int nei : adj[node]) {
            indegree[nei]--;
            if (indegree[nei] == 0) {
                q.push(nei);
            }
        }
    }
    return res;
}

int main() {
    int V = 4;
    vector<pair<int, int>> edges = {{0, 1}, {0, 2}, {1, 3}, {2, 3}};
    
    printVector("Brute : ", Brute(V, edges));
    printVector("Optimal: ", optimal(V, edges));
    
    return 0;
}