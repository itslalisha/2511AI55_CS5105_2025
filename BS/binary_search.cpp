#include <bits/stdc++.h>
using namespace std;

// -------- Standard Binary Search (any occurrence) ----------
int binarysearch(vector<int> &nums, int target) {
    int low = 0;
    int high = (int)nums.size() - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (nums[mid] == target)
            return mid;  // found one occurrence
        else if (nums[mid] < target)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;  // not found
}

// -------- Leftmost Binary Search (first occurrence) ----------
int binarysearch_leftmost(vector<int> &nums, int target) {
    int low = 0, high = (int)nums.size() - 1, ans = -1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (nums[mid] == target) {
            ans = mid;
            high = mid - 1; // move left
        } else if (nums[mid] < target)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return ans;
}

/*TC-O(logn)
SC-O(1)*/

int main() {
    int n, target;
    cout << "Enter size of array: ";
    cin >> n;

    vector<int> nums(n);
    cout << "Enter " << n << " elements (sorted): ";
    for (int i = 0; i < n; i++)
        cin >> nums[i];

    cout << "Enter target value: ";
    cin >> target;

    int result = binarysearch(nums, target);
    cout << "\nAny occurrence index (binarysearch): " << result << endl;

    int leftmost = binarysearch_leftmost(nums, target);
    cout << "Leftmost occurrence index: " << leftmost << endl;

    return 0;
}
