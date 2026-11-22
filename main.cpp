// Concert Ticket Booking and Event Management System. Compile: g++ -std=c++17 main.cpp -o concert

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
using namespace std;

// Part 1: Seat Map (2D Array) and Bookings (Linked List)

const int MAX_ROWS = 5;
const int MAX_COLS = 8;

// Turns a 0-based seat position into a label like "R2-S3". O(1)
string seatLabel(int row, int col) {
    stringstream ss;
    ss << "R" << row + 1 << "-S" << col + 1;
    return ss.str();
}

class SeatMap {
public:
    int rows, cols;
    bool booked[MAX_ROWS][MAX_COLS];

    // Sets the size and marks every seat as free. O(R*C)
    SeatMap(int r, int c) : rows(r), cols(c) {
        for (int i = 0; i < MAX_ROWS; i++)
            for (int j = 0; j < MAX_COLS; j++) booked[i][j] = false;
    }

    // True if the seat exists and is not booked. O(1)
    bool isFree(int r, int c) {
        return r >= 0 && r < rows && c >= 0 && c < cols && !booked[r][c];
    }

    // Counts booked seats by visiting every seat. O(R*C)
    int countBooked() {
        int n = 0;
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                if (booked[i][j]) n++;
        return n;
    }

    // True if no seat is free. O(R*C)
    bool isFull() { return countBooked() == rows * cols; }

    // Prints the seat grid with [X] for booked and [ ] for free. O(R*C)
    void display() {
        cout << "\n          STAGE\n      ";
        for (int j = 0; j < cols; j++) cout << " S" << j + 1 << " ";
        cout << "\n";
        for (int i = 0; i < rows; i++) {
            cout << "  R" << i + 1 << "  ";
            for (int j = 0; j < cols; j++) cout << (booked[i][j] ? " [X]" : " [ ]");
            cout << "\n";
        }
        cout << "  [ ] free  [X] booked   (" << rows * cols - countBooked()
             << " of " << rows * cols << " seats free)\n";
    }
};

class User {
public:
    int userId;
    string name;
};

class Booking {
public:
    int bookingId, userId, concertId;
    string concertName;
    int row, col;
    string status;

    // Creates a booking with status "Booked". O(1)
    Booking(int id, int uid, int cid, string cname, int r, int c)
        : bookingId(id), userId(uid), concertId(cid), concertName(cname),
          row(r), col(c), status("Booked") {}

    // Prints this booking as one table row. O(1)
    void display() {
        cout << "  " << left << setw(8) << bookingId << setw(7) << userId << setw(7)
             << concertId << setw(24) << concertName << setw(9) << seatLabel(row, col)
             << status << right << "\n";
    }
};

class BookingNode {
public:
    Booking data;
    BookingNode* next;

    // Creates a node holding one booking. O(1)
    BookingNode(Booking b) : data(b), next(nullptr) {}
};

class BookingList {
public:
    BookingNode* head;
    BookingNode* tail;
    int count;

    // Creates an empty list. O(1)
    BookingList() : head(nullptr), tail(nullptr), count(0) {}

