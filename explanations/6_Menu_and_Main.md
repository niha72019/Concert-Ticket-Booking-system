# Part 6: Menu and main()

Lines 621 to 976 of `main.cpp`.

## What this part does and why

This part connects all the data structures into one working program. It contains:
- the **global data** of the system: one BST, one booking list, one stack, one graph and
  the 5 users
- **input helpers** that validate everything the user types
- one function per **menu option**
- the **sample data** loaded at startup
- `main()` with the menu loop

Each menu option is its own small function, so every feature can be explained separately.

## Global data

```cpp
621 | // Part 6: Menu and main()
622 |
623 | ConcertBST concerts;
624 | BookingList bookings;
625 | CancelStack cancelled;
626 | CityGraph cities;
627 |
628 | const int NUM_USERS = 5;
629 | User users[NUM_USERS] = {{1, "Aarav"}, {2, "Diya"}, {3, "Kabir"}, {4, "Meera"}, {5, "Rohan"}};
630 |
631 | int nextBookingId = 1001;
```

| Variable | Data structure | Purpose |
|---|---|---|
| `concerts` | BST (Part 3) | all concerts, key = ID |
| `bookings` | linked list (Part 1) | every booking ever made |
| `cancelled` | stack (Part 2) | cancelled booking IDs for undo |
| `cities` | graph (Part 5) | cities and roads |
| `users` | plain array of 5 | fixed users; no registration |
| `nextBookingId` | int | the next booking number (starts at 1001) |

## Input helpers

```cpp
633 | // Reads a whole number in [low, high], asking again after wrong input. O(1) per try
634 | int readInt(string prompt, int low, int high) {
635 |     int x;
636 |     while (true) {
637 |         cout << prompt;
638 |         cin >> x;
639 |         if (cin.fail()) {
640 |             if (cin.eof()) { cout << "\nInput ended. Goodbye!\n"; exit(0); }
641 |             cin.clear();
642 |             cin.ignore(10000, '\n');
643 |             cout << "  Invalid input! Please enter a number.\n";
644 |         } else {
645 |             cin.ignore(10000, '\n');
646 |             if (x >= low && x <= high) return x;
647 |             cout << "  Please enter a number between " << low << " and " << high << ".\n";
648 |         }
649 |     }
650 | }
651 |
652 | // Reads a line of text that is not empty. O(length)
653 | string readText(string prompt) {
654 |     string s;
655 |     while (true) {
656 |         cout << prompt;
657 |         if (!getline(cin, s)) { cout << "\nInput ended. Goodbye!\n"; exit(0); }
658 |         if (s != "") return s;
659 |         cout << "  Input cannot be empty.\n";
660 |     }
661 | }
```

**`readInt(prompt, low, high)`**
1. Print the prompt and try `cin >> x`.
2. If the input is not a number (for example "abc"), `cin.fail()` is true. Then:
   - `cin.clear()` removes the error flag.
   - `cin.ignore(...)` throws away the wrong line.
   - A message is printed, and the loop asks again.
3. If input has ended (`cin.eof()`), say goodbye and exit, so the program never loops
   forever.
4. If it is a number, remove the rest of the line. Then return it if it is in range;
   otherwise print the allowed range and ask again.

**`readText(prompt)`**: reads a full line with `getline`, so names with spaces work. It
refuses an empty line.

## Small helper functions

