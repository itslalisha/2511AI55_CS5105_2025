#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Helper to print result
void printAccounts(const string& title, const vector<vector<string>>& accounts) {
    cout << title << endl;
    for (const auto& acc : accounts) {
        cout << "  [";
        for (size_t i = 0; i < acc.size(); ++i) {
            cout << "\"" << acc[i] << "\"";
            if (i < acc.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

// Brute force solution
vector<vector<string>> brute(vector<vector<string>> accounts) {
    bool changes = true;
    while (changes) {
        changes = false;
        vector<bool> used(accounts.size(), false);
        vector<vector<string>> new_account;

        for (size_t i = 0; i < accounts.size(); ++i) {
            if (used[i]) {
                continue;
            }
            string name = accounts[i][0];
            set<string> emails1(accounts[i].begin() + 1, accounts[i].end());

            for (size_t j = i + 1; j < accounts.size(); ++j) {
                if (used[j]) {
                    continue;
                }
                set<string> emails2(accounts[j].begin() + 1, accounts[j].end());
                
                // Check for intersection
                bool has_intersection = false;
                for (const string& email : emails2) {
                    if (emails1.count(email)) {
                        has_intersection = true;
                        break;
                    }
                }

                if (has_intersection) {
                    emails1.insert(emails2.begin(), emails2.end()); // Union
                    used[j] = true;
                    changes = true;
                }
            }
            vector<string> merged_acc = {name};
            merged_acc.insert(merged_acc.end(), emails1.begin(), emails1.end());
            new_account.push_back(merged_acc);
            used[i] = true;
        }
        accounts = new_account;
        // The Python code has a bug: `return account` is inside the while loop.
        // It should be outside. I'll place it correctly.
    }
    return accounts; // Correct position
}

// DSU for strings
class DSU {
    unordered_map<string, string> parent;
public:
    string find(string x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
        }
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    void unite(string x, string y) {
        parent[find(x)] = find(y);
    }
};

// Optimal solution
vector<vector<string>> optimal(const vector<vector<string>>& accounts) {
    DSU dsu;
    unordered_map<string, string> email_to_name;

    for (const auto& account : accounts) {
        string name = account[0];
        string first_email = account[1];
        for (size_t i = 1; i < account.size(); ++i) {
            dsu.unite(first_email, account[i]);
            email_to_name[account[i]] = name;
        }
    }

    map<string, set<string>> groups; // map root_email -> set_of_emails
    for (auto const& [email, name] : email_to_name) {
        string root = dsu.find(email);
        groups[root].insert(email);
    }

    vector<vector<string>> result;
    for (auto const& [root, emails] : groups) {
        vector<string> account = {email_to_name[root]};
        account.insert(account.end(), emails.begin(), emails.end());
        result.push_back(account);
    }
    return result;
}

int main() {
    vector<vector<string>> accounts = {
      {"John", "johnsmith@mail.com", "john00@mail.com"},
      {"John", "johnnybravo@mail.com"},
      {"John", "johnsmith@mail.com", "john_newyork@mail.com"},
      {"Mary", "mary@mail.com"}
    };

    // Need copies as 'brute' modifies its input
    vector<vector<string>> accounts_brute = accounts;
    vector<vector<string>> accounts_optimal = accounts;

    printAccounts("Brute Force:", brute(accounts_brute));
    printAccounts("Optimal:", optimal(accounts_optimal));
    return 0;
}