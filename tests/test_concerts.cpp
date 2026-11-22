// Test for Part 3 of main.cpp: concerts and BST
// From the project folder:
//   g++ -std=c++17 tests/test_concerts.cpp -o tests/t3 && tests/t3
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

int main() {
    check(formatDate(20270105) == "05-01-2027", "formatDate");

    ConcertBST tree;
    check(tree.search(101) == nullptr, "search in empty tree");
    int ids[] = {104, 102, 106, 101, 103, 105, 107};   // inserted out of order
    for (int i = 0; i < 7; i++)
        tree.insert(new Concert(ids[i], "Show", "Delhi", 20261201 + i, 1000 + i, 2, 3));
    check(tree.count == 7 && tree.root->data->id == 104, "7 inserted, root is 104");
    check(tree.root->left->data->id == 102 && tree.root->right->data->id == 106,
          "102 goes left, 106 goes right");
    check(tree.search(105) != nullptr && tree.search(105)->id == 105, "search finds 105 (leaf)");
    check(tree.search(999) == nullptr, "search 999 -> not found");

    Concert* dup = new Concert(103, "Dup", "Agra", 20261201, 1, 1, 1);
    check(!tree.insert(dup) && tree.count == 7, "duplicate ID rejected");
    delete dup;   // not in tree, so the caller frees it

    vector<Concert*> arr;
    tree.inorderToArray(tree.root, arr);
    bool sorted = arr.size() == 7;
    for (size_t i = 1; i < arr.size(); i++) if (arr[i - 1]->id >= arr[i]->id) sorted = false;
    check(sorted, "in-order gives IDs in increasing order");

    Concert* c = tree.search(101);
    for (int r = 0; r < 2; r++) for (int s = 0; s < 3; s++) c->seats.booked[r][s] = true;
    c->waitList.enqueue(4);
    check(c->ticketsSold() == 6 && c->seats.isFull(), "popularity = tickets sold");

    Concert::printHeader();
    tree.inorderDisplay(tree.root);

    cout << (fails ? "\nSOME TESTS FAILED\n" : "\nALL TESTS PASSED\n");
    return fails;
}