```cpp
663 | // Linear search for a user ID in the users array, -1 if not found. O(n)
664 | int findUser(int id) {
665 |     for (int i = 0; i < NUM_USERS; i++)
666 |         if (users[i].userId == id) return i;
667 |     return -1;
668 | }
669 |
670 | // Shows the users, reads a user ID and returns its index or -1. O(n)
671 | int askUser() {
672 |     cout << "  Users: ";
673 |     for (int i = 0; i < NUM_USERS; i++) cout << users[i].userId << "-" << users[i].name << "  ";
674 |     int idx = findUser(readInt("\n  Enter User ID: ", -1000000, 1000000));
675 |     if (idx == -1) cout << "  User not found.\n";
676 |     return idx;
677 | }
678 |
679 | // Reads a concert ID and searches the BST, nullptr if not found. O(h)
680 | Concert* askConcert() {
681 |     Concert* c = concerts.search(readInt("  Enter Concert ID: ", -1000000, 1000000));
682 |     if (c == nullptr) cout << "  Concert not found.\n";
683 |     return c;
684 | }
685 |
686 | // Returns all concerts in an array sorted by ID (BST in-order). O(n)
687 | vector<Concert*> concertArray() {
688 |     vector<Concert*> arr;
689 |     concerts.inorderToArray(concerts.root, arr);
690 |     return arr;
691 | }
692 |
693 | // Prints a list of concerts as a table. O(n)
694 | void printConcertList(vector<Concert*>& arr) {
695 |     Concert::printHeader();
696 |     for (int i = 0; i < (int)arr.size(); i++) arr[i]->printRow();
697 | }
698 |
699 | // Marks the seat booked and adds a new booking to the linked list. O(1)
700 | int makeBooking(int userId, Concert* con, int r, int c) {
701 |     con->seats.booked[r][c] = true;
702 |     bookings.insertEnd(Booking(nextBookingId, userId, con->id, con->name, r, c));
703 |     return nextBookingId++;
704 | }
```

- **`findUser(id)`**: a linear search through the 5 users. Returns the index, or -1.
- **`askUser()`**: prints the users, reads an ID and checks that it exists.
- **`askConcert()`**: reads an ID and searches the BST. Prints "Concert not found." if it is missing.
- **`concertArray()`**: in-order traversal into a vector, so the result is sorted by ID.
- **`printConcertList(arr)`**: prints the header, then each concert.
- **`makeBooking(...)`**: the single place where a booking is created.
  1. Mark the seat as booked in the 2D array.
  2. Add a new `Booking` to the end of the linked list.
  3. Return the booking ID, and increase `nextBookingId`.

  It is used by Book Ticket, by the waiting-list auto-allocation and by the sample data.

## Menu 1 and 2: View and Search

```cpp
706 | // Menu 1: shows all concerts in ID order, then one seat map if asked. O(n)
707 | void viewConcerts() {
708 |     cout << "\n  ALL CONCERTS (in-order traversal of BST = sorted by ID)\n";
709 |     Concert::printHeader();
710 |     concerts.inorderDisplay(concerts.root);
711 |     int id = readInt("\n  Enter a Concert ID to see its seat map (0 = back): ", -1000000, 1000000);
712 |     if (id == 0) return;
713 |     Concert* c = concerts.search(id);
714 |     if (c == nullptr) cout << "  Concert not found.\n";
715 |     else c->seats.display();
716 | }
717 |
718 | // Menu 2: search by ID (BST), by name (linear) or by ID (binary search).
719 | void searchConcert() {
720 |     cout << "\n  1. By ID   (BST search)\n  2. By name (linear search)\n"
721 |          << "  3. By ID   (binary search on sorted array)\n";
722 |     int ch = readInt("  Choice: ", 1, 3);
723 |     vector<Concert*> arr = concertArray();
724 |
725 |     if (ch == 1) {
726 |         Concert* c = askConcert();
727 |         if (c) { Concert::printHeader(); c->printRow(); }
728 |     } else if (ch == 2) {
729 |         string text = readText("  Enter name (or part of it): ");
730 |         vector<Concert*> found = SortSearch::linearSearchByName(arr, text);
731 |         if (found.empty()) cout << "  Concert not found.\n";
732 |         else printConcertList(found);
733 |     } else {
734 |         int id = readInt("  Enter Concert ID: ", -1000000, 1000000);
735 |         int pos = SortSearch::binarySearchById(arr, id);
736 |         if (pos == -1) cout << "  Concert not found.\n";
737 |         else {
738 |             cout << "  Found at index " << pos << " of the sorted array.\n";
739 |             Concert::printHeader();
740 |             arr[pos]->printRow();
741 |         }
742 |     }
743 | }
```

