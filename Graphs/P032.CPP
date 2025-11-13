#include <iostream>
#include <vector>
#include <algorithm> // For std::sort, std::min, std::max

using namespace std;

using Interval = vector<int>;

// Helper to print intervals
void printIntervals(const string& title, const vector<Interval>& intervals) {
    cout << title << "[";
    for (size_t i = 0; i < intervals.size(); ++i) {
        cout << "[" << intervals[i][0] << ", " << intervals[i][1] << "]";
        if (i < intervals.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

// --- Brute Force ---
vector<Interval> brute(vector<Interval>& intervals) {
    int n = intervals.size();
    vector<bool> merged(n, false);
    vector<Interval> res;
    
    // Sort to make the brute force logic a bit more stable
    // (Python's set logic doesn't care, but this helps)
    sort(intervals.begin(), intervals.end());

    for (int i = 0; i < n; ++i) {
        if (merged[i]) {
            continue;
        }
        int curr_start = intervals[i][0];
        int curr_end = intervals[i][1];

        for (int j = i + 1; j < n; ++j) {
            if (merged[j]) {
                continue;
            }
            int s = intervals[j][0];
            int e = intervals[j][1];
            
            // Check for overlap
            if (!(s > curr_end || e < curr_start)) {
                curr_start = min(curr_start, s);
                curr_end = max(curr_end, e);
                merged[j] = true;
            }
        }
        res.push_back({curr_start, curr_end});
    }
    return res;
}

// --- Optimal ---
vector<Interval> optimal(vector<Interval>& intervals) {
    if (intervals.empty()) {
        return {};
    }
    
    // Sort by start time
    sort(intervals.begin(), intervals.end());

    vector<Interval> res;
    res.push_back(intervals[0]);

    for (size_t i = 1; i < intervals.size(); ++i) {
        int start = intervals[i][0];
        int end = intervals[i][1];
        
        // If current interval overlaps with the last one in result
        if (start <= res.back()[1]) {
            res.back()[1] = max(end, res.back()[1]);
        } else {
            res.push_back(intervals[i]);
        }
    }
    return res;
}

int main() {
    vector<Interval> intervals = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    // Create a copy for 'brute'
    vector<Interval> intervals_brute = intervals;

    printIntervals("Brute :", brute(intervals_brute));
    printIntervals("Optimal :", optimal(intervals));

    return 0;
}