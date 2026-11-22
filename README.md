# Concert Ticket Booking and Event Management System

Data Structures (15B11CI311) project: a menu-driven C++ console program that
manages concerts, seat bookings, cancellations and waiting lists, and shows
where each data structure fits a real problem.

## How to compile and run

```
g++ -std=c++17 main.cpp -o concert
./concert            (Windows: concert.exe)
```

The whole program is in one file, `main.cpp`, so `g++ -std=c++17 *.cpp -o concert`
works too. Tested with MinGW g++ 6.3. The code does not use `std::to_string`,
`std::stoi` or `std::stod`.

Sample data is loaded at startup, so you can try every menu option right away:

- **Cities:** 8 cities with road distances in km: Delhi, Jaipur, Agra, Lucknow, Chandigarh, Mumbai, Pune and Bengaluru.
- **Concerts:** 7 concerts, IDs 101 to 107.
- **Users:** 5 users, IDs 1 to 5 (Aarav, Diya, Kabir, Meera, Rohan).
- **Bookings:** 12 bookings, IDs 1001 to 1012.
- **Ready-made demo:** concert 101 (*Indie Unplugged*, 2 x 3 seats) is sold out, and users 4 and 5 are already on its waiting list.

## Files

`main.cpp` is split into six parts, each starting with a one-line comment:

| Part | Lines in `main.cpp` | Contents | Notes |
|---|---|---|---|
| 1 | 12-152 | `SeatMap` (2D array), `User`, `Booking`, `BookingList` (linked list) | `explanations/1_SeatMap_and_Bookings.md` |
| 2 | 154-233 | `CircularQueue` (waiting list), `CancelStack` (undo) | `explanations/2_WaitingList_and_Stack.md` |
| 3 | 235-350 | `Concert`, `ConcertBST` (binary search tree) | `explanations/3_Concerts_BST.md` |
| 4 | 352-459 | bubble / merge / quick sort, linear search, binary search | `explanations/4_Sorting_and_Searching.md` |
| 5 | 461-619 | weighted graph (adjacency matrix), BFS, DFS, Dijkstra, fewest stops (BFS) | `explanations/5_City_Graph.md` |
| 6 | 621-976 | input validation, menu functions, sample data, `main()` | `explanations/6_Menu_and_Main.md` |

Other folders:
- **`explanations/`:** study notes in Markdown, not compiled. Each note has the code of its
  part with line numbers, a step-by-step explanation of each function, a worked example,
  the time complexities and 5 viva questions.
- **`tests/`:** one unit test per part, plus the input and expected output of a scripted
  full run.

## Data structures: where they are used and their cost

n = number of concerts or bookings, h = height of the BST, R x C = seat grid,
V = number of cities.

| Data structure | Where it is used | Operations and time complexity |
|---|---|---|
| 2D array `booked[5][8]` | Seat map of each concert | check/book/free a seat O(1); count booked, is full, display O(R x C) |
| Singly linked list | All booking records; a user's history is found by walking the list | insert at end O(1) (tail pointer); find by booking ID O(n); user history O(n) |
| Circular queue (array of 5) | Waiting list of a sold-out concert | enqueue O(1), dequeue O(1), isFull/isEmpty O(1), display O(n) |
| Stack (array) | IDs of cancelled bookings, for "Undo Last Cancellation" | push O(1), pop O(1) |
| Binary search tree (key = concert ID) | Storing all concerts | insert O(h), search O(h), in-order display O(n); h is about log n when balanced and n in the worst case |
| Weighted graph (adjacency matrix 8 x 8) | Cities and road distances | BFS O(V^2), DFS O(V^2), Dijkstra O(V^2), fewest stops (BFS) O(V^2) |
| Linear search | Concert by name (part of the name, any case); user by ID | O(n) |
| Binary search | Concert by ID on the array from the BST's in-order traversal (already sorted by ID) | O(log n) |
| Bubble sort | Sorting concerts by price, date or popularity | O(n^2); O(n) best case using the `swapped` flag |
| Merge sort | Same | O(n log n) in every case, O(n) extra space |
| Quick sort | Same | O(n log n) on average, O(n^2) worst case |

## Menu

1. **View Concerts:** all concerts in BST in-order (sorted by ID), then an optional seat map.
2. **Search Concert:** by ID (BST), by name (linear search), or by ID (binary search).
3. **Add Concert:**
   - The ID must be unique, and the city is picked from the graph's list.
   - The date must be a real date, and the price must be positive.
   - Seat sizes above 5 rows x 8 seats are rejected with a message.
4. **Sort Concerts:** choose the key (price, date, popularity) and the algorithm (bubble, merge, quick). This sorts a copy, so the BST does not change.
5. **Book Ticket:**
   - Pick a user and a concert, then a row and seat from the seat map. An already-booked seat is rejected.
   - If the concert is sold out, the user is offered the waiting list.
   - If the waiting list is full, the program prints "Waiting list full".
