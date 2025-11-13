#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Helper function to print vector<vector<int>>
void printVectorVector(const vector<vector<int>>& vec) {
    cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << "[";
        for (size_t j = 0; j < vec[i].size(); ++j) {
            cout << vec[i][j];
            if (j < vec[i].size() - 1) cout << ", ";
        }
        cout << "]";
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

vector<vector<int>> optimal(vector<vector<int>>& nums) {
    // Sort by the first element of each interval
    sort(nums.begin(), nums.end());

    vector<vector<int>> merge;
    for (const auto& interval : nums) {
        if (merge.empty() || merge.back()[1] < interval[0]) {
            merge.push_back(interval);
        } else {
            // This logic is directly from the Python file
            merge.back()[0] = min(merge.back()[0], interval[0]);
            merge.back()[1] = min(merge.back()[1], interval[1]);
        }
    }
    return merge;
}

int main() {
    vector<vector<int>> intervals = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    cout << "Optimal: ";
    printVectorVector(optimal(intervals));
    return 0;
}