#include <iostream>
#include <string>
#include <queue>
#include <set>
using namespace std;

/**
 *
 * */

int main() {
    int len, step;
    string left, right;
    queue<pair<pair<bool, int>, int>> q;
    int pos = 0, counter = 0;
    bool que = true;

    // Reading input data
    cin >> len >> step >> left >> right;

    // Starting the queue
    q.push({{que, pos}, counter});

    // BFS loop
    while (!q.empty()) {

        if ((que ? left[pos] : right[pos]) == '.' && pos + 1 > counter) {

            // Adding the neighbors to the queue
            if ((pos > 0) && ((que ? left[pos - 1] : right[pos - 1]) == '.'))
                q.push({{que, pos - 1}, counter + 1});
            if ((pos < len - 1) && ((que ? left[pos + 1] : right[pos + 1]) == '.'))
                q.push({{que, pos + 1}, counter + 1});
            if ((pos + step < len) && ((!que ? left[pos + step] : right[pos + step]) == '.'))
                q.push({{!que, pos + step}, counter + 1});

            (que ? left[pos] : right[pos]) = 'x'; // Removing visited points
        }

        // Exit condition
        if (pos + step >= len) {
            cout << counter + 1 << endl;
            return 0;
        }

        // Next element
        q.pop();
        que = q.front().first.first;
        pos = q.front().first.second;
        counter = q.front().second;
    }

    // The path was not found :(
    cout << -1 << endl;
    return 0;
}