6. **Cancel Ticket:**
   - The booking's status becomes "Cancelled", the seat is freed, and the booking ID is pushed on the stack.
   - If someone is waiting, they are dequeued and given that seat automatically.
7. **Undo Last Cancellation:** pops the stack. The booking becomes "Booked" again if its seat is still free; otherwise the undo is refused with a message.
8. **View Booking History:** every booking of one user, both booked and cancelled.
9. **Show Waiting List:** the circular queue from front to rear, with the array slot of each entry.
10. **Route Information:** BFS order, DFS order, the shortest route in km (Dijkstra), or the route with the fewest stops (BFS); both routes print the path and total km, and fewest stops also prints the number of stops.
11. **Exit.**

## Sample test cases

The inputs below are typed in order after starting `./concert`. The whole set is
saved in `tests/sample_input.txt`. Running
`./concert < tests/sample_input.txt` produces `tests/sample_output.txt`.

| # | Test | Input | Expected result |
|---|---|---|---|
| 1 | View concerts | `1`, then `101` | 7 concerts in ID order; 101 marked SOLD OUT with 2 waiting; 2 x 3 seat map all `[X]` |
| 2 | Search by ID (BST) | `2`, `1`, `103` | Sufi Evening shown |
| 3 | Search by name | `2`, `2`, `night` | Melody Night and Fusion Studio Night |
| 4 | Binary search | `2`, `3`, `106` | "Found at index 5 of the sorted array" |
| 5 | Search, not found | `2`, `3`, `999` | "Concert not found." |
| 6 | Add, duplicate ID | `3`, `104` | "A concert with ID 104 already exists." |
| 7 | Add, bad input | `3`, `108`, name, city `5`, date `20260231`, then `20261111`, price `abc`, then `1500`, rows `9`, then `2`, seats `10`, then `3` | "That date does not exist", "Invalid input!", "Too many rows!", "Too many seats per row!", then "Concert added" |
| 8 | Sort | `4`, `3`, `3` | Quick sort by popularity: 101 (6 sold) first |
| 9 | Unknown user | `5`, `9` | "User not found." |
| 10 | Sold out, join waiting list | `5`, `1`, `101`, `1` (same again for users 2 and 3) | Added at positions 3, 4, 5 |
| 11 | Waiting list full | `5`, `1`, `101` | "Waiting list full." |
| 12 | Already-booked seat | `5`, `2`, `104`, row `1`, seat `1`, then `2`, `2` | "Seat R1-S1 is already booked", then booking 1013 confirmed |
| 13 | Invalid booking ID | `6`, `9999` | "Invalid Booking ID." |
| 14 | Cancel, auto-allocation | `6`, `1001` | Seat R1-S1 freed and given to Meera (user 4) as booking 1014 |
| 15 | Cancel twice | `6`, `1001` | "already cancelled" |
| 16 | Undo, seat taken | `7` | "Cannot undo booking 1001: seat R1-S1 has already been booked by someone else." |
| 17 | Undo, seat free | `6`, `1013`, then `7` | Booking 1013 cancelled, then "Undo done" |
| 18 | Undo, empty stack | `7` | "Nothing to undo." |
| 19 | History | `8`, `1` | 1001 Cancelled, 1004 Booked, 1007 Booked |
| 20 | Waiting list display | `9`, `101` | Users 5, 1, 2, 3; front index 1 |
| 21 | Dijkstra | `10`, `3`, `1`, `8` | Delhi -> Jaipur -> Mumbai -> Pune -> Bengaluru, 2420 km |
| 22 | BFS | `10`, `1`, `6` | Mumbai (0); Jaipur, Pune, Bengaluru (1); Delhi, Agra (2); Lucknow, Chandigarh (3) |
| 23 | DFS | `10`, `2`, `8` | Bengaluru Mumbai Jaipur Delhi Agra Lucknow Chandigarh Pune |
| 24 | Fewest stops (BFS) | `10`, `4`, `1`, `4` | Delhi -> Lucknow (the direct road), 1 stop, 600 km |
| 25 | Dijkstra, fewer km than fewest stops | `10`, `3`, `1`, `4` | Delhi -> Agra -> Lucknow, 565 km |
| 26 | Bad menu input | `abc`, `0`, `12` | "Invalid input!", then "between 1 and 11" twice |
| 27 | Exit | `11` | "Thank you for using the system!" |

Tests 24 and 25 show the difference between the two route options: Delhi to Lucknow by
fewest stops is the **direct 600 km road**, while Dijkstra finds **565 km via Agra** (one more stop).

### Unit tests

Each part of `main.cpp` has its own test in `tests/`. A test includes `main.cpp`
with `#define main program_main`, which renames the program's `main()` so the test
can have its own. `main.cpp` itself is not changed. Run the tests from the project
folder:

