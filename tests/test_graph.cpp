// Test for Part 5 of main.cpp: city graph (BFS, DFS, Dijkstra, fewest stops), same map as main.cpp
// From the project folder:
//   g++ -std=c++17 tests/test_graph.cpp -o tests/t5 && tests/t5
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
// Rename the program main() so this test can have its own main().
#define main program_main
#include "../main.cpp"
#undef main

int fails = 0;
void check(bool ok, string what) {
    cout << (ok ? "PASS " : "FAIL ") << what << "\n";
    if (!ok) fails++;
}

// Runs one graph function and returns what it printed.
stringstream captured;
streambuf* realCout;
void startCapture() { captured.str(""); realCout = cout.rdbuf(captured.rdbuf()); }
string stopCapture() { cout.rdbuf(realCout); return captured.str(); }

int main() {
    CityGraph g;
    string names[] = {"Delhi", "Jaipur", "Agra", "Lucknow", "Chandigarh", "Mumbai", "Pune", "Bengaluru"};
    for (int i = 0; i < 8; i++) g.addCity(names[i]);
    g.addRoad(0, 1, 280);  g.addRoad(0, 2, 230);  g.addRoad(0, 4, 245);
    g.addRoad(0, 3, 600);  g.addRoad(1, 2, 240);  g.addRoad(2, 3, 335);
    g.addRoad(1, 5, 1150); g.addRoad(5, 6, 150);  g.addRoad(6, 7, 840);
    g.addRoad(5, 7, 1000);
    check(g.numCities == 8 && g.road[3][2] == 335, "8 cities, roads are two-way");

    startCapture();
    int d = g.shortestPath(0, 3);
    string out = stopCapture();
    check(d == 565 && out.find("Delhi -> Agra -> Lucknow") != string::npos,
          "Delhi->Lucknow: 565 km via Agra beats direct 600 km road");

    startCapture();
    d = g.shortestPath(0, 7);
    out = stopCapture();
    check(d == 2420 && out.find("Delhi -> Jaipur -> Mumbai -> Pune -> Bengaluru") != string::npos,
          "Delhi->Bengaluru: 2420 km via Pune");

    startCapture();
    d = g.shortestPath(4, 4);
    stopCapture();
    check(d == 0, "same source and destination -> 0 km");

    startCapture();
    g.BFS(0);
    out = stopCapture();
    check(out.find("Delhi  (0") < out.find("Jaipur  (1") &&
          out.find("Chandigarh  (1") < out.find("Mumbai  (2") &&
          out.find("Mumbai  (2") < out.find("Bengaluru  (3"),
          "BFS visits level by level (0, 1, 2, 3 roads away)");

    startCapture();
    g.DFS(0);
    out = stopCapture();
    check(out.find("Delhi  Jaipur  Agra  Lucknow  Mumbai  Pune  Bengaluru  Chandigarh") != string::npos,
          "DFS goes deep first: Delhi Jaipur Agra Lucknow Mumbai Pune Bengaluru Chandigarh");

    startCapture();
    int s = g.fewestStops(0, 3);
    out = stopCapture();
    check(s == 1 && out.find("Delhi -> Lucknow") != string::npos && out.find("600 km") != string::npos,
          "Fewest stops Delhi->Lucknow: direct road, 1 stop, 600 km");

    startCapture();
    s = g.fewestStops(0, 7);
    out = stopCapture();
    check(s == 3 && out.find("Delhi -> Jaipur -> Mumbai -> Bengaluru") != string::npos &&
          out.find("2430 km") != string::npos,
          "Fewest stops Delhi->Bengaluru: 3 stops via Mumbai, 2430 km");

    // A city with no roads cannot be reached.
    CityGraph g2;
    g2.addCity("A"); g2.addCity("B"); g2.addCity("C");
    g2.addRoad(0, 1, 10);
    startCapture();
    d = g2.shortestPath(0, 2);
    out = stopCapture();
    check(d == -1 && out.find("No route") != string::npos, "unreachable city -> 'No route'");
    startCapture();
    s = g2.fewestStops(0, 2);
    out = stopCapture();
    check(s == -1 && out.find("No route") != string::npos, "fewest stops, unreachable city -> 'No route'");

    g.display();
    g.BFS(0);
    g.DFS(0);
    g.shortestPath(0, 7);

    cout << (fails ? "\nSOME TESTS FAILED\n" : "\nALL TESTS PASSED\n");
    return fails;
}
