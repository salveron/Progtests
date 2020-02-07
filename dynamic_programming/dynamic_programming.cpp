#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <list>
#include <limits>
using namespace std;

//NOT_FINISHED!

//----------------------------------------------------------------------------------------------------------------------

bool readInput(int & n, int & k, int & first, map <int, int> & beers, map <int, pair <int, set <int>>> & restaurants){
    cin >> n >> k;

    for (int i = 1; i < k + 1; i++)
        cin >> beers[i];
    if ((int)beers.size() != k) return false;

    for (int i = 1; i < n + 1; i++){
        cin >> restaurants[i].first;
    }
    if ((int)restaurants.size() != n) return false;

    for (int i = 1; i < n; i++){
        int x, y;
        cin >> x >> y;
        if (i == 1) first = x;
        restaurants[x].second.insert(y);
        restaurants[y].second.insert(x);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------

int findMinSumOfPrices(map <int, pair <int, set <int>>> & restaurants, map <int, int> & beers,
                       int vertex, int restricted, set <int> & visited, map <int, pair <int, int>> & memory){
    int minSum = numeric_limits<int>::max();
    int sum = 0;

    visited.insert(vertex);

    for (auto & beersIt : beers){
        if (beersIt.first != restricted) {
            sum = beersIt.second * restaurants.at(vertex).first;
            for (int vertexIt : restaurants.at(vertex).second) {
                if (visited.find(vertexIt) == visited.cend()) {
                    if (memory.find(vertexIt) != memory.end() && memory[vertexIt].second != beersIt.first){
                        sum += memory[vertexIt].first;
                    } else
                        sum += findMinSumOfPrices(restaurants, beers, vertexIt, beersIt.first, visited, memory);
                }
            }
            if (sum < minSum){
                minSum = sum;
            }
        }
    }

    visited.erase(vertex);
    memory[vertex] = make_pair(minSum, restricted);
    return minSum;
}

//----------------------------------------------------------------------------------------------------------------------

void restoreBeerIndexes(map <int, pair <int, set <int>>> & restaurants, map <int, int> & beers,
                        set <int> & visited, int vertex, map <int, pair <int, int>> & memory, map <int, int> & result){
    visited.insert(vertex);
    int sum = memory[vertex].first;
    for (auto & it : restaurants.at(vertex).second){
        if (visited.find(it) == visited.end()){
            restoreBeerIndexes(restaurants, beers, visited, it, memory, result);
            sum -= memory[it].first;
        }
    }
    sum /= restaurants[vertex].first;
    for (auto & it : beers)
        if (it.second == sum)
            result[vertex] = it.first;
}

//----------------------------------------------------------------------------------------------------------------------

void printMaps(int & n, int & k, map <int, int> & beers, map <int, pair <int, set <int>>> & restaurants){
    cout << "----------------------------------------------------------------------------------------------------------\n";
    cout << "Restos: " << n << ", beers: " << k << endl;
    cout << endl;
    for (auto & it : beers)
        cout << it.first << " (" << it.second << "$);" << endl;
    cout << endl;
    for (auto & it : restaurants) {
        cout << it.first << " (" << it.second.first << ") -> ";
        for (auto & iter : it.second.second)
            cout << iter << ", ";
        cout << endl;
    }
    cout << "----------------------------------------------------------------------------------------------------------\n";
}

//----------------------------------------------------------------------------------------------------------------------

int main() {
    int n, k;
    map <int, int> beers;
    map <int, pair <int, set <int>>> restaurants;
    set <int> visited = set <int> ();
    map <int, pair <int, int>> memory = map <int, pair <int, int>> ();
    map <int, int> result = map <int, int> ();
    int first;

    if (!readInput(n, k, first, beers, restaurants))
        return 1;

    //printMaps(n, k, beers, restaurants);

    cout << findMinSumOfPrices(restaurants, beers, first, -1, visited, memory) << endl;

    visited.clear();
    restoreBeerIndexes(restaurants, beers, visited, first, memory, result);

    for (auto & it : memory)
        cout << it.first << "(" << it.second.first << "$) ";
    cout << endl;

    for (int i = 1; i < n; i++)
        cout << result[i] << " ";
    cout << result[n] << endl;

    return 0;
}