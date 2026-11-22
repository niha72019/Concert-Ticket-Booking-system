# Part 3: Concerts and Binary Search Tree

Lines 235 to 350 of `main.cpp`.

## What this part does and why

The `Concert` class holds everything about one event:
- its ID, name, city, date and price
- its own seat map (Part 1)
- its own waiting list (Part 2)

All concerts are stored in a **Binary Search Tree (BST)**, using the concert ID as the key.
In every node, smaller IDs go to the **left** subtree and bigger IDs go to the **right** subtree.

Why a BST?
- **Searching:** each comparison skips a whole subtree, so a search follows only one path
  from the root instead of checking every concert.
- **Sorted listing:** an **in-order traversal** (left, node, right) gives the concerts in
  increasing ID order without any extra sorting. This sorted list is later used for
  binary search (Part 4).

## formatDate

```cpp
235 | // Part 3: Concerts and Binary Search Tree (key = concert ID)
236 |
237 | // Turns a YYYYMMDD number into "DD-MM-YYYY". O(1)
238 | string formatDate(int d) {
239 |     stringstream ss;
240 |     ss << setfill('0') << setw(2) << d % 100 << "-" << setw(2) << (d / 100) % 100
241 |        << "-" << d / 10000;
242 |     return ss.str();
243 | }
```

Dates are stored as one number in YYYYMMDD form, for example 20261220. This makes comparing
and sorting dates easy, because a later date is simply a bigger number.

`formatDate` breaks the number into parts:
- `d % 100` is the day.
- `(d / 100) % 100` is the month.
- `d / 10000` is the year.

`setfill('0')` and `setw(2)` print 5 as "05". So `formatDate(20261220)` gives "20-12-2026".

## The Concert class

```cpp
245 | class Concert {
246 | public:
247 |     int id;
248 |     string name;
249 |     string city;
250 |     int date;
251 |     int price;
252 |     SeatMap seats;
253 |     CircularQueue waitList;
254 |
255 |     // Creates a concert with an empty seat map and waiting list. O(R*C)
256 |     Concert(int i, string n, string c, int d, int p, int rows, int cols)
257 |         : id(i), name(n), city(c), date(d), price(p), seats(rows, cols) {}
258 |
259 |     // Popularity: number of tickets sold. O(R*C)
260 |     int ticketsSold() { return seats.countBooked(); }
261 |
262 |     // Prints the column titles of the concert table. O(1)
263 |     static void printHeader() {
264 |         cout << "  " << left << setw(6) << "ID" << setw(24) << "Name" << setw(12) << "City"
265 |              << setw(12) << "Date" << setw(8) << "Price" << setw(6) << "Sold"
266 |              << setw(6) << "Free" << "Waiting" << right << "\n  " << string(80, '-') << "\n";
267 |     }
268 |
269 |     // Prints this concert as one table row. O(R*C)
270 |     void printRow() {
271 |         cout << "  " << left << setw(6) << id << setw(24) << name << setw(12) << city
272 |              << setw(12) << formatDate(date) << setw(8) << price << setw(6) << ticketsSold()
273 |              << setw(6) << seats.rows * seats.cols - ticketsSold() << waitList.count
274 |              << (seats.isFull() ? "  SOLD OUT" : "") << right << "\n";
275 |     }
276 | };
```

- **Constructor** (lines 256-257): copies the values in and creates a seat map of size
  rows x cols. The waiting list starts empty by its own constructor.
- **`ticketsSold()`** (line 260): asks the seat map how many seats are booked. This is the
  concert's popularity.
- **`printHeader()`** (lines 263-267): prints the column titles. It is `static` because it
  does not need any particular concert.
- **`printRow()`** (lines 270-275): prints one concert as a table row. "Free" is
  rows x cols minus sold, "Waiting" is the queue count, and "SOLD OUT" is added when the
  hall is full.

## BSTNode

```cpp
278 | class BSTNode {
279 | public:
280 |     Concert* data;
281 |     BSTNode* left;
282 |     BSTNode* right;
283 |
284 |     // Creates a tree node with no children. O(1)
285 |     BSTNode(Concert* c) : data(c), left(nullptr), right(nullptr) {}
286 | };
```

Each node holds a pointer to one concert and pointers to its left and right children.
A new node has no children (`nullptr`).

## ConcertBST: constructor and freeing memory

```cpp
288 | class ConcertBST {
289 | public:
290 |     BSTNode* root;
291 |     int count;
292 |
293 |     // Creates an empty tree. O(1)
294 |     ConcertBST() : root(nullptr), count(0) {}
295 |
296 |     // Frees all nodes and concerts when the program ends. O(n)
297 |     ~ConcertBST() { destroy(root); }
298 |
299 |     // Post-order delete: children first, then the node. O(n)
300 |     void destroy(BSTNode* node) {
301 |         if (node == nullptr) return;
302 |         destroy(node->left);
303 |         destroy(node->right);
304 |         delete node->data;
305 |         delete node;
306 |     }
```

- **Constructor** (line 294): empty tree, so root is `nullptr` and count is 0.
- **Destructor** (line 297): calls `destroy(root)` when the program ends.
- **`destroy(node)`** (lines 300-306): a **post-order** traversal.
  1. Delete the left subtree.
  2. Delete the right subtree.
  3. Delete this node's concert, then the node itself.

  Children must go first, because after deleting a node we could no longer reach them.

