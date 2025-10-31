#include <bits/stdc++.h>
using namespace std;

int main() {

  int n=3;
  
  int prev2 = 1;
  int prev = 1;
  
  
  for(int i=2; i<=n; i++){
      int cur_i = prev2+ prev;
      prev2 = prev;
      prev= cur_i;
  }
   cout<<prev;
  return 0;
}
/*
// Time Complexity: O(N)

// Reason: We are running a simple iterative loop

// Space Complexity: O(1)

// Reason: We are not using any extra space.
// */


