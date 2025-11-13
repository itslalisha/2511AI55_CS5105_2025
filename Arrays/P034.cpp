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

vector<vector<int>> optimal(vector<int>& nums, int target) {
    int n = nums.size();
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;

    for (int i = 0; i < n - 3; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) {
            continue; // Skip duplicates
        }
        for (int j = i + 1; j < n - 2; ++j) {
            // The original python code has a bug here: `if j > 0` should be `if j > i + 1`
            // Translating exactly:
            if (j > i + 1 && nums[j] == nums[j - 1]) {
                continue; // Skip duplicates
            }
            int k = j + 1;
            int l = n - 1;
            while (k < l) {
                // Use long long for sum to prevent overflow
                long long tot = (long long)nums[i] + nums[j] + nums[k] + nums[l];
                if (tot == target) {
                    res.push_back({nums[i], nums[j], nums[k], nums[l]});
                    k++;
                    l--;
                    while (k < l && nums[k] == nums[k - 1]) {
                        k++;
                    }
                    while (l > k && nums[l] == nums[l + 1]) {
                        l--;
                    }
                } else if (tot > target) {
                    l--;
                } else {
                    k++;
                }
            }
        }
    }
    return res;
}

int main() {
    vector<int> nums = {1, 0, -1, 0, -2, 2};
    int target = 0;
    printVectorVector(optimal(nums, target));
    return 0;
}