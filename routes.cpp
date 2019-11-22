#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#endif /* C++ 11 */

using namespace std;
#endif /* __PROGTEST__ */

/**
 * This is a program where I trained templates and BFS for finding routes. _T means node, _E means route.
 * */

//----------------------------------------------------------------------------------------------------------------------

class NoRouteException{};

//----------------------------------------------------------------------------------------------------------------------

template <typename _T, typename _E>
class CRoute{
public:
    CRoute() = default;
    ~CRoute() = default;

    CRoute & Add(const _T & a, const _T & b, const _E & e){
        Graph[a].insert(make_pair(b, e));
        Graph[b].insert(make_pair(a, e));
        return *this;
    }

    list <_T> Find(const _T & a, const _T & b, function <bool(const _E &)> f = [](const _E &){ return true; }){ // Default filter function - always returns true.
        list <_T> Res;

        if(a == b){
            Res.push_back(a);
            return Res;
        }

        if(!BFS(a, b, Res, f))
            throw NoRouteException();
        return Res;
    }

private:
    map<_T, map<_T, _E>> Graph;

    //------------------------------------------------------------------------------------------------------------------

    bool BFS(_T src, _T dest, list <_T> & Res, function <bool(const _E &)> f){
        list <_T> q;
        map <_T, bool> v;
        map <_T, _T> prev;

        if (Graph.find(src) == Graph.cend())
            return false;

        q.push_back(src);
        v.insert(make_pair(src, true));
        prev.insert(make_pair(src, src));

        while(!q.empty()){
            _T tmp = q.front();
            q.pop_front();

            for(const auto & x : Graph.at(tmp)){
                if (v.find(x.first) == v.cend()
                && (tmp == src ? true : f(Graph.at(tmp).at(prev.at(tmp))))){
                    v.insert(make_pair(x.first, true));
                    prev.insert(make_pair(x.first, tmp));
                    q.push_back(x.first);

                    //found = build the route from the end to beginning
                    if (x.first == dest){
                        tmp = x.first;
                        while(prev.at(tmp) != tmp){
                            Res.push_front(tmp);
                            tmp = prev.at(tmp);
                        }
                        Res.push_front(tmp);
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

#ifndef __PROGTEST__

//some classes for better testing...
//----------------------------------------------------------------------------------------------------------------------

class CTrain{
public:
    string m_Company;
    int m_Speed;

    CTrain (const string & company, int speed)
        : m_Company (company), m_Speed (speed){}
};

//----------------------------------------------------------------------------------------------------------------------

class TrainFilterCompany{
    set <string> m_Companies;
public:
    TrainFilterCompany (const set<string> & companies)
        : m_Companies (companies){}

    bool operator ()(const CTrain & train) const {
        return m_Companies . find ( train . m_Company ) != m_Companies . end ();
    }
};

//----------------------------------------------------------------------------------------------------------------------

class TrainFilterSpeed{
    int m_Min;
    int m_Max;
public:
    TrainFilterSpeed (int min, int max)
        : m_Min (min), m_Max (max){}

    bool operator ()(const CTrain & train) const {
        return train . m_Speed >= m_Min && train . m_Speed <= m_Max;
    }
};

//----------------------------------------------------------------------------------------------------------------------

//custom filter function...
bool FastTrain (const CTrain    & train){
    return ( train . m_Company == "OBB" || train . m_Company == "DB" ) && train . m_Speed > 100;
}

//----------------------------------------------------------------------------------------------------------------------

static string toText (const list<string> & l){
    ostringstream oss;

    auto it = l . cbegin();
    oss << *it;
    for ( ++it; it != l . cend (); ++it )
        oss << " > " << *it;
    return oss . str ();
}

//----------------------------------------------------------------------------------------------------------------------

int main (){
    CRoute <string, CTrain> lines;

    lines .Add ("Berlin", "Prague", CTrain ("DB", 120))
          .Add ("Berlin", "Prague", CTrain ("CD",  80))
          .Add ("Berlin", "Dresden", CTrain ("DB", 160))
          .Add ("Dresden", "Munchen", CTrain ("DB", 160))
          .Add ("Munchen", "Prague", CTrain ("CD",  90))
          .Add ("Munchen", "Linz", CTrain ("DB", 200))
          .Add ("Munchen", "Linz", CTrain ("OBB", 90))
          .Add ("Linz", "Prague", CTrain ("CD", 50))
          .Add ("Prague", "Wien", CTrain ("CD", 100))
          .Add ("Linz", "Wien", CTrain ("OBB", 160))
          .Add ("Paris", "Marseille", CTrain ("SNCF", 300))
          .Add ("Paris", "Dresden",  CTrain ("SNCF", 250));

    list<string> r1 = lines.Find("Berlin", "Linz");
    assert (toText(r1) == "Berlin > Prague > Linz");
    cout << toText(r1) << endl;

    list<string> r2 = lines.Find("Linz", "Berlin");
    assert (toText(r2) == "Linz > Prague > Berlin");
    cout << toText(r2) << endl;

    list<string> r3 = lines.Find("Wien", "Berlin");
    assert (toText(r3) == "Wien > Prague > Berlin");
    cout << toText(r3) << endl;

    list<string> r4 = lines.Find("Wien", "Berlin", FastTrain );
    assert (toText(r4) == "Wien > Linz > Munchen > Dresden > Berlin");
    cout << toText(r4) << endl;

    list<string> r5 = lines.Find("Wien", "Munchen", TrainFilterCompany (set<string> { "CD", "DB" } ));
    assert (toText(r5) == "Wien > Prague > Munchen");
    cout << toText(r5) << endl;

    list<string> r6 = lines.Find("Wien", "Munchen", TrainFilterSpeed (120, 200));
    assert (toText(r6) == "Wien > Linz > Munchen");
    cout << toText(r6) << endl;

    list<string> r7 = lines.Find("Wien", "Munchen", [](const CTrain & x){ return x . m_Company == "CD"; } );
    assert (toText(r7) == "Wien > Prague > Munchen");
    cout << toText(r7) << endl;

    list<string> r8 = lines.Find("Munchen", "Munchen");
    assert (toText(r8) == "Munchen");
    cout << toText(r8) << endl;

    list<string> r9 = lines.Find("Marseille", "Prague");
    assert (toText(r9) == "Marseille > Paris > Dresden > Berlin > Prague"
         || toText(r9) == "Marseille > Paris > Dresden > Munchen > Prague");
    cout << toText(r9) << endl;

    try{
        list<string> r10 = lines.Find("Marseille", "Prague", FastTrain);
        assert ("Marseille > Prague connection does not exist!!" == NULL);
    } catch (const NoRouteException & e){}

    list<string> r11 = lines.Find("Salzburg", "Salzburg");
    assert (toText(r11) == "Salzburg");
    cout << toText(r11) << endl;

    list<string> r12 = lines.Find("Salzburg", "Salzburg", [] ( const CTrain & x ) { return x . m_Company == "SNCF"; } );
    assert (toText(r12) == "Salzburg");
    cout << toText(r12) << endl;

    try{
        list<string> r13 = lines.Find("London", "Oxford");
        assert ( "London > Oxford connection does not exist!!" == NULL);
    } catch ( const NoRouteException & e ){}
    return 0;
}
#endif  /* __PROGTEST__ */