**`viewConcerts()`**
1. Print all concerts with the BST in-order traversal.
2. Ask for an ID to show its seat map. 0 goes back to the menu.

**`searchConcert()`** offers three ways:
1. **By ID with BST search**: `askConcert()`.
2. **By name with linear search**: read text and call `linearSearchByName`. All matches are printed.
3. **By ID with binary search**: binary search on `concertArray()`, which is sorted by ID.
   It also prints the index where the concert was found.

## Menu 3: Add Concert

```cpp
745 | // True if the day and month of a YYYYMMDD date exist. O(1)
746 | bool validDate(int d) {
747 |     int day = d % 100, month = (d / 100) % 100;
748 |     int daysIn[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
749 |     return month >= 1 && month <= 12 && day >= 1 && day <= daysIn[month];
750 | }
751 |
752 | // Menu 3: reads and checks a new concert, then inserts it into the BST. O(h)
753 | void addConcert() {
754 |     int id = readInt("  New Concert ID (1-9999): ", 1, 9999);
755 |     if (concerts.search(id) != nullptr) {
756 |         cout << "  A concert with ID " << id << " already exists.\n";
757 |         return;
758 |     }
759 |     string name = readText("  Concert name: ");
760 |     cities.display();
761 |     int city = readInt("  Choose city number: ", 1, cities.numCities) - 1;
762 |
763 |     int date = readInt("  Date (YYYYMMDD, e.g. 20261225): ", 20260101, 20301231);
764 |     while (!validDate(date)) {
765 |         cout << "  That date does not exist.\n";
766 |         date = readInt("  Date (YYYYMMDD): ", 20260101, 20301231);
767 |     }
768 |     int price = readInt("  Ticket price (Rs): ", 1, 100000);
769 |
770 |     int rows = readInt("  Rows of seats (max 5): ", 1, 1000);
771 |     while (rows > MAX_ROWS) {
772 |         cout << "  Too many rows! The seat array allows at most 5 rows x 8 seats.\n";
773 |         rows = readInt("  Rows of seats (max 5): ", 1, 1000);
774 |     }
775 |     int cols = readInt("  Seats per row (max 8): ", 1, 1000);
776 |     while (cols > MAX_COLS) {
777 |         cout << "  Too many seats per row! The seat array allows at most 5 rows x 8 seats.\n";
778 |         cols = readInt("  Seats per row (max 8): ", 1, 1000);
779 |     }
780 |
781 |     concerts.insert(new Concert(id, name, cities.cityName[city], date, price, rows, cols));
782 |     cout << "  Concert added to the BST.\n";
783 | }
```

**`validDate(d)`**: splits YYYYMMDD into day and month, and checks the month is 1 to 12
and the day is between 1 and that month's number of days. February allows 29.

**`addConcert()`**: every input is checked.
1. **ID:** must be 1 to 9999 and must not already be in the BST.
2. **Name:** any non-empty text.
3. **City:** chosen by number from the graph's list, so every concert city is in the graph.
4. **Date:** must be between 2026 and 2030, and must really exist (20260231 is rejected).
5. **Price:** must be 1 or more.
6. **Rows and seats per row:** sizes above 5 x 8 are rejected with a message, because the
   seat array is 5 x 8.
7. **Insert:** the new concert goes into the BST.

## Menu 4: Sort Concerts

