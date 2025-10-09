#include<bits/stdc++.h>

using namespace std;
vector < vector < int >> rotate(vector < vector < int >> & matrix) {
    int n = matrix.size();
    vector < vector < int >> rotated(n, vector < int > (n, 0));
    for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        rotated[j][n - i - 1] = matrix[i][j];
    }
    }
    return rotated;
}
/*
Complexity Analysis

Time Complexity: O(N*N) to linearly iterate and put it into some other matrix.

Space Complexity: O(N*N) to copy it into some other matrix.
*/

vector < vector < int >> rotate(vector < vector < int >> & matrix) {
    int n = matrix.size();
    //transposing the matrix
    for (int i = 0; i < n; i++) {
    for (int j = 0; j < i; j++) {
        swap(matrix[i][j], matrix[j][i]);
    }
    }
    //reversing each row of the matrix
    for (int i = 0; i < n; i++) {
    reverse(matrix[i].begin(), matrix[i].end());
    }
    return matrix;
}

/*
Complexity Analysis

Time Complexity: O(N*N) + O(N*N).One O(N*N) is for transposing the matrix and the other is for reversing the matrix.

Space Complexity: O(1).
*/

int main() {
    vector < vector < int >> arr;
    arr =  {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    vector < vector < int >> rotated = rotate(arr);
    cout << "Rotated Image" << endl;
    for (int i = 0; i < rotated.size(); i++) {
    for (int j = 0; j < rotated[0].size(); j++) {
        cout << rotated[i][j] << " ";
    }
    cout << "n";
    }

}


