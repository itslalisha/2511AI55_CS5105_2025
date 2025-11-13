#include <iostream>
#include <vector>

using namespace std;

// Helper function to print a vector
void printVector(const vector<int>& vec) {
    cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

vector<int> merge(const vector<int>& nums1, const vector<int>& nums2) {
    int n1 = nums1.size();
    int n2 = nums2.size();
    vector<int> res;
    int i = 0, j = 0;

    while (i < n1 && j < n2) {
        if (nums1[i] > nums2[j]) {
            res.push_back(nums2[j]);
            j++;
        } else {
            res.push_back(nums1[i]);
            i++;
        }
    }
    while (i < n1) {
        res.push_back(nums1[i]);
        i++;
    }
    while (j < n2) {
        res.push_back(nums2[j]);
        j++;
    }
    return res;
}

int main() {
    vector<int> nums1 = {1, 2, 3};
    // int m = 3; // Not used in the Python function
    vector<int> nums2 = {2, 5, 6};
    // int n = 3; // Not used in the Python function

    vector<int> res = merge(nums1, nums2);
    printVector(res);
    return 0;
}