```cpp
785 | // Menu 4: sorts a copy of the concert list with the chosen key and algorithm.
786 | void sortConcerts() {
787 |     cout << "\n  Sort by: 1. Price  2. Date  3. Popularity (tickets sold)\n";
788 |     int key = readInt("  Choice: ", 1, 3);
789 |     cout << "  Algorithm: 1. Bubble Sort  2. Merge Sort  3. Quick Sort\n";
790 |     int alg = readInt("  Choice: ", 1, 3);
791 |
792 |     vector<Concert*> arr = concertArray();
793 |     int n = arr.size();
794 |     if (alg == 1) SortSearch::bubbleSort(arr, key);
795 |     else if (alg == 2) SortSearch::mergeSort(arr, 0, n - 1, key);
796 |     else SortSearch::quickSort(arr, 0, n - 1, key);
797 |
798 |     string keyName[] = {"", "price", "date", "popularity"};
799 |     string algName[] = {"", "Bubble Sort", "Merge Sort", "Quick Sort"};
800 |     cout << "\n  Concerts sorted by " << keyName[key] << " using " << algName[alg] << ":\n";
801 |     printConcertList(arr);
802 | }
```

1. Read the key (price, date or popularity) and the algorithm (bubble, merge or quick).
2. Take a copy of the concerts as an array, so the BST is never changed.
3. Call the chosen sort, then print the result with a title such as "sorted by price using
   Bubble Sort".

## Menu 5: Book Ticket

```cpp
804 | // Menu 5: books a free seat, or offers the waiting list if sold out. O(h + R*C)
805 | void bookTicket() {
806 |     int u = askUser();
807 |     if (u == -1) return;
808 |     Concert* c = askConcert();
809 |     if (c == nullptr) return;
810 |
811 |     if (c->seats.isFull()) {
812 |         cout << "  Sorry, \"" << c->name << "\" is SOLD OUT.\n";
813 |         if (c->waitList.isFull()) {
814 |             cout << "  Waiting list full. Please try another concert.\n";
815 |             return;
816 |         }
817 |         if (readInt("  Join the waiting list? (1 = Yes, 0 = No): ", 0, 1) == 1) {
818 |             c->waitList.enqueue(users[u].userId);
819 |             cout << "  " << users[u].name << " added to the waiting list at position "
820 |                  << c->waitList.count << ".\n";
821 |         }
822 |         return;
823 |     }
824 |
825 |     c->seats.display();
826 |     int r, col;
827 |     while (true) {
828 |         r = readInt("  Row number: ", 1, c->seats.rows) - 1;
829 |         col = readInt("  Seat number: ", 1, c->seats.cols) - 1;
830 |         if (c->seats.isFree(r, col)) break;
831 |         cout << "  Seat " << seatLabel(r, col) << " is already booked. Choose another.\n";
832 |     }
833 |     int id = makeBooking(users[u].userId, c, r, col);
834 |     cout << "  Booking confirmed! Booking ID " << id << ", seat " << seatLabel(r, col)
835 |          << ", price Rs " << c->price << ".\n";
836 | }
```

1. Ask for a user and a concert. Stop if either is not found.
2. **If the concert is sold out:**
   - If the waiting list is full, print "Waiting list full" and stop.
   - Otherwise ask "Join the waiting list?". On yes, `enqueue` the user ID and print their position.
3. **Otherwise:**
   1. Show the seat map.
   2. Read a row and seat (the ranges are checked).
   3. If the seat is already booked, say so and ask again.
   4. Call `makeBooking` and print the booking ID, seat and price.

## Menu 6 and 7: Cancel and Undo

