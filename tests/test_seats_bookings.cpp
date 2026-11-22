// Test for Part 1 of main.cpp: seat map (2D array) and bookings (linked list)
// From the project folder:
//   g++ -std=c++17 tests/test_seats_bookings.cpp -o tests/t1 && tests/t1
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
    // ---- 2D array ----
    SeatMap m(2, 3);
    check(m.isFree(0, 0) && m.countBooked() == 0, "new seat map is empty");
    check(!m.isFree(2, 0) && !m.isFree(0, 3) && !m.isFree(-1, 0), "out-of-range seats are not free");
    m.booked[0][0] = true;
    check(!m.isFree(0, 0), "booked seat is not free");
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 3; j++) m.booked[i][j] = true;
    check(m.isFull() && m.countBooked() == 6, "map full after booking all 6");
    m.booked[1][2] = false;
    check(!m.isFull() && m.isFree(1, 2), "freeing a seat makes it available");
    check(seatLabel(1, 2) == "R2-S3", "seat label R2-S3");
    m.display();

    // ---- linked list ----
    BookingList list;
    check(list.head == nullptr && list.findById(1) == nullptr, "empty list");
    list.insertEnd(Booking(1001, 1, 101, "Melody Night", 0, 0));
    list.insertEnd(Booking(1002, 2, 101, "Melody Night", 0, 1));
    list.insertEnd(Booking(1003, 1, 102, "Rock the Fort", 1, 1));
    check(list.count == 3 && list.head->data.bookingId == 1001 && list.tail->data.bookingId == 1003,
          "insertEnd keeps order, tail correct");
    Booking* b = list.findById(1002);
    check(b != nullptr && b->userId == 2, "findById finds middle node");
    check(list.findById(9999) == nullptr, "findById unknown -> nullptr");
    b->status = "Cancelled";
    check(list.findById(1002)->status == "Cancelled", "status change stays in the list");

    cout << "\nHistory of user 1:\n";
    check(list.displayByUser(1) == 2, "user 1 has 2 bookings");
    check(list.displayByUser(5) == 0, "user 5 has none");

    cout << (fails ? "\nSOME TESTS FAILED\n" : "\nALL TESTS PASSED\n");
    return fails;
}
