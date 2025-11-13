#include <iostream>
#include <vector>

using namespace std;

// Global variable for reverse pairs count
long long c = 0;

void merge(vector<int>& nums, int l, int mid, int h) {
    int n1 = mid - l + 1;
    int n2 = h - mid;

    vector<int> arr1(n1);
    vector<int> arr2(n2);

    for (int i = 0; i < n1; ++i) {
        arr1[i] = nums[l + i];
    }
    for (int i = 0; i < n2; ++i) {
        arr2[i] = nums[mid + 1 + i];
    }

    int i = 0, j = 0;

    // Count reverse pairs
    while (i < n1 && j < n2) {
        // Use 2LL * to cast to long long before comparison
        if (arr1[i] > 2LL * arr2[j]) {
            c += (n1 - i);
            j++;
        } else {
            i++;
        }
    }

    // Reset pointers for standard merge
    i = 0;
    j = 0;
    int k = l;

    while (i < n1 && j < n2) {
        if (arr1[i] <= arr2[j]) {
            nums[k] = arr1[i];
            i++;
        } else {
            nums[k] = arr2[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        nums[k] = arr1[i];
        i++;
        k++;
    }
    while (j < n2) {
        nums[k] = arr2[j];
        j++;
        k++;
    }
}

void mergesort(vector<int>& nums, int l, int h) {
    if (l < h) {
        int mid = (l + h) / 2;
        mergesort(nums, l, mid);
        mergesort(nums, mid + 1, h);
        merge(nums, l, mid, h);
    }
}

int main() {
    vector<int> nums = {6, 4, 1, 2, 7};
    mergesort(nums, 0, 4);
    // print("Brute:", brute(nums))
    cout << "Optimal: " << c << endl;
    return 0;
}