```cpp
838 | // Menu 6: cancels a booking, pushes it on the stack, gives the seat to the next waiting user. O(n)
839 | void cancelTicket() {
840 |     int id = readInt("  Enter Booking ID: ", -1000000, 1000000);
841 |     Booking* b = bookings.findById(id);
842 |     if (b == nullptr) {
843 |         cout << "  Invalid Booking ID.\n";
844 |         return;
845 |     }
846 |     if (b->status == "Cancelled") {
847 |         cout << "  Booking " << id << " is already cancelled.\n";
848 |         return;
849 |     }
850 |     Concert* c = concerts.search(b->concertId);
851 |     b->status = "Cancelled";
852 |     c->seats.booked[b->row][b->col] = false;
853 |     cancelled.push(id);
854 |     cout << "  Booking " << id << " cancelled. Seat " << seatLabel(b->row, b->col) << " is free.\n";
855 |
856 |     if (!c->waitList.isEmpty()) {
857 |         int nextUser = c->waitList.dequeue();
858 |         int newId = makeBooking(nextUser, c, b->row, b->col);
859 |         cout << "  Seat given to " << users[findUser(nextUser)].name << " (User " << nextUser
860 |              << ") from the waiting list. New Booking ID " << newId << ".\n";
861 |     }
862 | }
863 |
864 | // Menu 7: pops the last cancelled booking and restores it if its seat is still free. O(n)
865 | void undoCancellation() {
866 |     if (cancelled.isEmpty()) {
867 |         cout << "  Nothing to undo.\n";
868 |         return;
869 |     }
870 |     int id = cancelled.pop();
871 |     Booking* b = bookings.findById(id);
872 |     Concert* c = concerts.search(b->concertId);
873 |     if (c->seats.isFree(b->row, b->col)) {
874 |         c->seats.booked[b->row][b->col] = true;
875 |         b->status = "Booked";
876 |         cout << "  Undo done: booking " << id << " is active again (seat "
877 |              << seatLabel(b->row, b->col) << ").\n";
878 |     } else {
879 |         cout << "  Cannot undo booking " << id << ": seat " << seatLabel(b->row, b->col)
880 |              << " has already been booked by someone else.\n";
881 |     }
882 | }
```

**`cancelTicket()`**
1. Find the booking in the linked list. Print "Invalid Booking ID." if it is missing, or
   "already cancelled" if its status says so.
2. Set the status to "Cancelled".
3. Free the seat in the 2D array.
4. Push the booking ID onto the stack.
5. **Auto-allocation:** if the concert's waiting list is not empty, `dequeue` the first user
   and call `makeBooking` for them on the same seat.

**`undoCancellation()`**
1. If the stack is empty, print "Nothing to undo."
2. Pop the latest cancelled booking ID and find the booking and its concert.
3. If the seat is still free, book it again and set the status back to "Booked".
4. Otherwise refuse, because someone else now has that seat.

## Menu 8, 9 and 10: History, Waiting List, Routes

```cpp
884 | // Menu 8: prints every booking of one user from the linked list. O(n)
885 | void viewHistory() {
886 |     int u = askUser();
887 |     if (u == -1) return;
888 |     cout << "\n  Booking history of " << users[u].name << ":\n";
889 |     if (bookings.displayByUser(users[u].userId) == 0) cout << "  No bookings yet.\n";
890 | }
891 |
892 | // Menu 9: prints the waiting list of one concert. O(h + n)
893 | void showWaitingList() {
894 |     Concert* c = askConcert();
895 |     if (c == nullptr) return;
896 |     cout << "\n  Waiting list for \"" << c->name << "\":\n";
897 |     c->waitList.display();
898 | }
899 |
900 | // Menu 10: shows the city map and runs BFS, DFS, Dijkstra or the fewest-stops BFS. O(V^2)
901 | void routeInfo() {
902 |     cities.display();
903 |     cout << "\n  1. BFS traversal  2. DFS traversal  3. Shortest route (Dijkstra)\n"
904 |          << "  4. Fewest stops (BFS)  0. Back\n";
905 |     int ch = readInt("  Choice: ", 0, 4);
906 |     if (ch == 0) return;
907 |     int src = readInt("  Source city number: ", 1, cities.numCities) - 1;
908 |     if (ch == 1) cities.BFS(src);
909 |     else if (ch == 2) cities.DFS(src);
910 |     else {
911 |         int dest = readInt("  Destination city number: ", 1, cities.numCities) - 1;
912 |         if (ch == 3) cities.shortestPath(src, dest);
913 |         else cities.fewestStops(src, dest);
914 |     }
915 | }
```

