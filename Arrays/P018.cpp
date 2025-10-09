#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    int stockBuySell(vector<int> &arr) {
        int n = arr.size();
        if (n < 2) return 0;
        int profit = 0;
        for (int i = 0; i < n - 1; ++i) {
            if (arr[i+1] > arr[i]) 
                profit += arr[i+1] - arr[i];
        }
        return profit;
    }
};
//TC-O(N)
//SC-O(1)


int main() {
    Solution obj;

    // Example test
    vector<int> arr = {63, 17, 59, 6, 8, 7, 52, 50, 20};
    
    cout << "Maximum Profit: " << obj.stockBuySell(arr) << endl;

    return 0;
}
