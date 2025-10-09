#include <bits/stdc++.h>
using namespace std;

class Solution {
  public:
    void rearrange(vector<int> &arr) {
        int j=0,k=0;
        int n = arr.size();
        vector<int> positive;
        vector<int> negative;
        
        for(int i=0;i<n;i++) {
            if(arr[i]>=0)
                positive.push_back(arr[i]);
            else
                negative.push_back(arr[i]);
        }
        
        int i=0;
        while(i<n) {
            if(j<positive.size()) {
                arr[i++] = positive[j++];
            }
            if(k<negative.size()) {
                arr[i++] = negative[k++];
            }
        }
    }
};
//TC-O(n)
//SC-O(n)

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;
    
    vector<int> arr(n);
    cout << "Enter elements: ";
    for(int i=0;i<n;i++) {
        cin >> arr[i];
    }

    Solution obj;
    obj.rearrange(arr);

    cout << "Rearranged array: ";
    for(int i=0;i<n;i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}
