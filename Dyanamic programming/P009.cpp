#include <bits/stdc++.h>
using namespace std;

// Class containing the solution method
class Solution {
public:

    // Function to compute minimum elements to reach target sum
    int minimumElements(vector<int>& arr, int T) {
        int n = arr.size();

        // Create two vectors for previous and current rows
        vector<int> prev(T + 1, 0);
        vector<int> cur(T + 1, 0);

        // Initialize base case for the first row
        for (int i = 0; i <= T; i++) {
            if (i % arr[0] == 0)
                prev[i] = i / arr[0];
            // Very large value if not possible
            else
                prev[i] = 1e9; 
        }

        // Fill table using bottom-up approach
        for (int ind = 1; ind < n; ind++) {
            for (int target = 0; target <= T; target++) {

                // Option 1: Not take the current element
                int notTake = prev[target];

                // Option 2: Take the current element if possible
                int take = 1e9;
                if (arr[ind] <= target)
                    take = 1 + cur[target - arr[ind]];

                // Store minimum of take and notTake
                cur[target] = min(notTake, take);
            }

            // Update previous row with current row values
            prev = cur;
        }

        // Retrieve answer from the last row
        int ans = prev[T];

        // Return -1 if target cannot be formed
        if (ans >= 1e9)
            return -1;

        // Return minimum number of elements required
        return ans;
    }
};

// Main function to test the solution
int main() {

    // Input array
    vector<int> arr = {1, 2, 3};

    // Target sum
    int T = 7;

    // Create object of Solution class
    Solution sol;

    // Call function to get result
    int result = sol.minimumElements(arr, T);

    // Output the result
    cout << "The minimum number of coins required to form the target sum is "
         << result << endl;

    // Return 0 to indicate successful execution
    return 0;
}
/*Time Complexity: O(N*T), there are total of N*T states.
Space Complexity: O(T), additonal space used to for storing rows.*/