- **`viewHistory()`**: asks for a user and prints their bookings by traversing the linked
  list. Both booked and cancelled bookings are shown.
- **`showWaitingList()`**: asks for a concert and prints its circular queue, front to rear.
- **`routeInfo()`**: prints the city map, then runs BFS, DFS, Dijkstra or the fewest-stops
  BFS. For Dijkstra and fewest stops (options 3 and 4) it also asks for a destination.

## Sample data

```cpp
917 | // Loads 8 cities, 7 concerts, 12 bookings and a waiting list for the demo.
918 | void loadSampleData() {
919 |     string names[] = {"Delhi", "Jaipur", "Agra", "Lucknow", "Chandigarh", "Mumbai", "Pune", "Bengaluru"};
920 |     for (int i = 0; i < 8; i++) cities.addCity(names[i]);
921 |     cities.addRoad(0, 1, 280);  cities.addRoad(0, 2, 230);  cities.addRoad(0, 4, 245);
922 |     cities.addRoad(0, 3, 600);  cities.addRoad(1, 2, 240);  cities.addRoad(2, 3, 335);
923 |     cities.addRoad(1, 5, 1150); cities.addRoad(5, 6, 150);  cities.addRoad(6, 7, 840);
924 |     cities.addRoad(5, 7, 1000);
925 |
926 |     concerts.insert(new Concert(104, "Melody Night", "Delhi", 20261220, 2500, 5, 8));
927 |     concerts.insert(new Concert(102, "Electro Beach Fest", "Mumbai", 20261205, 3000, 4, 6));
928 |     concerts.insert(new Concert(106, "Fusion Studio Night", "Bengaluru", 20270110, 1800, 5, 6));
929 |     concerts.insert(new Concert(101, "Indie Unplugged", "Pune", 20261215, 800, 2, 3));
930 |     concerts.insert(new Concert(103, "Sufi Evening", "Lucknow", 20270125, 1200, 3, 5));
931 |     concerts.insert(new Concert(105, "Bollywood Beats", "Jaipur", 20261230, 1500, 4, 5));
932 |     concerts.insert(new Concert(107, "Rock the Fort", "Agra", 20270214, 2000, 3, 6));
933 |
934 |     Concert* indie = concerts.search(101);
935 |     int who[] = {1, 2, 3, 1, 2, 3};
936 |     for (int i = 0; i < 6; i++) makeBooking(who[i], indie, i / 3, i % 3);
937 |     indie->waitList.enqueue(4);
938 |     indie->waitList.enqueue(5);
939 |
940 |     makeBooking(1, concerts.search(104), 0, 0);
941 |     makeBooking(4, concerts.search(104), 0, 1);
942 |     makeBooking(2, concerts.search(102), 1, 2);
943 |     makeBooking(5, concerts.search(102), 1, 3);
944 |     makeBooking(3, concerts.search(102), 0, 0);
945 |     makeBooking(4, concerts.search(105), 2, 1);
946 | }
```

- **Cities and roads:** 8 cities and 10 roads.
- **Concerts:** 7 concerts, inserted in the order 104, 102, 106, 101, ... so the BST is
  balanced (see Part 3).
- **Concert 101:** only 2 x 3 seats. All 6 are booked (bookings 1001-1006), and users 4 and 5
  are on its waiting list. This is ready for the sold-out and auto-allocation demo.
- **Other bookings:** 6 more bookings (1007-1012) on other concerts.

## main()

