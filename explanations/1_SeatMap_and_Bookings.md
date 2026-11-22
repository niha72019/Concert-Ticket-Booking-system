# Part 1: Seat Map (2D Array) and Bookings (Linked List)

Lines 12 to 152 of `main.cpp`.

## What this part does and why

**Seat map (2D array).** A concert hall is made of rows, and each row has seats. A 2D
array `booked[row][col]` copies that layout exactly. `true` means the seat is booked and
`false` means it is free. Any seat can be reached directly by its row and column
number, so checking or booking a seat takes the same time however big the hall is.

**Bookings (linked list).** Bookings keep arriving while the program runs, and we do not
know how many there will be. A linked list grows one node at a time and never needs to be
resized. All bookings live in **one** list.
- A user's booking history is found by walking the list and printing only the nodes with
  that user's ID.
- A cancelled booking is **not** deleted: its status changes to "Cancelled". This keeps the
  history complete and lets us undo a cancellation later.

`User` and `Booking` are simple classes with public data members.

## Constants and seatLabel

```cpp
 12 | // Part 1: Seat Map (2D Array) and Bookings (Linked List)
 13 |
 14 | const int MAX_ROWS = 5;
 15 | const int MAX_COLS = 8;
 16 |
 17 | // Turns a 0-based seat position into a label like "R2-S3". O(1)
 18 | string seatLabel(int row, int col) {
 19 |     stringstream ss;
 20 |     ss << "R" << row + 1 << "-S" << col + 1;
 21 |     return ss.str();
 22 | }
```

- Lines 14-15: the biggest hall allowed is 5 rows by 8 seats. The array is always this
  size; a smaller concert just uses part of it.
- `seatLabel` (lines 18-22): inside the program rows and seats start at 0, but people count
  from 1. So row 1, col 2 becomes `"R2-S3"`. A `stringstream` joins the text and numbers
  (we do not use `to_string` because old MinGW does not support it).

## The SeatMap class

```cpp
 24 | class SeatMap {
 25 | public:
 26 |     int rows, cols;
 27 |     bool booked[MAX_ROWS][MAX_COLS];
 28 |
 29 |     // Sets the size and marks every seat as free. O(R*C)
 30 |     SeatMap(int r, int c) : rows(r), cols(c) {
 31 |         for (int i = 0; i < MAX_ROWS; i++)
 32 |             for (int j = 0; j < MAX_COLS; j++) booked[i][j] = false;
 33 |     }
 34 |
 35 |     // True if the seat exists and is not booked. O(1)
 36 |     bool isFree(int r, int c) {
 37 |         return r >= 0 && r < rows && c >= 0 && c < cols && !booked[r][c];
 38 |     }
 39 |
 40 |     // Counts booked seats by visiting every seat. O(R*C)
 41 |     int countBooked() {
 42 |         int n = 0;
 43 |         for (int i = 0; i < rows; i++)
 44 |             for (int j = 0; j < cols; j++)
 45 |                 if (booked[i][j]) n++;
 46 |         return n;
 47 |     }
 48 |
 49 |     // True if no seat is free. O(R*C)
 50 |     bool isFull() { return countBooked() == rows * cols; }
 51 |
 52 |     // Prints the seat grid with [X] for booked and [ ] for free. O(R*C)
 53 |     void display() {
 54 |         cout << "\n          STAGE\n      ";
 55 |         for (int j = 0; j < cols; j++) cout << " S" << j + 1 << " ";
 56 |         cout << "\n";
 57 |         for (int i = 0; i < rows; i++) {
 58 |             cout << "  R" << i + 1 << "  ";
 59 |             for (int j = 0; j < cols; j++) cout << (booked[i][j] ? " [X]" : " [ ]");
 60 |             cout << "\n";
 61 |         }
 62 |         cout << "  [ ] free  [X] booked   (" << rows * cols - countBooked()
 63 |              << " of " << rows * cols << " seats free)\n";
 64 |     }
 65 | };
```

**Constructor `SeatMap(r, c)`** (lines 30-33)
1. Saves how many rows and seats this concert really uses.
2. Two nested loops set every cell of the 5 x 8 array to `false` (free).

