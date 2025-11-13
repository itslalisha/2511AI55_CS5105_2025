#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

int optimal(const vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    int prefixXor = 0;
    int count = 0;

    for (int num : nums) {
        prefixXor ^= num;
        if (prefixXor == k) {
            count++;
        }
        if (freq.count(prefixXor ^ k)) {
            count += freq[prefixXor ^ k];
        }
        freq[prefixXor]++;
    }
    return count;
}

int main() {
    vector<int> nums = {4, 2, 2, 6, 4};
    int k = 6;
    cout << optimal(nums, k) << endl;
    return 0;
}