#include <bits/stdc++.h>
using namespace std;
///optimal
void sortArray(vector<int>& arr, int n) {

    int low = 0, mid = 0, high = n - 1; // 3 pointers

    while (mid <= high) {
        if (arr[mid] == 0) {
            swap(arr[low], arr[mid]);
            low++;
            mid++;
        }
        else if (arr[mid] == 1) {
            mid++;
        }
        else {
            swap(arr[mid], arr[high]);
            high--;
        }
    }
}
/*Time Complexity: O(N), where N = size of the given array.
Reason: We are using a single loop that can run at most N times.

Space Complexity: O(1) as we are not using any extra space.*/

////brute
void sortArray(vector<int>& arr, int n) {

    int cnt0 = 0, cnt1 = 0, cnt2 = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] == 0) cnt0++;
        else if (arr[i] == 1) cnt1++;
        else cnt2++;
    }

    //Replace the places in the original array:
    for (int i = 0; i < cnt0; i++) arr[i] = 0; // replacing 0's

    for (int i = cnt0; i < cnt0 + cnt1; i++) arr[i] = 1; // replacing 1's

    for (int i = cnt0 + cnt1; i < n; i++) arr[i] = 2; // replacing 2's

}
/*Time Complexity: O(N) + O(N), where N = size of the array. First O(N) for counting the number of 0’s, 1’s, 2’s, and second O(N) for placing them correctly in the original array.

Space Complexity: O(1) as we are not using any extra space.*/

int main()
{
    int n = 6;
    vector<int> arr = {0, 2, 1, 2, 0, 1};
    sortArray(arr, n);
    cout << "After sorting:" << endl;
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}