**`isFree(r, c)`** (lines 36-38)
1. Checks that the row is between 0 and rows-1 and the seat is between 0 and cols-1.
2. If it is inside the hall, returns true only when `booked[r][c]` is false.
3. Because of the bounds check, a wrong seat number never reads outside the array.

**`countBooked()`** (lines 41-47)
1. Starts a counter at 0.
2. Visits every seat in every used row and adds 1 for each booked seat.
3. Returns the counter. This number is also the concert's **popularity** (tickets sold).

**`isFull()`** (line 50): the hall is full when booked seats equal rows x cols.

**`display()`** (lines 53-64)
1. Prints "STAGE" and the seat numbers S1, S2, ... as column titles.
2. For each row prints R1, R2, ... and then `[X]` for booked or `[ ]` for free.
3. Prints how many seats are free out of the total.

## User and Booking

```cpp
 67 | class User {
 68 | public:
 69 |     int userId;
 70 |     string name;
 71 | };
 72 |
 73 | class Booking {
 74 | public:
 75 |     int bookingId, userId, concertId;
 76 |     string concertName;
 77 |     int row, col;
 78 |     string status;
 79 |
 80 |     // Creates a booking with status "Booked". O(1)
 81 |     Booking(int id, int uid, int cid, string cname, int r, int c)
 82 |         : bookingId(id), userId(uid), concertId(cid), concertName(cname),
 83 |           row(r), col(c), status("Booked") {}
 84 |
 85 |     // Prints this booking as one table row. O(1)
 86 |     void display() {
 87 |         cout << "  " << left << setw(8) << bookingId << setw(7) << userId << setw(7)
 88 |              << concertId << setw(24) << concertName << setw(9) << seatLabel(row, col)
 89 |              << status << right << "\n";
 90 |     }
 91 | };
```

- `User` just stores an ID and a name. The 5 users are fixed in Part 6.
- `Booking` stores who booked (userId), which concert (concertId and name), which seat
  (row, col) and the status.
- **Constructor** (lines 81-83): copies the values in and always starts with status
  `"Booked"`.
- **`display()`** (lines 86-90): prints one row of the history table. `setw` gives each
  column a fixed width, `left` lines text up on the left.

## The linked list: BookingNode and BookingList

```cpp
 93 | class BookingNode {
 94 | public:
 95 |     Booking data;
 96 |     BookingNode* next;
 97 |
 98 |     // Creates a node holding one booking. O(1)
 99 |     BookingNode(Booking b) : data(b), next(nullptr) {}
100 | };
101 |
102 | class BookingList {
103 | public:
104 |     BookingNode* head;
105 |     BookingNode* tail;
106 |     int count;
107 |
108 |     // Creates an empty list. O(1)
109 |     BookingList() : head(nullptr), tail(nullptr), count(0) {}
110 |
111 |     // Deletes every node when the program ends. O(n)
112 |     ~BookingList() {
113 |         while (head != nullptr) {
114 |             BookingNode* temp = head;
115 |             head = head->next;
116 |             delete temp;
117 |         }
118 |     }
119 |
120 |     // Adds a booking after the tail, keeping bookings in time order. O(1)
121 |     void insertEnd(Booking b) {
122 |         BookingNode* node = new BookingNode(b);
123 |         if (head == nullptr) head = tail = node;
124 |         else {
125 |             tail->next = node;
126 |             tail = node;
127 |         }
128 |         count++;
129 |     }
130 |
131 |     // Linear search from head for a booking ID, nullptr if not found. O(n)
132 |     Booking* findById(int id) {
133 |         for (BookingNode* p = head; p != nullptr; p = p->next)
134 |             if (p->data.bookingId == id) return &p->data;
135 |         return nullptr;
136 |     }
137 |
138 |     // Prints all bookings of one user and returns how many there were. O(n)
139 |     int displayByUser(int userId) {
140 |         int found = 0;
141 |         for (BookingNode* p = head; p != nullptr; p = p->next) {
142 |             if (p->data.userId != userId) continue;
143 |             if (found == 0)
144 |                 cout << "  " << left << setw(8) << "BkID" << setw(7) << "User" << setw(7)
145 |                      << "ConID" << setw(24) << "Concert" << setw(9) << "Seat" << "Status"
146 |                      << right << "\n  " << string(62, '-') << "\n";
147 |             p->data.display();
148 |             found++;
149 |         }
150 |         return found;
151 |     }
152 | };
```