```cpp
948 | // Loads the sample data and runs the menu until the user chooses Exit.
949 | int main() {
950 |     loadSampleData();
951 |     while (true) {
952 |         cout << "\n========== CONCERT TICKET BOOKING SYSTEM ==========\n"
953 |              << "  1. View Concerts           7. Undo Last Cancellation\n"
954 |              << "  2. Search Concert          8. View Booking History\n"
955 |              << "  3. Add Concert             9. Show Waiting List\n"
956 |              << "  4. Sort Concerts          10. Route Information\n"
957 |              << "  5. Book Ticket            11. Exit\n"
958 |              << "  6. Cancel Ticket\n";
959 |         int choice = readInt("Enter choice: ", 1, 11);
960 |         switch (choice) {
961 |             case 1: viewConcerts(); break;
962 |             case 2: searchConcert(); break;
963 |             case 3: addConcert(); break;
964 |             case 4: sortConcerts(); break;
965 |             case 5: bookTicket(); break;
966 |             case 6: cancelTicket(); break;
967 |             case 7: undoCancellation(); break;
968 |             case 8: viewHistory(); break;
969 |             case 9: showWaitingList(); break;
970 |             case 10: routeInfo(); break;
971 |             case 11:
972 |                 cout << "\nThank you for using the system!\n";
973 |                 return 0;
974 |         }
975 |     }
976 | }
```

1. Load the sample data.
2. Loop forever:
   1. Print the menu.
   2. Read a choice from 1 to 11 (any other input is rejected by `readInt`).
   3. Call the matching function with `switch`.
3. Choice 11 prints "Thank you for using the system!" and returns, which ends the program.

When `main` returns, the global objects are destroyed. The destructors of the linked list
and the BST free all their nodes, so there are no memory leaks.

## Worked example

**`readInt("Enter choice: ", 1, 11)`** with the inputs `abc`, then `12`, then `5`:

```
"abc" -> cin.fail() is true -> clear, ignore line -> "Invalid input! Please enter a number."
"12"  -> a number, but out of range              -> "Please enter a number between 1 and 11."
"5"   -> valid                                    -> returns 5 (Book Ticket)
```

**Cancel booking 1001, right after the program starts**

```
Before: concert 101 seats all booked; waiting list = [4, 5]; stack empty
1. findById(1001) -> user 1, seat R1-S1, status "Booked"
2. status = "Cancelled", booked[0][0] = false, push(1001)    stack = [1001]
3. waiting list not empty -> dequeue() returns 4 (Meera)      waiting list = [5]
4. makeBooking(4, concert 101, 0, 0) -> booking 1013, seat R1-S1 booked again
Undo now: pop() returns 1001, but R1-S1 is no longer free -> undo refused
```

## Time complexity of the menu options

n is the number of bookings, c the number of concerts, h the BST height, R x C the seat
grid and V the number of cities.

| Option | Main cost |
|---|---|
| View Concerts | O(c x R x C) to print the table (each row counts its seats) |
| Search by ID (BST) | O(h) |
| Search by name | O(c) names checked |
| Search by binary search | O(c) to build the array + O(log c) to search |
| Add Concert | O(h) |
| Sort Concerts | O(c^2) bubble, O(c log c) merge / quick on average |
| Book Ticket | O(h + R x C) |
| Cancel Ticket | O(n) to find the booking (the queue and stack steps are O(1)) |
| Undo | O(n) to find the booking |
| View History | O(n) |
| Show Waiting List | O(h + 5) |
| Route Information | O(V^2) |

## Viva questions

1. **How do you stop the program from crashing when the user types letters instead of a number?**
   `readInt` checks `cin.fail()`. It then calls `cin.clear()` to reset the stream and
   `cin.ignore()` to throw away the bad input, and asks again.
2. **What happens to a freed seat when someone is on the waiting list?**
   `cancelTicket` dequeues the first waiting user and immediately books the same seat for
   them with `makeBooking`.
3. **Why is there a separate `makeBooking` function?**
   The same three steps (mark seat, add to list, next ID) are needed in booking, in
   auto-allocation and in the sample data. Writing them once avoids mistakes.
4. **Why does sorting not change the BST?**
   We sort a copy (a vector of pointers from the in-order traversal). The tree keeps its
   ID order for searching.
5. **How is memory freed at the end?**
   The global BST and linked list are destroyed when `main` returns. Their destructors
   delete every node (and the BST also deletes each concert).