    // Deletes every node when the program ends. O(n)
    ~BookingList() {
        while (head != nullptr) {
            BookingNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Adds a booking after the tail, keeping bookings in time order. O(1)
    void insertEnd(Booking b) {
        BookingNode* node = new BookingNode(b);
        if (head == nullptr) head = tail = node;
        else {
            tail->next = node;
            tail = node;
        }
        count++;
    }

    // Linear search from head for a booking ID, nullptr if not found. O(n)
    Booking* findById(int id) {
        for (BookingNode* p = head; p != nullptr; p = p->next)
            if (p->data.bookingId == id) return &p->data;
        return nullptr;
    }

    // Prints all bookings of one user and returns how many there were. O(n)
    int displayByUser(int userId) {
        int found = 0;
        for (BookingNode* p = head; p != nullptr; p = p->next) {
            if (p->data.userId != userId) continue;
            if (found == 0)
                cout << "  " << left << setw(8) << "BkID" << setw(7) << "User" << setw(7)
                     << "ConID" << setw(24) << "Concert" << setw(9) << "Seat" << "Status"
                     << right << "\n  " << string(62, '-') << "\n";
            p->data.display();
            found++;
        }
        return found;
    }
};

// Part 2: Circular Queue (Waiting List) and Stack (Undo Cancellation)

const int WAIT_SIZE = 5;
const int STACK_SIZE = 100;

class CircularQueue {
public:
    int items[WAIT_SIZE];
    int front, rear, count;

    // Creates an empty queue. O(1)
    CircularQueue() : front(0), rear(-1), count(0) {}

    // True if nobody is waiting. O(1)
    bool isEmpty() { return count == 0; }

    // True if all 5 places are used. O(1)
    bool isFull() { return count == WAIT_SIZE; }

    // Adds a user at the rear, wrapping to index 0 after the end. O(1)
    bool enqueue(int userId) {
        if (isFull()) return false;
        rear = (rear + 1) % WAIT_SIZE;
        items[rear] = userId;
        count++;
        return true;
    }

    // Removes and returns the user at the front, or -1 if empty. O(1)
    int dequeue() {
        if (isEmpty()) return -1;
        int userId = items[front];
        front = (front + 1) % WAIT_SIZE;
        count--;
        return userId;
    }

    // Prints the waiting users from front to rear. O(n)
    void display() {
        if (isEmpty()) {
            cout << "  Waiting list is empty.\n";
            return;
        }
        cout << "  (front index = " << front << ", rear index = " << rear
             << ", " << count << "/" << WAIT_SIZE << " used)\n";
        for (int i = 0; i < count; i++) {
            int index = (front + i) % WAIT_SIZE;
            cout << "  Position " << i + 1 << ": User " << items[index]
                 << "   [array slot " << index << "]\n";
        }
    }
};

class CancelStack {
public:
    int items[STACK_SIZE];
    int top;

    // Creates an empty stack (top = -1). O(1)
    CancelStack() { top = -1; }

    // True if the stack has no items. O(1)
    bool isEmpty() { return top == -1; }

    // True if the array is full. O(1)
    bool isFull() { return top == STACK_SIZE - 1; }

    // Puts a cancelled booking ID on top. O(1)
    bool push(int bookingId) {
        if (isFull()) return false;
        items[++top] = bookingId;
        return true;
    }

    // Removes and returns the top booking ID, or -1 if empty. O(1)
    int pop() {
        if (isEmpty()) return -1;
        return items[top--];
    }
};

// Part 3: Concerts and Binary Search Tree (key = concert ID)

// Turns a YYYYMMDD number into "DD-MM-YYYY". O(1)
string formatDate(int d) {
    stringstream ss;
    ss << setfill('0') << setw(2) << d % 100 << "-" << setw(2) << (d / 100) % 100
       << "-" << d / 10000;
    return ss.str();
}

class Concert {
public:
    int id;
    string name;
    string city;
    int date;
    int price;
    SeatMap seats;
    CircularQueue waitList;

    // Creates a concert with an empty seat map and waiting list. O(R*C)
    Concert(int i, string n, string c, int d, int p, int rows, int cols)
        : id(i), name(n), city(c), date(d), price(p), seats(rows, cols) {}

    // Popularity: number of tickets sold. O(R*C)
    int ticketsSold() { return seats.countBooked(); }

    // Prints the column titles of the concert table. O(1)
    static void printHeader() {
        cout << "  " << left << setw(6) << "ID" << setw(24) << "Name" << setw(12) << "City"
             << setw(12) << "Date" << setw(8) << "Price" << setw(6) << "Sold"
             << setw(6) << "Free" << "Waiting" << right << "\n  " << string(80, '-') << "\n";
    }

    // Prints this concert as one table row. O(R*C)
    void printRow() {
        cout << "  " << left << setw(6) << id << setw(24) << name << setw(12) << city
             << setw(12) << formatDate(date) << setw(8) << price << setw(6) << ticketsSold()
             << setw(6) << seats.rows * seats.cols - ticketsSold() << waitList.count
             << (seats.isFull() ? "  SOLD OUT" : "") << right << "\n";
    }
};

class BSTNode {
public:
    Concert* data;
    BSTNode* left;
    BSTNode* right;

    // Creates a tree node with no children. O(1)
    BSTNode(Concert* c) : data(c), left(nullptr), right(nullptr) {}
};

class ConcertBST {
public:
    BSTNode* root;
    int count;

    // Creates an empty tree. O(1)
    ConcertBST() : root(nullptr), count(0) {}

    // Frees all nodes and concerts when the program ends. O(n)
    ~ConcertBST() { destroy(root); }

    // Post-order delete: children first, then the node. O(n)
    void destroy(BSTNode* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node->data;
        delete node;
    }

    // Inserts a concert, returns false if the ID already exists. O(h)
    bool insert(Concert* c) {
        if (search(c->id) != nullptr) return false;
        root = insertNode(root, c);
        count++;
        return true;
    }

    // Goes left for a smaller ID, right for a bigger one, until an empty spot. O(h)
    BSTNode* insertNode(BSTNode* node, Concert* c) {
        if (node == nullptr) return new BSTNode(c);
        if (c->id < node->data->id) node->left = insertNode(node->left, c);
        else node->right = insertNode(node->right, c);
        return node;
    }

    // Finds a concert by ID, each step skips one subtree. O(h)
    Concert* search(int id) {
        BSTNode* cur = root;
        while (cur != nullptr) {
            if (id == cur->data->id) return cur->data;
            if (id < cur->data->id) cur = cur->left;
            else cur = cur->right;
        }
        return nullptr;
    }

    // In-order traversal (left, node, right) prints concerts in ID order. O(n)
    void inorderDisplay(BSTNode* node) {
        if (node == nullptr) return;
        inorderDisplay(node->left);
        node->data->printRow();
        inorderDisplay(node->right);
    }

    // In-order copy into an array, so the array is sorted by ID. O(n)
    void inorderToArray(BSTNode* node, vector<Concert*>& arr) {
        if (node == nullptr) return;
        inorderToArray(node->left, arr);
        arr.push_back(node->data);
        inorderToArray(node->right, arr);
    }
};

// Part 4: Sorting (bubble, merge, quick) and Searching (linear, binary)

class SortSearch {
public:
    // Value to sort by: 1 price, 2 date, 3 popularity (negative so most sold comes first). O(R*C)
    static int keyValue(Concert* c, int key) {
        if (key == 1) return c->price;
        if (key == 2) return c->date;
        return -c->ticketsSold();
    }

    // Bubble sort, stops early if a pass makes no swap. O(n^2), best O(n)
    static void bubbleSort(vector<Concert*>& a, int key) {
        int n = a.size();
        for (int pass = 0; pass < n - 1; pass++) {
            bool swapped = false;
            for (int j = 0; j < n - 1 - pass; j++) {
                if (keyValue(a[j], key) > keyValue(a[j + 1], key)) {
                    Concert* temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    // Merge sort: split in half, sort both halves, merge. O(n log n)
    static void mergeSort(vector<Concert*>& a, int left, int right, int key) {
        if (left >= right) return;
        int mid = (left + right) / 2;
        mergeSort(a, left, mid, key);
        mergeSort(a, mid + 1, right, key);
        merge(a, left, mid, right, key);
    }

    // Merges the sorted halves a[left..mid] and a[mid+1..right]. O(n)
    static void merge(vector<Concert*>& a, int left, int mid, int right, int key) {
        vector<Concert*> temp;
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            if (keyValue(a[i], key) <= keyValue(a[j], key)) temp.push_back(a[i++]);
            else temp.push_back(a[j++]);
        }
        while (i <= mid) temp.push_back(a[i++]);
        while (j <= right) temp.push_back(a[j++]);
        for (int k = 0; k < (int)temp.size(); k++) a[left + k] = temp[k];
    }

    // Quick sort around a pivot. Average O(n log n), worst O(n^2)
    static void quickSort(vector<Concert*>& a, int low, int high, int key) {
        if (low >= high) return;
        int p = partition(a, low, high, key);
        quickSort(a, low, p - 1, key);
        quickSort(a, p + 1, high, key);
    }

    // Uses the last element as pivot and moves smaller ones to its left. O(n)
    static int partition(vector<Concert*>& a, int low, int high, int key) {
        int pivot = keyValue(a[high], key);
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (keyValue(a[j], key) < pivot) {
                i++;
                Concert* temp = a[i]; a[i] = a[j]; a[j] = temp;
            }
        }
        Concert* temp = a[i + 1]; a[i + 1] = a[high]; a[high] = temp;
        return i + 1;
    }

    // Converts one capital letter to lower case. O(1)
    static char lower(char ch) {
        return (ch >= 'A' && ch <= 'Z') ? ch - 'A' + 'a' : ch;
    }

    // True if text appears inside name, ignoring case. O(name length x text length)
    static bool containsText(string name, string text) {
        int n = name.size(), m = text.size();
        for (int start = 0; start + m <= n; start++) {
            int k = 0;
            while (k < m && lower(name[start + k]) == lower(text[k])) k++;
            if (k == m) return true;
        }
        return false;
    }

    // Linear search: checks every concert name one by one. O(n)
    static vector<Concert*> linearSearchByName(vector<Concert*>& a, string text) {
        vector<Concert*> found;
        for (int i = 0; i < (int)a.size(); i++)
            if (containsText(a[i]->name, text)) found.push_back(a[i]);
        return found;
    }

    // Binary search by ID on an array sorted by ID, returns index or -1. O(log n)
    static int binarySearchById(vector<Concert*>& a, int id) {
        int low = 0, high = (int)a.size() - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            if (a[mid]->id == id) return mid;
            if (a[mid]->id < id) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }
};

// Part 5: City Graph (weighted, adjacency matrix) with BFS, DFS and Dijkstra

const int MAX_CITIES = 8;
const int INF = 1000000000;

class CityGraph {
public:
    int numCities;
    string cityName[MAX_CITIES];
    int road[MAX_CITIES][MAX_CITIES];

    // Creates a graph with no cities and no roads (0 means no road). O(V^2)
    CityGraph() : numCities(0) {
        for (int i = 0; i < MAX_CITIES; i++)
            for (int j = 0; j < MAX_CITIES; j++)
                road[i][j] = 0;
    }

    // Adds a city as a new vertex. O(1)
    void addCity(string name) {
        if (numCities < MAX_CITIES) cityName[numCities++] = name;
    }

    // Adds a two-way road of the given length in km. O(1)
    void addRoad(int a, int b, int km) {
        road[a][b] = km;
        road[b][a] = km;
    }

    // Prints every city with its direct roads. O(V^2)
    void display() {
        cout << "\n  Cities and direct roads:\n";
        for (int i = 0; i < numCities; i++) {
            cout << "  " << i + 1 << ". " << cityName[i] << "  ->  ";
            for (int j = 0; j < numCities; j++)
                if (road[i][j] > 0) cout << cityName[j] << " (" << road[i][j] << " km)  ";
            cout << "\n";
        }
    }

    // BFS: visits cities level by level using a queue. O(V^2)
    void BFS(int start) {
        bool visited[MAX_CITIES] = {false};
        int level[MAX_CITIES] = {0};
        queue<int> q;
        visited[start] = true;
        q.push(start);
        cout << "\n  BFS order from " << cityName[start] << ":\n";
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << "  " << cityName[u] << "  (" << level[u] << " road(s) away)\n";
            for (int v = 0; v < numCities; v++) {
                if (road[u][v] > 0 && !visited[v]) {
                    visited[v] = true;
                    level[v] = level[u] + 1;
                    q.push(v);
                }
            }
        }
    }

    // DFS: starts the recursive deep-first visit. O(V^2)
    void DFS(int start) {
        bool visited[MAX_CITIES] = {false};
        cout << "\n  DFS order from " << cityName[start] << ":\n  ";
        dfsVisit(start, visited);
        cout << "\n";
    }

    // Visits a city, then each unvisited neighbour before backtracking. O(V) per call
    void dfsVisit(int u, bool visited[]) {
        visited[u] = true;
        cout << cityName[u] << "  ";
        for (int v = 0; v < numCities; v++)
            if (road[u][v] > 0 && !visited[v]) dfsVisit(v, visited);
    }

    // Dijkstra: prints the shortest path and returns its km, or -1 if no route. O(V^2)
    int shortestPath(int src, int dest) {
        int dist[MAX_CITIES], parent[MAX_CITIES];
        bool done[MAX_CITIES];
        for (int i = 0; i < numCities; i++) {
            dist[i] = INF;
            parent[i] = -1;
            done[i] = false;
        }
        dist[src] = 0;

        for (int round = 0; round < numCities; round++) {
            int u = -1;
            for (int i = 0; i < numCities; i++)
                if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
            if (u == -1 || dist[u] == INF) break;
            done[u] = true;

            for (int v = 0; v < numCities; v++) {
                if (road[u][v] > 0 && !done[v] && dist[u] + road[u][v] < dist[v]) {
                    dist[v] = dist[u] + road[u][v];
                    parent[v] = u;
                }
            }
        }

        if (dist[dest] == INF) {
            cout << "  No route from " << cityName[src] << " to " << cityName[dest] << ".\n";
            return -1;
        }
        int path[MAX_CITIES], len = 0;
        for (int c = dest; c != -1; c = parent[c]) path[len++] = c;
        cout << "\n  Shortest route: ";
        for (int i = len - 1; i >= 0; i--) {
            cout << cityName[path[i]];
            if (i > 0) cout << " -> ";
        }
        cout << "\n  Total distance: " << dist[dest] << " km\n";
        return dist[dest];
    }

    // BFS with parent[]: prints the route with the fewest stops and returns the stops, or -1. O(V^2)
    int fewestStops(int src, int dest) {
        bool visited[MAX_CITIES] = {false};
        int parent[MAX_CITIES];
        for (int i = 0; i < numCities; i++) parent[i] = -1;
        queue<int> q;
        visited[src] = true;
        q.push(src);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            if (u == dest) break;
            for (int v = 0; v < numCities; v++) {
                if (road[u][v] > 0 && !visited[v]) {
                    visited[v] = true;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        if (!visited[dest]) {
            cout << "  No route from " << cityName[src] << " to " << cityName[dest] << ".\n";
            return -1;
        }
        int path[MAX_CITIES], len = 0, km = 0;
        for (int c = dest; c != -1; c = parent[c]) {
            path[len++] = c;
            if (parent[c] != -1) km += road[parent[c]][c];
        }
        cout << "\n  Fewest-stops route: ";
        for (int i = len - 1; i >= 0; i--) {
            cout << cityName[path[i]];
            if (i > 0) cout << " -> ";
        }
        cout << "\n  Number of stops: " << len - 1 << " road(s)"
             << "\n  Total distance: " << km << " km\n";
        return len - 1;
    }
};

// Part 6: Menu and main()

ConcertBST concerts;
BookingList bookings;
CancelStack cancelled;
CityGraph cities;

const int NUM_USERS = 5;
User users[NUM_USERS] = {{1, "Aarav"}, {2, "Diya"}, {3, "Kabir"}, {4, "Meera"}, {5, "Rohan"}};

int nextBookingId = 1001;

// Reads a whole number in [low, high], asking again after wrong input. O(1) per try
int readInt(string prompt, int low, int high) {
    int x;
    while (true) {
        cout << prompt;
        cin >> x;
        if (cin.fail()) {
            if (cin.eof()) { cout << "\nInput ended. Goodbye!\n"; exit(0); }
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "  Invalid input! Please enter a number.\n";
        } else {
            cin.ignore(10000, '\n');
            if (x >= low && x <= high) return x;
            cout << "  Please enter a number between " << low << " and " << high << ".\n";
        }
    }
}

// Reads a line of text that is not empty. O(length)
string readText(string prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) { cout << "\nInput ended. Goodbye!\n"; exit(0); }
        if (s != "") return s;
        cout << "  Input cannot be empty.\n";
    }
}

// Linear search for a user ID in the users array, -1 if not found. O(n)
int findUser(int id) {
    for (int i = 0; i < NUM_USERS; i++)
        if (users[i].userId == id) return i;
    return -1;
}

// Shows the users, reads a user ID and returns its index or -1. O(n)
int askUser() {
    cout << "  Users: ";
    for (int i = 0; i < NUM_USERS; i++) cout << users[i].userId << "-" << users[i].name << "  ";
    int idx = findUser(readInt("\n  Enter User ID: ", -1000000, 1000000));
    if (idx == -1) cout << "  User not found.\n";
    return idx;
}

// Reads a concert ID and searches the BST, nullptr if not found. O(h)
Concert* askConcert() {
    Concert* c = concerts.search(readInt("  Enter Concert ID: ", -1000000, 1000000));
    if (c == nullptr) cout << "  Concert not found.\n";
    return c;
}

// Returns all concerts in an array sorted by ID (BST in-order). O(n)
vector<Concert*> concertArray() {
    vector<Concert*> arr;
    concerts.inorderToArray(concerts.root, arr);
    return arr;
}

// Prints a list of concerts as a table. O(n)
void printConcertList(vector<Concert*>& arr) {
    Concert::printHeader();
    for (int i = 0; i < (int)arr.size(); i++) arr[i]->printRow();
}

// Marks the seat booked and adds a new booking to the linked list. O(1)
int makeBooking(int userId, Concert* con, int r, int c) {
    con->seats.booked[r][c] = true;
    bookings.insertEnd(Booking(nextBookingId, userId, con->id, con->name, r, c));
    return nextBookingId++;
}

// Menu 1: shows all concerts in ID order, then one seat map if asked. O(n)
void viewConcerts() {
    cout << "\n  ALL CONCERTS (in-order traversal of BST = sorted by ID)\n";
    Concert::printHeader();
    concerts.inorderDisplay(concerts.root);
    int id = readInt("\n  Enter a Concert ID to see its seat map (0 = back): ", -1000000, 1000000);
    if (id == 0) return;
    Concert* c = concerts.search(id);
    if (c == nullptr) cout << "  Concert not found.\n";
    else c->seats.display();
}

// Menu 2: search by ID (BST), by name (linear) or by ID (binary search).
void searchConcert() {
    cout << "\n  1. By ID   (BST search)\n  2. By name (linear search)\n"
         << "  3. By ID   (binary search on sorted array)\n";
    int ch = readInt("  Choice: ", 1, 3);
    vector<Concert*> arr = concertArray();

    if (ch == 1) {
        Concert* c = askConcert();
        if (c) { Concert::printHeader(); c->printRow(); }
    } else if (ch == 2) {
        string text = readText("  Enter name (or part of it): ");
        vector<Concert*> found = SortSearch::linearSearchByName(arr, text);
        if (found.empty()) cout << "  Concert not found.\n";
        else printConcertList(found);
    } else {
        int id = readInt("  Enter Concert ID: ", -1000000, 1000000);
        int pos = SortSearch::binarySearchById(arr, id);
        if (pos == -1) cout << "  Concert not found.\n";
        else {
            cout << "  Found at index " << pos << " of the sorted array.\n";
            Concert::printHeader();
            arr[pos]->printRow();
        }
    }
}

// True if the day and month of a YYYYMMDD date exist. O(1)
bool validDate(int d) {
    int day = d % 100, month = (d / 100) % 100;
    int daysIn[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month >= 1 && month <= 12 && day >= 1 && day <= daysIn[month];
}

// Menu 3: reads and checks a new concert, then inserts it into the BST. O(h)
void addConcert() {
    int id = readInt("  New Concert ID (1-9999): ", 1, 9999);
    if (concerts.search(id) != nullptr) {
        cout << "  A concert with ID " << id << " already exists.\n";
        return;
    }
    string name = readText("  Concert name: ");
    cities.display();
    int city = readInt("  Choose city number: ", 1, cities.numCities) - 1;

    int date = readInt("  Date (YYYYMMDD, e.g. 20261225): ", 20260101, 20301231);
    while (!validDate(date)) {
        cout << "  That date does not exist.\n";
        date = readInt("  Date (YYYYMMDD): ", 20260101, 20301231);
    }
    int price = readInt("  Ticket price (Rs): ", 1, 100000);

    int rows = readInt("  Rows of seats (max 5): ", 1, 1000);
    while (rows > MAX_ROWS) {
        cout << "  Too many rows! The seat array allows at most 5 rows x 8 seats.\n";
        rows = readInt("  Rows of seats (max 5): ", 1, 1000);
    }
    int cols = readInt("  Seats per row (max 8): ", 1, 1000);
    while (cols > MAX_COLS) {
        cout << "  Too many seats per row! The seat array allows at most 5 rows x 8 seats.\n";
        cols = readInt("  Seats per row (max 8): ", 1, 1000);
    }

    concerts.insert(new Concert(id, name, cities.cityName[city], date, price, rows, cols));
    cout << "  Concert added to the BST.\n";
}

// Menu 4: sorts a copy of the concert list with the chosen key and algorithm.
void sortConcerts() {
    cout << "\n  Sort by: 1. Price  2. Date  3. Popularity (tickets sold)\n";
    int key = readInt("  Choice: ", 1, 3);
    cout << "  Algorithm: 1. Bubble Sort  2. Merge Sort  3. Quick Sort\n";
    int alg = readInt("  Choice: ", 1, 3);

    vector<Concert*> arr = concertArray();
    int n = arr.size();
    if (alg == 1) SortSearch::bubbleSort(arr, key);
    else if (alg == 2) SortSearch::mergeSort(arr, 0, n - 1, key);
    else SortSearch::quickSort(arr, 0, n - 1, key);

    string keyName[] = {"", "price", "date", "popularity"};
    string algName[] = {"", "Bubble Sort", "Merge Sort", "Quick Sort"};
    cout << "\n  Concerts sorted by " << keyName[key] << " using " << algName[alg] << ":\n";
    printConcertList(arr);
}

// Menu 5: books a free seat, or offers the waiting list if sold out. O(h + R*C)
void bookTicket() {
    int u = askUser();
    if (u == -1) return;
    Concert* c = askConcert();
    if (c == nullptr) return;

    if (c->seats.isFull()) {
        cout << "  Sorry, \"" << c->name << "\" is SOLD OUT.\n";
        if (c->waitList.isFull()) {
            cout << "  Waiting list full. Please try another concert.\n";
            return;
        }
        if (readInt("  Join the waiting list? (1 = Yes, 0 = No): ", 0, 1) == 1) {
            c->waitList.enqueue(users[u].userId);
            cout << "  " << users[u].name << " added to the waiting list at position "
                 << c->waitList.count << ".\n";
        }
        return;
    }

    c->seats.display();
    int r, col;
    while (true) {
        r = readInt("  Row number: ", 1, c->seats.rows) - 1;
        col = readInt("  Seat number: ", 1, c->seats.cols) - 1;
        if (c->seats.isFree(r, col)) break;
        cout << "  Seat " << seatLabel(r, col) << " is already booked. Choose another.\n";
    }
    int id = makeBooking(users[u].userId, c, r, col);
    cout << "  Booking confirmed! Booking ID " << id << ", seat " << seatLabel(r, col)
         << ", price Rs " << c->price << ".\n";
}

// Menu 6: cancels a booking, pushes it on the stack, gives the seat to the next waiting user. O(n)
void cancelTicket() {
    int id = readInt("  Enter Booking ID: ", -1000000, 1000000);
    Booking* b = bookings.findById(id);
    if (b == nullptr) {
        cout << "  Invalid Booking ID.\n";
        return;
    }
    if (b->status == "Cancelled") {
        cout << "  Booking " << id << " is already cancelled.\n";
        return;
    }
    Concert* c = concerts.search(b->concertId);
    b->status = "Cancelled";
    c->seats.booked[b->row][b->col] = false;
    cancelled.push(id);
    cout << "  Booking " << id << " cancelled. Seat " << seatLabel(b->row, b->col) << " is free.\n";

    if (!c->waitList.isEmpty()) {
        int nextUser = c->waitList.dequeue();
        int newId = makeBooking(nextUser, c, b->row, b->col);
        cout << "  Seat given to " << users[findUser(nextUser)].name << " (User " << nextUser
             << ") from the waiting list. New Booking ID " << newId << ".\n";
    }
}

// Menu 7: pops the last cancelled booking and restores it if its seat is still free. O(n)
void undoCancellation() {
    if (cancelled.isEmpty()) {
        cout << "  Nothing to undo.\n";
        return;
    }
    int id = cancelled.pop();
    Booking* b = bookings.findById(id);
    Concert* c = concerts.search(b->concertId);
    if (c->seats.isFree(b->row, b->col)) {
        c->seats.booked[b->row][b->col] = true;
        b->status = "Booked";
        cout << "  Undo done: booking " << id << " is active again (seat "
             << seatLabel(b->row, b->col) << ").\n";
    } else {
        cout << "  Cannot undo booking " << id << ": seat " << seatLabel(b->row, b->col)
             << " has already been booked by someone else.\n";
    }
}

// Menu 8: prints every booking of one user from the linked list. O(n)
void viewHistory() {
    int u = askUser();
    if (u == -1) return;
    cout << "\n  Booking history of " << users[u].name << ":\n";
    if (bookings.displayByUser(users[u].userId) == 0) cout << "  No bookings yet.\n";
}

// Menu 9: prints the waiting list of one concert. O(h + n)
void showWaitingList() {
    Concert* c = askConcert();
    if (c == nullptr) return;
    cout << "\n  Waiting list for \"" << c->name << "\":\n";
    c->waitList.display();
}

// Menu 10: shows the city map and runs BFS, DFS, Dijkstra or the fewest-stops BFS. O(V^2)
void routeInfo() {
    cities.display();
    cout << "\n  1. BFS traversal  2. DFS traversal  3. Shortest route (Dijkstra)\n"
         << "  4. Fewest stops (BFS)  0. Back\n";
    int ch = readInt("  Choice: ", 0, 4);
    if (ch == 0) return;
    int src = readInt("  Source city number: ", 1, cities.numCities) - 1;
    if (ch == 1) cities.BFS(src);
    else if (ch == 2) cities.DFS(src);
    else {
        int dest = readInt("  Destination city number: ", 1, cities.numCities) - 1;
        if (ch == 3) cities.shortestPath(src, dest);
        else cities.fewestStops(src, dest);
    }
}

// Loads 8 cities, 7 concerts, 12 bookings and a waiting list for the demo.
void loadSampleData() {
    string names[] = {"Delhi", "Jaipur", "Agra", "Lucknow", "Chandigarh", "Mumbai", "Pune", "Bengaluru"};
    for (int i = 0; i < 8; i++) cities.addCity(names[i]);
    cities.addRoad(0, 1, 280);  cities.addRoad(0, 2, 230);  cities.addRoad(0, 4, 245);
    cities.addRoad(0, 3, 600);  cities.addRoad(1, 2, 240);  cities.addRoad(2, 3, 335);
    cities.addRoad(1, 5, 1150); cities.addRoad(5, 6, 150);  cities.addRoad(6, 7, 840);
    cities.addRoad(5, 7, 1000);

    concerts.insert(new Concert(104, "Melody Night", "Delhi", 20261220, 2500, 5, 8));
    concerts.insert(new Concert(102, "Electro Beach Fest", "Mumbai", 20261205, 3000, 4, 6));
    concerts.insert(new Concert(106, "Fusion Studio Night", "Bengaluru", 20270110, 1800, 5, 6));
    concerts.insert(new Concert(101, "Indie Unplugged", "Pune", 20261215, 800, 2, 3));
    concerts.insert(new Concert(103, "Sufi Evening", "Lucknow", 20270125, 1200, 3, 5));
    concerts.insert(new Concert(105, "Bollywood Beats", "Jaipur", 20261230, 1500, 4, 5));
    concerts.insert(new Concert(107, "Rock the Fort", "Agra", 20270214, 2000, 3, 6));

    Concert* indie = concerts.search(101);
    int who[] = {1, 2, 3, 1, 2, 3};
    for (int i = 0; i < 6; i++) makeBooking(who[i], indie, i / 3, i % 3);
    indie->waitList.enqueue(4);
    indie->waitList.enqueue(5);

    makeBooking(1, concerts.search(104), 0, 0);
    makeBooking(4, concerts.search(104), 0, 1);
    makeBooking(2, concerts.search(102), 1, 2);
    makeBooking(5, concerts.search(102), 1, 3);
    makeBooking(3, concerts.search(102), 0, 0);
    makeBooking(4, concerts.search(105), 2, 1);
}

// Loads the sample data and runs the menu until the user chooses Exit.
int main() {
    loadSampleData();
    while (true) {
        cout << "\n========== CONCERT TICKET BOOKING SYSTEM ==========\n"
             << "  1. View Concerts           7. Undo Last Cancellation\n"
             << "  2. Search Concert          8. View Booking History\n"
             << "  3. Add Concert             9. Show Waiting List\n"
             << "  4. Sort Concerts          10. Route Information\n"
             << "  5. Book Ticket            11. Exit\n"
             << "  6. Cancel Ticket\n";
        int choice = readInt("Enter choice: ", 1, 11);
        switch (choice) {
            case 1: viewConcerts(); break;
            case 2: searchConcert(); break;
            case 3: addConcert(); break;
            case 4: sortConcerts(); break;
            case 5: bookTicket(); break;
            case 6: cancelTicket(); break;
            case 7: undoCancellation(); break;
            case 8: viewHistory(); break;
            case 9: showWaitingList(); break;
            case 10: routeInfo(); break;
            case 11:
                cout << "\nThank you for using the system!\n";
                return 0;
        }
    }
}
