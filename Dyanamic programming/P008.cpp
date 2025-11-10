#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // Function to find the maximum number of content students
    int findContentChildren(vector<int>& student, vector<int>& cookie) {
        // Sort both arrays to apply the greedy strategy
        sort(student.begin(), student.end());
        sort(cookie.begin(), cookie.end());

        int studentIndex = 0; 
        int cookieIndex = 0;  

        // Try to assign cookies until any one list is fully processed
        while (studentIndex < student.size() && cookieIndex < cookie.size()) {
            // If the cookie satisfies the student's greed
            if (cookie[cookieIndex] >= student[studentIndex]) {
                studentIndex++; 
            }
            // Move to next cookie in both cases
            cookieIndex++; 
        }

        // Number of students satisfied is equal to studentIndex
        return studentIndex;
    }
};

int main() {
    vector<int> student = {1, 2, 3};
    vector<int> cookie = {1, 1};

    // Create Solution object
    Solution solver;

    // Get the number of content students and print it
    int result = solver.findContentChildren(student, cookie);
    cout << "Maximum number of content students: " << result << endl;

    return 0;
}
/*
Complexity Analysis
Time Complexity: O(n*logn + m*logm), Both the arrays are sorted in increasing order.
Space Complexity: O(1), No extra space is used.
*/