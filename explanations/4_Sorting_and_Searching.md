# Part 4: Sorting and Searching

Lines 352 to 459 of `main.cpp`.

## What this part does and why

All functions are `static` members of one class, `SortSearch`, so they are called like
`SortSearch::bubbleSort(arr, key)`. They work on a `vector<Concert*>`, which is an array of
pointers. Sorting only moves the pointers; the concerts themselves stay where they are.

**Sorting.** The user can sort concerts by price, date or popularity with three
algorithms. This lets us compare a simple O(n^2) method with two O(n log n) methods. We
always sort a **copy** of the list taken from the BST, so the tree itself never changes.

**Searching.** Two methods are used:
- **Linear search** for names, because names are not in sorted order and the user can type
  only part of a name in any case.
- **Binary search** for IDs, on the array produced by the BST in-order traversal, which is
  already sorted by ID.

## Choosing the sort key

```cpp
352 | // Part 4: Sorting (bubble, merge, quick) and Searching (linear, binary)
353 |
354 | class SortSearch {
355 | public:
356 |     // Value to sort by: 1 price, 2 date, 3 popularity (negative so most sold comes first). O(R*C)
357 |     static int keyValue(Concert* c, int key) {
358 |         if (key == 1) return c->price;
359 |         if (key == 2) return c->date;
360 |         return -c->ticketsSold();
361 |     }
```

`keyValue(c, key)` returns the number to compare:
- key 1 is the price.
- key 2 is the date (YYYYMMDD, so a later date is a bigger number).
- key 3 is **minus** the tickets sold.

All three algorithms sort from small to big. Using the negative value for popularity means
the concert with the most tickets sold comes first. For example, 6 sold gives -6, which
is smaller than -3.

## Bubble sort

```cpp
363 |     // Bubble sort, stops early if a pass makes no swap. O(n^2), best O(n)
364 |     static void bubbleSort(vector<Concert*>& a, int key) {
365 |         int n = a.size();
366 |         for (int pass = 0; pass < n - 1; pass++) {
367 |             bool swapped = false;
368 |             for (int j = 0; j < n - 1 - pass; j++) {
369 |                 if (keyValue(a[j], key) > keyValue(a[j + 1], key)) {
370 |                     Concert* temp = a[j];
371 |                     a[j] = a[j + 1];
372 |                     a[j + 1] = temp;
373 |                     swapped = true;
374 |                 }
375 |             }
376 |             if (!swapped) break;
377 |         }
378 |     }
```

1. Make passes over the array. In each pass, compare neighbours `a[j]` and `a[j+1]`.
2. If they are in the wrong order, swap them using a `temp` variable.
3. After each pass the biggest remaining item has "bubbled" to the end, so the inner loop
   can stop one place earlier (`n - 1 - pass`).
4. If a whole pass makes no swap (`swapped` is false), the array is already sorted, so stop
   early.

## Merge sort

```cpp
380 |     // Merge sort: split in half, sort both halves, merge. O(n log n)
381 |     static void mergeSort(vector<Concert*>& a, int left, int right, int key) {
382 |         if (left >= right) return;
383 |         int mid = (left + right) / 2;
384 |         mergeSort(a, left, mid, key);
385 |         mergeSort(a, mid + 1, right, key);
386 |         merge(a, left, mid, right, key);
387 |     }
388 |
389 |     // Merges the sorted halves a[left..mid] and a[mid+1..right]. O(n)
390 |     static void merge(vector<Concert*>& a, int left, int mid, int right, int key) {
391 |         vector<Concert*> temp;
392 |         int i = left, j = mid + 1;
393 |         while (i <= mid && j <= right) {
394 |             if (keyValue(a[i], key) <= keyValue(a[j], key)) temp.push_back(a[i++]);
395 |             else temp.push_back(a[j++]);
396 |         }
397 |         while (i <= mid) temp.push_back(a[i++]);
398 |         while (j <= right) temp.push_back(a[j++]);
399 |         for (int k = 0; k < (int)temp.size(); k++) a[left + k] = temp[k];
400 |     }
```

