#include <bits/stdc++.h>

using namespace std;


/// brute
bool isSorted(int arr[], int n) {
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      if (arr[j] < arr[i])
        return false;
    }
  }

  return true;
}
/*Time Complexity: O(N^2)

Space Complexity: O(1)*/

int main() {

  int arr[] = {1, 2, 3, 4, 5}, n = 5;
  bool ans = isSorted(arr, n);
  if (ans) cout << "True" << endl;
  else cout << "False" << endl;
  return 0;
}



// ///optimal

// bool isSorted(int arr[], int n) {
//   for (int i = 1; i < n; i++) {
//     if (arr[i] < arr[i - 1])
//       return false;
//   }

//   return true;
// }

// int main() {

//   int arr[] = {1, 2, 3, 4, 5}, n = 5;

//   printf("%s", isSorted(arr, n) ? "True" : "False");

// }

// // /*
// Time Complexity: O(N)

// Space Complexity: O(1)*/