A **node** holds one `Booking` (the data) and a pointer `next` to the following node. The list
keeps three things: `head` (first node), `tail` (last node) and `count`.

**Constructor** (line 109): empty list, so head and tail are `nullptr` and count is 0.

**Destructor `~BookingList()`** (lines 112-118)
1. While there is a first node, remember it in `temp`.
2. Move `head` to the next node.
3. Delete `temp`.
This frees every node when the program ends, so there is no memory leak.

**`insertEnd(b)`** (lines 121-129)
1. Create a new node with `new`.
2. If the list is empty, the new node is both head and tail.
3. Otherwise link it after the current tail and make it the new tail.
4. Add 1 to count.
Because we keep a `tail` pointer we do not need to walk the list, so this is O(1).
Bookings therefore stay in the order they were made.

**`findById(id)`** (lines 132-136)
1. Start at head.
2. Move node by node (`p = p->next`) until the booking ID matches.
3. Return the address of that booking so the caller can change its status,
   or `nullptr` if we reached the end.
This is a **linear search**.

**`displayByUser(userId)`** (lines 139-151)
1. Walk the whole list.
2. Skip nodes of other users (`continue`).
3. Before the first match print the table header.
4. Print the booking and count it.
5. Return the count, so the menu can say "No bookings yet" when it is 0.

## Worked example

**Seat map.** Start with a 2 x 3 hall, then book row 0 seat 1 and row 1 seat 2.

```
          S1  S2  S3
  R1     [ ] [X] [ ]
  R2     [ ] [ ] [X]
```

- `booked[0][1] = true` and `booked[1][2] = true`.
- `isFree(0, 1)` is false, `isFree(0, 0)` is true, and `isFree(2, 0)` is false (no row 3).
- `countBooked()` is 2, and `isFull()` is false (2 of 6 seats are booked).
- `seatLabel(1, 2)` is `"R2-S3"`.

**Linked list.** Insert three bookings:

```
insertEnd(1001, user 1)   head -> [1001] -> NULL                       tail = 1001
insertEnd(1002, user 2)   head -> [1001] -> [1002] -> NULL             tail = 1002
insertEnd(1003, user 1)   head -> [1001] -> [1002] -> [1003] -> NULL   tail = 1003
```

- `findById(1002)` checks 1001, then 1002, and finds it after 2 steps.
- Cancelling 1002 sets its status to "Cancelled". The node stays in the list.
- `displayByUser(1)` walks all 3 nodes, prints 1001 and 1003, and returns 2.

## Time complexity

R x C is the size of the seat grid; n is the number of bookings.

| Operation | Time |
|---|---|
| `seatLabel` | O(1) |
| `SeatMap` constructor | O(R x C) |
| `isFree` | O(1) |
| `countBooked`, `isFull`, `display` | O(R x C) |
| `insertEnd` | O(1) (thanks to the tail pointer) |
| `findById` | O(n) |
| `displayByUser` | O(n) |
| destructor | O(n) |

## Viva questions

1. **Why did you use a 2D array for seats?**
   A hall is a grid of rows and seats, so a 2D array matches it exactly and gives O(1)
   access to any seat by `booked[row][col]`.
2. **Why a linked list for bookings and not an array?**
   The number of bookings is not known in advance. A linked list grows one node at a time
   without resizing or shifting elements.
3. **How is insertion at the end O(1)?**
   We keep a `tail` pointer to the last node, so we link the new node directly without
   walking the list.
4. **How do you show one user's booking history?**
   We traverse the whole booking list and print only nodes whose `userId` matches. This is O(n).
5. **Why do you not delete a node when a booking is cancelled?**
   We only change its status to "Cancelled". The history stays complete, and the undo
   option can set it back to "Booked".