**`mergeSort(a, left, right, key)`**: divide and conquer.
1. If the part has 0 or 1 items (`left >= right`), it is already sorted.
2. Find the middle, then sort the left half and the right half recursively.
3. Merge the two sorted halves.

**`merge(...)`**
1. `i` walks the left half, `j` walks the right half.
2. Copy the smaller of `a[i]` and `a[j]` into `temp`. Using `<=` keeps equal items in their
   old order, which makes the sort **stable**.
3. When one half runs out, copy the rest of the other half.
4. Copy `temp` back into `a[left..right]`.

## Quick sort

```cpp
402 |     // Quick sort around a pivot. Average O(n log n), worst O(n^2)
403 |     static void quickSort(vector<Concert*>& a, int low, int high, int key) {
404 |         if (low >= high) return;
405 |         int p = partition(a, low, high, key);
406 |         quickSort(a, low, p - 1, key);
407 |         quickSort(a, p + 1, high, key);
408 |     }
409 |
410 |     // Uses the last element as pivot and moves smaller ones to its left. O(n)
411 |     static int partition(vector<Concert*>& a, int low, int high, int key) {
412 |         int pivot = keyValue(a[high], key);
413 |         int i = low - 1;
414 |         for (int j = low; j < high; j++) {
415 |             if (keyValue(a[j], key) < pivot) {
416 |                 i++;
417 |                 Concert* temp = a[i]; a[i] = a[j]; a[j] = temp;
418 |             }
419 |         }
420 |         Concert* temp = a[i + 1]; a[i + 1] = a[high]; a[high] = temp;
421 |         return i + 1;
422 |     }
```

**`quickSort(a, low, high, key)`**
1. If the part has 0 or 1 items, stop.
2. `partition` puts the pivot in its final place `p`.
3. Sort the part left of `p` and the part right of `p`.

**`partition(...)`**
1. The pivot is the last element, `a[high]`.
2. `i` marks the end of the "smaller than pivot" area. It starts at `low - 1`, which means
   empty.
3. For each `j` from low to high-1, if `a[j]` is smaller than the pivot, grow the area
   (`i++`) and swap `a[j]` into it.
4. Finally swap the pivot to position `i + 1`, just after the smaller items, and return
   that position.

## Linear search by name

```cpp
424 |     // Converts one capital letter to lower case. O(1)
425 |     static char lower(char ch) {
426 |         return (ch >= 'A' && ch <= 'Z') ? ch - 'A' + 'a' : ch;
427 |     }
428 |
429 |     // True if text appears inside name, ignoring case. O(name length x text length)
430 |     static bool containsText(string name, string text) {
431 |         int n = name.size(), m = text.size();
432 |         for (int start = 0; start + m <= n; start++) {
433 |             int k = 0;
434 |             while (k < m && lower(name[start + k]) == lower(text[k])) k++;
435 |             if (k == m) return true;
436 |         }
437 |         return false;
438 |     }
439 |
440 |     // Linear search: checks every concert name one by one. O(n)
441 |     static vector<Concert*> linearSearchByName(vector<Concert*>& a, string text) {
442 |         vector<Concert*> found;
443 |         for (int i = 0; i < (int)a.size(); i++)
444 |             if (containsText(a[i]->name, text)) found.push_back(a[i]);
445 |         return found;
446 |     }
```

- **`lower(ch)`** turns 'A'-'Z' into 'a'-'z', so "NIGHT" matches "Night".
- **`containsText(name, text)`** is a hand-written check of whether `text` appears anywhere
  in `name`.
  1. Try every starting position in `name`.
  2. Compare characters one by one, ignoring case.
  3. If all `m` characters matched, return true.
- **`linearSearchByName(a, text)`** checks every concert one by one and collects all
  matches. More than one concert can match.

## Binary search by ID

