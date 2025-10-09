#include<bits/stdc++.h>
using namespace std;

////brute
string read(int n,vector<int>& arr, int target){
 int n= arr.size();
    for(int i=0; i<n; i++){
        for( int j=i; j<n; j++){
            if(arr[i]+arr[j]==target&&i!=j)
            return "YES";
            
        }
    }
    return "NO";
      }
/*Time Complexity: O(N2), where N = size of the array.
Reason: There are two loops(i.e. nested) each running for approximately N times.

Space Complexity: O(1) as we are not using any extra space.*/


////better
class Solution{
      string read(int n,vector<int>& arr, int target){
        map<int,int>mpp;
        for(int i=0; i<n; i++){
            int a=arr[i];
            int more=target-a;
            if(mpp.find(more)!=mpp.end()){
                return "YES";
            }
            mpp[a]=i;
        }
        return "NO";
      }
};
///TC-O(n)//best case   O(n^2)if we use unordered map  and O(nlogn) if we use map intead of uordered map
///SC-O(n)
    
////optimal
string read(int n,vector<int> &arr, int target){
sort(arr.begin(), arr.end());
    int left = 0, right = n - 1;
    while (left < right) {
        int sum = arr[left] + arr[right];
        if (sum == target) {
            return "YES";
        }
        else if (sum < target) left++;
        else right--;
    }
    return "NO";
}

/*Time Complexity: O(N) + O(N*logN), where N = size of the array.
Reason: The loop will run at most N times. And sorting the array will take N*logN time complexity.

Space Complexity: O(1) as we are not using any extra space.*/

 int main(){
   int n = 5;
    vector<int> arr = {2, 6, 5, 8, 11};
    int target = 14;
    string ans = read(n, arr, target);
    cout << "This is the answer for variant 1: " << ans << endl;
    return 0;
 }