## Insert

```cpp
308 |     // Inserts a concert, returns false if the ID already exists. O(h)
309 |     bool insert(Concert* c) {
310 |         if (search(c->id) != nullptr) return false;
311 |         root = insertNode(root, c);
312 |         count++;
313 |         return true;
314 |     }
315 |
316 |     // Goes left for a smaller ID, right for a bigger one, until an empty spot. O(h)
317 |     BSTNode* insertNode(BSTNode* node, Concert* c) {
318 |         if (node == nullptr) return new BSTNode(c);
319 |         if (c->id < node->data->id) node->left = insertNode(node->left, c);
320 |         else node->right = insertNode(node->right, c);
321 |         return node;
322 |     }
```

**`insert(c)`** (lines 309-314)
1. Search for the ID first. If it already exists, return false (IDs must be unique).
2. Otherwise call `insertNode` starting at the root, and add 1 to count.

**`insertNode(node, c)`** (lines 317-322): a recursive function.
1. If `node` is `nullptr`, we found the empty spot: create the new node and return it.
2. If the new ID is smaller, insert into the left subtree and store the result in
   `node->left`.
3. Otherwise insert into the right subtree.
4. Return `node`, so the parent's link stays the same.

## Search

```cpp
324 |     // Finds a concert by ID, each step skips one subtree. O(h)
325 |     Concert* search(int id) {
326 |         BSTNode* cur = root;
327 |         while (cur != nullptr) {
328 |             if (id == cur->data->id) return cur->data;
329 |             if (id < cur->data->id) cur = cur->left;
330 |             else cur = cur->right;
331 |         }
332 |         return nullptr;
333 |     }
```

**`search(id)`** (lines 325-333)
1. Start at the root.
2. If the ID matches, return this concert.
3. If the ID is smaller, go left; if bigger, go right.
4. If we fall off the tree (`nullptr`), the concert does not exist, so return `nullptr`.

## Traversals

```cpp
335 |     // In-order traversal (left, node, right) prints concerts in ID order. O(n)
336 |     void inorderDisplay(BSTNode* node) {
337 |         if (node == nullptr) return;
338 |         inorderDisplay(node->left);
339 |         node->data->printRow();
340 |         inorderDisplay(node->right);
341 |     }
342 |
343 |     // In-order copy into an array, so the array is sorted by ID. O(n)
344 |     void inorderToArray(BSTNode* node, vector<Concert*>& arr) {
345 |         if (node == nullptr) return;
346 |         inorderToArray(node->left, arr);
347 |         arr.push_back(node->data);
348 |         inorderToArray(node->right, arr);
349 |     }
350 | };
```

**`inorderDisplay(node)`** (lines 336-341)
1. Visit the left subtree.
2. Print this concert.
3. Visit the right subtree.

Left always has smaller IDs, so the concerts come out in increasing ID order.

**`inorderToArray(node, arr)`** (lines 344-349): the same traversal, but instead of
printing it adds each concert to a vector. The vector is therefore **sorted by ID**.
It is used for sorting (a copy, so the tree is never changed) and for binary search.

## Worked example

The sample data inserts the IDs in the order 104, 102, 106, 101, 103, 105, 107:

```
            104
          /     \
       102       106
      /   \     /   \
    101   103 105   107
```

- **Insert 104:** the tree is empty, so 104 becomes the root.
- **Insert 102:** 102 < 104, so it goes left of 104.
- **Insert 106:** 106 > 104, so it goes right of 104.
- **Insert 101:** 101 < 104, so go left; 101 < 102, so it goes left of 102. The rest are placed the same way.
- **search(105):**
  - 105 > 104, so go right.
  - 105 < 106, so go left.
  - 105 is found after **3 comparisons** instead of up to 7.
- **search(108):**
  - 104, then 106, then 107.
  - The right child of 107 is `nullptr`, so the result is "not found".
- **Add concert 108:** it follows the same path and becomes the right child of 107.
- **In-order:** 101, 102, 103, 104, 105, 106, 107.

If the IDs were inserted already sorted (101, 102, 103, ...), every node would go to the right,
and the tree would become a chain like a linked list. That is the worst case.

## Time complexity

n is the number of concerts. h is the height of the tree: about log n when the tree is
balanced, and n in the worst case.

| Operation | Time |
|---|---|
| `insert` / `insertNode` | O(h) |
| `search` | O(h) |
| `inorderDisplay`, `inorderToArray` | O(n) |
| `destroy` | O(n) |
| `formatDate` | O(1) |
| `ticketsSold`, `printRow` | O(R x C) (they count the seats) |

## Viva questions

1. **What is the BST property?**
   For every node, all IDs in the left subtree are smaller and all IDs in the right subtree
   are bigger.
2. **What is the time complexity of searching a BST?**
   O(h), where h is the height. That is O(log n) for a balanced tree and O(n) in the worst
   case (sorted insertions make a chain).
3. **Why does in-order traversal give sorted output?**
   It visits left (smaller), then the node, then right (bigger), so every ID comes before
   all bigger IDs.
4. **How are duplicate concert IDs handled?**
   `insert` searches first and returns false if the ID exists, and Add Concert shows
   "already exists".
5. **Why is post-order used to delete the tree?**
   The children must be deleted before their parent. Otherwise we would lose the pointers
   to them and leak memory.