```
g++ -std=c++17 tests/test_seats_bookings.cpp -o t && ./t     (Part 1)
g++ -std=c++17 tests/test_queue_stack.cpp    -o t && ./t     (Part 2)
g++ -std=c++17 tests/test_concerts.cpp       -o t && ./t     (Part 3)
g++ -std=c++17 tests/test_sort_search.cpp    -o t && ./t     (Part 4)
g++ -std=c++17 tests/test_graph.cpp          -o t && ./t     (Part 5)
```

## Viva notes

These are short summaries. The full notes, with code, worked examples and practice
questions, are in `explanations/`.

### 2D array: seat map
- **What it does here:** `booked[r][c]` tells whether row r, seat c of a concert is taken. Booking sets it to true; cancelling sets it back to false.
- **Why it was chosen:** a hall is a grid of rows and seats, so a 2D array matches it exactly. Any seat can be reached directly by its row and column.
- **Time complexity:** checking or changing one seat is O(1). Counting booked seats or printing the map visits every seat, so it is O(R x C).

### Linked list: booking records
- **What it does here:**
  - Every booking (active or cancelled) is one node.
  - Cancelling only changes the status, so the full history is kept.
  - A user's history is found by walking the list and printing nodes with that user's ID.
- **Why it was chosen:** the number of bookings keeps growing and is not known in advance. A linked list grows one node at a time and never needs resizing.
- **Time complexity:**
  - Insert at the end is O(1) because we keep a `tail` pointer.
  - Finding a booking by ID is O(n), because nodes can only be reached by walking from `head`.
  - Showing a user's history is also O(n).

### Circular queue: waiting list
- **What it does here:** when a concert is sold out, user IDs are stored in a fixed array of 5, and the first person in is the first to get a freed seat (FIFO).
- **Why it was chosen:** a plain array queue wastes the slots in front after each dequeue. In a circular queue, `rear = (rear + 1) % 5` wraps back to index 0 and reuses those slots. The fixed size also gives a natural "Waiting list full" limit.
- **Time complexity:** enqueue, dequeue, isFull and isEmpty are O(1). Display is O(n).

### Stack: undo last cancellation
- **What it does here:** every cancelled booking ID is pushed. Undo pops the most recent one and restores it if the seat is still free.
- **Why it was chosen:** undo must reverse the *latest* action first, which is exactly Last-In-First-Out.
- **Time complexity:** push and pop are O(1).

### Binary search tree: concerts by ID
- **What it does here:** stores all concerts. Smaller IDs go to the left, bigger IDs go to the right.
  - Search follows one path from the root.
  - In-order traversal (left, node, right) lists concerts in ID order.
- **Why it was chosen:** each comparison skips a whole subtree, so search is faster than checking every concert. In-order traversal also gives a sorted list for free.
- **Time complexity:**
  - Insert and search are O(h): about O(log n) when the tree is balanced, but O(n) if IDs are inserted in sorted order, because the tree becomes a chain.
  - Traversal is O(n).
  - The sample data is inserted in the order 104, 102, 106, 101, ... so the tree is balanced.

### Weighted graph: cities and routes
- **What it does here:** cities are vertices and roads are edges, weighted by distance in km. The graph is stored in an 8 x 8 adjacency matrix where `road[a][b] = km` and 0 means no road.
  - **BFS** uses a queue and visits cities level by level (how many roads away).
  - **DFS** uses recursion and goes as deep as possible before backtracking.
  - **Dijkstra** repeatedly picks the closest unfinished city and relaxes its edges. A `parent[]` array is used to rebuild the path.
  - **Fewest stops** is the same BFS with a `parent[]` array, rebuilt the same way as in Dijkstra. It minimises roads, not km: Delhi to Lucknow is the direct 600 km road, while Dijkstra goes via Agra (565 km).
- **Why it was chosen:** a road network *is* a graph, and with only 8 cities a matrix is simple and fast to look up.
- **Time complexity:** BFS, DFS, Dijkstra and fewest stops are each O(V^2) with an adjacency matrix. For each city we scan its whole row of V entries.

### Searching
- **Linear search:**
  - Used for concert name, because names are not sorted and partial matches are allowed, and for user ID.
  - It checks each item one by one: O(n).
- **Binary search:**
  - Used for concert ID on the array produced by the BST in-order traversal, which is already sorted.
  - It halves the range each step: O(log n). It only works on sorted data.

### Sorting (by price, date or popularity)
Dates are stored as YYYYMMDD integers so they compare like numbers. Popularity is sorted using the negative of tickets sold, so the most popular concert comes first. A copy of the list is sorted, so the BST is never changed.

- **Bubble sort:**
  - Swaps neighbours that are out of order.
  - O(n^2), or O(n) if the list is already sorted, thanks to the `swapped` flag.
  - Simplest of the three.
- **Merge sort:**
  - Splits the list in half, sorts each half, then merges them.
  - Always O(n log n), and needs O(n) extra space.
  - Stable: equal prices keep their order.
- **Quick sort:**
  - Uses the last element as the pivot, puts smaller items to its left, then recurses on each side.
  - O(n log n) on average, O(n^2) worst case (for example, already-sorted input with this pivot).
  - Sorts in place.
