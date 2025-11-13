#include <iostream>
#include <vector>
#include <numeric> // For std::accumulate

using namespace std;

vector<long long> optimal(const vector<int>& nums) {
    long long n = nums.size();
    
    // Sum of first n numbers: S_n
    long long s1 = n * (n + 1) / 2;
    // Sum of squares of first n numbers: S_n^2
    long long q1 = n * (n + 1) * (2 * n + 1) / 6;

    long long s = 0;
    long long q = 0;

    for (int num : nums) {
        s += (long long)num;
        q += (long long)num * (long long)num;
    }
    
    // s - s1 = A - B
    long long d1 = s - s1;
    // q - q1 = A^2 - B^2 = (A - B)(A + B)
    long long d2 = q - q1;
    
    // (A + B) = (A^2 - B^2) / (A - B)
    long long d = d2 / d1; // A + B

    // A = ((A - B) + (A + B)) / 2
    long long A = (d1 + d) / 2;
    // B = A - (A - B)
    long long B = A - d1;

    return {A, B}; // {Repeating, Missing}
}

int main() {
    vector<int> nums = {1, 2, 2, 4};
    vector<long long> result = optimal(nums);
    cout << "[" << result[0] << ", " << result[1] << "]" << endl;
    return 0;
}