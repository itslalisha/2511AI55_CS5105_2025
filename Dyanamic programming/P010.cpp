#include <bits/stdc++.h>
using namespace std;
#define MOD 1000000007

int countTargetSumWays(vector<int>& A, int target) {
    int totalSum = accumulate(A.begin(), A.end(), 0);

    // If target + totalSum is odd or target > totalSum, not possible
    if ((target + totalSum) % 2 != 0 || abs(target) > totalSum)
        return 0;

    int S1 = (target + totalSum) / 2;

    vector<int> dp(S1 + 1, 0);
    dp[0] = 1; // one way to make sum 0

    for (int num : A) {
        for (int s = S1; s >= num; --s) {
            dp[s] = (dp[s] + dp[s - num]) % MOD;
        }
    }

    return dp[S1];
}

int main() {
    int N, target;
    cin >> N >> target;
    vector<int> A(N);
    for (int i = 0; i < N; ++i) cin >> A[i];

    cout << countTargetSumWays(A, target) << endl;
    return 0;
}
/*
TC-(N*sum(A))
SC-O(N)*/