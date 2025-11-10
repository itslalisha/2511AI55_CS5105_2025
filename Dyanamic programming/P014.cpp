#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // Function to find the minimum absolute difference between two subset sums
    int minSubsetSumDifference(vector<int>& arr, int n) {
        int totSum = 0;

        // Calculate the total sum of the array
        for (int i = 0; i < n; i++) {
            totSum += arr[i];
        }

        // Initialize a boolean vector 'prev' to represent the previous row of the DP table
        vector<bool> prev(totSum + 1, false);

        // Base case: If no elements are selected (sum is 0), it's a valid subset
        prev[0] = true;

        // Initialize the first row based on the first element of the array
        if (arr[0] <= totSum)
            prev[arr[0]] = true;

        // Fill in the DP table using a bottom-up approach
        for (int ind = 1; ind < n; ind++) {
            // Create a boolean vector 'cur' to represent the current row of the DP table
            vector<bool> cur(totSum + 1, false);
            cur[0] = true;

            for (int target = 1; target <= totSum; target++) {
                // Exclude the current element
                bool notTaken = prev[target];

                // Include the current element if it doesn't exceed the target
                bool taken = false;
                if (arr[ind] <= target)
                    taken = prev[target - arr[ind]];

                // Mark current target as achievable if either excluding or including current element works
                cur[target] = notTaken || taken;
            }

            // Set 'cur' as the 'prev' for the next iteration
            prev = cur;
        }

        int mini = INT_MAX;
        for (int i = 0; i <= totSum; i++) {
            if (prev[i]) {
                // Calculate the absolute difference between two subset sums
                int diff = abs(i - (totSum - i));
                mini = min(mini, diff);
            }
        }
        return mini;
    }
};

int main() {
    vector<int> arr = {1, 2, 3, 4};
    int n = arr.size();

    Solution sol;
    cout << "The minimum absolute difference is: " << sol.minSubsetSumDifference(arr, n) << "\n";

    return 0;
}
/*
Complexity Analysis
Time Complexity: O(N*K), there are total N*K states, where N is the length of array and K is the total sum of the array.
Space Complexity: O(N), we use two 1D arrays to store value of previous row and current row.
*/