#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

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

// Brute force solution using a map to count frequencies
vector<int> brute(const vector<int>& nums) {
    int n = nums.size();
    map<int, int> freq;
    for (int num : nums) {
        freq[num]++;
    }
    int count;

    vector<int> result;
    // for (auto const& [nums, count] : freq) {
    //     if (count > n / 3) {
    //         result.push_back(nums);
    //     }
    // }
    return result;
}

// Optimal solution (Boyer-Moore Voting Algorithm variant)
vector<int> optimal(const vector<int>& nums) {
    int cand1 = 0, cand2 = 0; // Using 0 as a placeholder, will be set
    int count1 = 0, count2 = 0;
    int n = nums.size();
    bool cand1_set = false, cand2_set = false;

    for (int num : nums) {
        if (cand1_set && cand1 == num) {
            count1++;
        } else if (cand2_set && cand2 == num) {
            count2++;
        } else if (count1 == 0) {
            count1 = 1;
            cand1 = num;
            cand1_set = true;
        } else if (count2 == 0) {
            count2 = 1;
            cand2 = num;
            cand2_set = true;
        } else {
            count1--;
            count2--;
        }
    }

    count1 = 0;
    count2 = 0;
    for (int num : nums) {
        if (cand1_set && cand1 == num) {
            count1++;
        } else if (cand2_set && cand2 == num) {
            count2++;
        }
    }

    vector<int> res;
    if (cand1_set && count1 > n / 3) {
        res.push_back(cand1);
    }
    if (cand2_set && count2 > n / 3 && cand1 != cand2) {
        res.push_back(cand2);
    }
    
    // Ensure unique elements if cand1 and cand2 were the same but passed threshold
    if (res.size() == 2 && res[0] == res[1]) {
        res.pop_back();
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

    cout << "Brute:   ";
    printVector(brute(nums));
    
    cout << "Optimal: ";
    printVector(optimal(nums));

    return 0;
}