```cpp
448 |     // Binary search by ID on an array sorted by ID, returns index or -1. O(log n)
449 |     static int binarySearchById(vector<Concert*>& a, int id) {
450 |         int low = 0, high = (int)a.size() - 1;
451 |         while (low <= high) {
452 |             int mid = (low + high) / 2;
453 |             if (a[mid]->id == id) return mid;
454 |             if (a[mid]->id < id) low = mid + 1;
455 |             else high = mid - 1;
456 |         }
457 |         return -1;
458 |     }
459 | };
```

The array **must be sorted by ID**. It is, because it comes from the BST in-order traversal.

1. `low` and `high` mark the part of the array that can still contain the ID.
2. Look at the middle element.
3. If it matches, return its index.
4. If the middle ID is smaller, the answer can only be on the right, so `low = mid + 1`.
5. If it is bigger, the answer can only be on the left, so `high = mid - 1`.
6. If `low` passes `high`, the ID is not there, so return -1.

Each step throws away half of the remaining array.

## Worked example

Take four concerts with these prices: 101: 800, 102: 3000, 103: 1200, 104: 2500. The array is
`[800, 3000, 1200, 2500]`.

**Bubble sort by price**

```
Pass 1: 800,3000 ok | 3000>1200 swap -> [800,1200,3000,2500] | 3000>2500 swap -> [800,1200,2500,3000]
Pass 2: 800,1200 ok | 1200,2500 ok | no swaps -> stop early
```

**Merge sort**

```
[800,3000,1200,2500] -> split -> [800,3000] and [1200,2500] (each already sorted)
merge: 800 vs 1200 -> 800 | 3000 vs 1200 -> 1200 | 3000 vs 2500 -> 2500 | then 3000
result [800,1200,2500,3000]
```

**Quick sort** (the pivot is the last element)

```
partition [800,3000,1200,2500], pivot 2500, i = -1
 j=0: 800 < 2500  -> i=0, swap a[0],a[0]   [800,3000,1200,2500]
 j=1: 3000 not smaller
 j=2: 1200 < 2500 -> i=1, swap a[1],a[2]   [800,1200,3000,2500]
 place pivot: swap a[2],a[3]               [800,1200,2500,3000], pivot index 2
left part [800,1200]: pivot 1200 is already in place. Right part [3000] has one item.
```

**Binary search** on the 7 sample IDs `[101,102,103,104,105,106,107]` (indexes 0 to 6)

```
search 106: low=0 high=6 mid=3 (104 < 106) -> low=4
            low=4 high=6 mid=5 (106)       -> found at index 5
search 999: mid=3 (104) -> low=4 | mid=5 (106) -> low=6 | mid=6 (107) -> low=7 > high -> -1
```

**Linear search** for "night": every concert is checked, and Melody Night (104) and Fusion
Studio Night (106) match.

## Time complexity

| Algorithm | Best | Average | Worst | Extra space |
|---|---|---|---|---|
| Bubble sort | O(n) (already sorted) | O(n^2) | O(n^2) | O(1) |
| Merge sort | O(n log n) | O(n log n) | O(n log n) | O(n) |
| Quick sort | O(n log n) | O(n log n) | O(n^2) (e.g. already sorted, last element as pivot) | O(log n) recursion |
| Linear search | O(1) | O(n) | O(n) | O(1) |
| Binary search | O(1) | O(log n) | O(log n) | O(1) |

## Viva questions

1. **Why can binary search not be used on names?**
   Binary search needs sorted data and an exact key. Names are not sorted, and we allow
   partial, case-insensitive matches, so we check each one (linear search).
2. **Where does the sorted array for binary search come from?**
   From the in-order traversal of the BST, which always gives concerts in increasing ID order.
3. **Which sort is stable and why does it matter?**
   Merge sort, because it uses `<=` while merging. Concerts with equal prices keep their
   previous (ID) order.
4. **When does quick sort become O(n^2)?**
   When the pivot is always the smallest or largest item, for example on an already sorted
   array when the last element is the pivot. The parts are then split as n-1 and 0.
5. **How do you sort by popularity with the most popular first?**
   `keyValue` returns minus the tickets sold, so the same ascending sort puts the biggest
   seller first.
