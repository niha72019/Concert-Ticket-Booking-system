// Test for Part 4 of main.cpp: sorting and searching
// From the project folder:
//   g++ -std=c++17 tests/test_sort_search.cpp -o tests/t4 && tests/t4
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

// true if array is in non-decreasing order of the key
bool isSorted(vector<Concert*>& a, int key) {
    for (size_t i = 1; i < a.size(); i++)
        if (SortSearch::keyValue(a[i - 1], key) > SortSearch::keyValue(a[i], key)) return false;
    return true;
}

int main() {
    ConcertBST tree;
    // id, name, price, date, sold   (several equal prices to test duplicates)
    int ids[] = {104, 102, 106, 101, 103, 105, 107};
    string names[] = {"Melody Night", "Electro Beach Fest", "Fusion Studio Night",
                      "Indie Unplugged", "Sufi Evening", "Bollywood Beats", "Rock the Fort"};
    int prices[] = {2500, 3000, 1800, 800, 1200, 1800, 2000};
    int dates[] = {20261220, 20261205, 20270110, 20261215, 20270125, 20261230, 20270214};
    int sold[] = {5, 9, 2, 6, 0, 7, 3};
    for (int i = 0; i < 7; i++) {
        Concert* c = new Concert(ids[i], names[i], "Delhi", dates[i], prices[i], 5, 8);
        for (int s = 0; s < sold[i]; s++) c->seats.booked[s / 8][s % 8] = true;
        tree.insert(c);
    }
    vector<Concert*> base;
    tree.inorderToArray(tree.root, base);

    string algo[] = {"", "bubble", "merge", "quick"};
    string keys[] = {"", "price", "date", "popularity"};
    for (int key = 1; key <= 3; key++) {
        for (int alg = 1; alg <= 3; alg++) {
            vector<Concert*> a = base;   // fresh copy each time
            if (alg == 1) SortSearch::bubbleSort(a, key);
            if (alg == 2) SortSearch::mergeSort(a, 0, a.size() - 1, key);
            if (alg == 3) SortSearch::quickSort(a, 0, a.size() - 1, key);
            check(isSorted(a, key) && a.size() == 7, algo[alg] + " sort by " + keys[key]);
        }
    }
    vector<Concert*> pop = base;
    SortSearch::quickSort(pop, 0, pop.size() - 1, 3);
    check(pop[0]->id == 102 && pop[6]->id == 103, "popularity: most sold (102) first, 0 sold (103) last");

    // already-sorted input, single element and empty array
    vector<Concert*> one(1, base[0]), none;
    SortSearch::bubbleSort(one, 1); SortSearch::mergeSort(none, 0, -1, 1); SortSearch::quickSort(none, 0, -1, 1);
    vector<Concert*> again = base;
    SortSearch::mergeSort(again, 0, 6, 1); SortSearch::quickSort(again, 0, 6, 1); SortSearch::bubbleSort(again, 1);
    check(isSorted(again, 1) && one.size() == 1, "edge cases: 1 element, empty, already sorted");

    // linear search by name (case-insensitive, partial)
    check(SortSearch::linearSearchByName(base, "NIGHT").size() == 2, "name search 'NIGHT' -> 2 matches");
    check(SortSearch::linearSearchByName(base, "rock").size() == 1, "name search 'rock' -> 1 match");
    check(SortSearch::linearSearchByName(base, "jazz").empty(), "name search 'jazz' -> none");

    // binary search on array sorted by ID (in-order of BST)
    bool allFound = true;
    for (int i = 0; i < 7; i++)
        if (SortSearch::binarySearchById(base, ids[i]) == -1) allFound = false;
    check(allFound, "binary search finds every ID");
    check(SortSearch::binarySearchById(base, 100) == -1 && SortSearch::binarySearchById(base, 108) == -1 &&
          SortSearch::binarySearchById(none, 101) == -1, "binary search: missing IDs and empty array -> -1");

    cout << (fails ? "\nSOME TESTS FAILED\n" : "\nALL TESTS PASSED\n");
    return fails;
}
