#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // Function to return maximum sum of non-adjacent elements
    int maxSum(vector<int>& nums) {
        // Handle edge case for empty input
        if (nums.empty()) return 0;

        // Initialize two tracking sums
        int prev2 = 0;
        int prev = nums[0];

        // Traverse through the array
        for (int i = 1; i < nums.size(); i++) {
            // Include current by adding it to two steps back
            int include = nums[i] + prev2;

            // Exclude current by taking previous best
            int exclude = prev;

            // Choose max of include and exclude
            int curr = max(include, exclude);

            // Update tracking variables
            prev2 = prev;
            prev = curr;
        }

        // Final result is stored in prev
        return prev;
    }
};
/*
TC-O(n)
SC-O(1)
*/

int main() {
    vector<int> arr = {3, 2, 5, 10, 7};
    Solution obj;
    cout << obj.maxSum(arr) << endl;
    return 0;
}