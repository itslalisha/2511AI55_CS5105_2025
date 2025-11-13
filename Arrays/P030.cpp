#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
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

// Brute force solution using a set to store unique triplets
vector<vector<int>> brute(vector<int>& nums) {
    int n = nums.size();
    set<vector<int>> res_set;
    for (int i = 0; i < n - 2; ++i) {
        for (int j = i + 1; j < n - 1; ++j) {
            for (int k = j + 1; k < n; ++k) {
                if (nums[i] + nums[j] + nums[k] == 0) {
                    vector<int> triplet = {nums[i], nums[j], nums[k]};
                    sort(triplet.begin(), triplet.end());
                    res_set.insert(triplet);
                }
            }
        }
    }
    return vector<vector<int>>(res_set.begin(), res_set.end());
}

// Optimal solution (Two-Pointer Technique)
vector<vector<int>> optimal(vector<int>& nums) {
    int n = nums.size();
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;

    for (int i = 0; i < n - 2; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) {
            continue; // Skip duplicate
        }
        int left = i + 1;
        int right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                res.push_back({nums[i], nums[left], nums[right]});
                left++;
                right--;
                while (left < right && nums[left] == nums[left - 1]) {
                    left++; // Skip duplicates
                }
                while (left < right && nums[right] == nums[right + 1]) {
                    right--; // Skip duplicates
                }
            } else if (sum < 0) {
                left++;
            } else {
                right--;
            }
        }
    }
    return res;
}

int main() {
    cout << "Enter array: ";
    string line;
    getline(cin, line);

    vector<int> nums;
    stringstream ss(line);
    int num;
    while (ss >> num) {
        nums.push_back(num);
    }

    // Need a copy for the brute function if it modifies the array (it doesn't, but good practice)
    vector<int> nums_brute = nums; 
    vector<int> nums_optimal = nums;

    cout << "Brute: ";
    printVectorVector(brute(nums_brute));

    cout << "Optimal: ";
    printVectorVector(optimal(nums_optimal));

    return 0;
}