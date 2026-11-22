# Part 5: City Graph (BFS, DFS, Dijkstra, Fewest Stops)

Lines 461 to 619 of `main.cpp`.

## What this part does and why

Concerts happen in different cities, and the "Route Information" menu helps the user
travel between them. A road network is naturally a **graph**:
- **Vertices** are the cities.
- **Edges** are the roads.
- **Weights** are the road lengths in km.

The graph is **undirected**, because every road can be used both ways.

It is stored as an **adjacency matrix**: `road[a][b]` holds the km from city a to city b, and
0 means there is no direct road. With only 8 cities, the matrix is tiny, easy to print and
easy to explain. Checking whether two cities are connected takes O(1).

Four operations are offered:
- **BFS** lists cities level by level: neighbours first, then their neighbours.
- **DFS** goes as deep as possible along one path before backtracking.
- **Dijkstra** finds the shortest route in km and prints the path.
- **Fewest stops** is BFS with a `parent[]` array. It finds the route with the fewest roads
  and prints the path, the number of stops and the total km.

## Constants and data

```cpp
461 | // Part 5: City Graph (weighted, adjacency matrix) with BFS, DFS and Dijkstra
462 |
463 | const int MAX_CITIES = 8;
464 | const int INF = 1000000000;
465 |
466 | class CityGraph {
467 | public:
468 |     int numCities;
469 |     string cityName[MAX_CITIES];
470 |     int road[MAX_CITIES][MAX_CITIES];
```

- `MAX_CITIES = 8` is the size of the matrix.
- `INF` is a very large number that means "no distance known yet" in Dijkstra.
- `cityName[i]` is the name of vertex i, and `numCities` is how many cities have been added.

## Building and showing the graph

```cpp
472 |     // Creates a graph with no cities and no roads (0 means no road). O(V^2)
473 |     CityGraph() : numCities(0) {
474 |         for (int i = 0; i < MAX_CITIES; i++)
475 |             for (int j = 0; j < MAX_CITIES; j++)
476 |                 road[i][j] = 0;
477 |     }
478 |
479 |     // Adds a city as a new vertex. O(1)
480 |     void addCity(string name) {
481 |         if (numCities < MAX_CITIES) cityName[numCities++] = name;
482 |     }
483 |
484 |     // Adds a two-way road of the given length in km. O(1)
485 |     void addRoad(int a, int b, int km) {
486 |         road[a][b] = km;
487 |         road[b][a] = km;
488 |     }
489 |
490 |     // Prints every city with its direct roads. O(V^2)
491 |     void display() {
492 |         cout << "\n  Cities and direct roads:\n";
493 |         for (int i = 0; i < numCities; i++) {
494 |             cout << "  " << i + 1 << ". " << cityName[i] << "  ->  ";
495 |             for (int j = 0; j < numCities; j++)
496 |                 if (road[i][j] > 0) cout << cityName[j] << " (" << road[i][j] << " km)  ";
497 |             cout << "\n";
498 |         }
499 |     }
```

- **Constructor** (lines 473-477): no cities, and every `road[i][j] = 0` (no roads).
- **`addCity(name)`** (lines 480-482): the next free index becomes this city.
- **`addRoad(a, b, km)`** (lines 485-488): sets both `road[a][b]` and `road[b][a]`, because the
  road is two-way.
- **`display()`** (lines 491-499): for each city, scans its row of the matrix and prints
  every city with a value above 0, plus the km.

## BFS (Breadth First Search)

```cpp
501 |     // BFS: visits cities level by level using a queue. O(V^2)
502 |     void BFS(int start) {
503 |         bool visited[MAX_CITIES] = {false};
504 |         int level[MAX_CITIES] = {0};
505 |         queue<int> q;
506 |         visited[start] = true;
507 |         q.push(start);
508 |         cout << "\n  BFS order from " << cityName[start] << ":\n";
509 |         while (!q.empty()) {
510 |             int u = q.front();
511 |             q.pop();
512 |             cout << "  " << cityName[u] << "  (" << level[u] << " road(s) away)\n";
513 |             for (int v = 0; v < numCities; v++) {
514 |                 if (road[u][v] > 0 && !visited[v]) {
515 |                     visited[v] = true;
516 |                     level[v] = level[u] + 1;
517 |                     q.push(v);
518 |                 }
519 |             }
520 |         }
521 |     }
```

1. Mark the start city as visited and put it in a queue. `std::queue` is allowed here; the
   BFS logic itself is written by hand.
2. While the queue is not empty:
   1. Take the front city `u` and print it with its level (how many roads away).
   2. Look at every city `v` in row `u`. If there is a road and `v` is not visited, mark it
      visited, set its level to one more than `u`, and add it to the queue.
3. Cities are marked visited **when they are added**, so no city enters the queue twice.

## DFS (Depth First Search)

```cpp
523 |     // DFS: starts the recursive deep-first visit. O(V^2)
524 |     void DFS(int start) {
525 |         bool visited[MAX_CITIES] = {false};
526 |         cout << "\n  DFS order from " << cityName[start] << ":\n  ";
527 |         dfsVisit(start, visited);
528 |         cout << "\n";
529 |     }
530 |
531 |     // Visits a city, then each unvisited neighbour before backtracking. O(V) per call
532 |     void dfsVisit(int u, bool visited[]) {
533 |         visited[u] = true;
534 |         cout << cityName[u] << "  ";
535 |         for (int v = 0; v < numCities; v++)
536 |             if (road[u][v] > 0 && !visited[v]) dfsVisit(v, visited);
537 |     }
```

**`DFS(start)`** creates the `visited` array and calls `dfsVisit(start)`.

**`dfsVisit(u)`** is recursive.
1. Mark `u` as visited and print it.
2. For every neighbour `v` that is not visited, call `dfsVisit(v)` straight away.
   The program goes deep first. When a city has no unvisited neighbours, the function
   returns; this is **backtracking**.

The recursion works as the stack here.

## Dijkstra's shortest path

```cpp
539 |     // Dijkstra: prints the shortest path and returns its km, or -1 if no route. O(V^2)
540 |     int shortestPath(int src, int dest) {
541 |         int dist[MAX_CITIES], parent[MAX_CITIES];
542 |         bool done[MAX_CITIES];
543 |         for (int i = 0; i < numCities; i++) {
544 |             dist[i] = INF;
545 |             parent[i] = -1;
546 |             done[i] = false;
547 |         }
548 |         dist[src] = 0;
549 |
550 |         for (int round = 0; round < numCities; round++) {
551 |             int u = -1;
552 |             for (int i = 0; i < numCities; i++)
553 |                 if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
554 |             if (u == -1 || dist[u] == INF) break;
555 |             done[u] = true;
556 |
557 |             for (int v = 0; v < numCities; v++) {
558 |                 if (road[u][v] > 0 && !done[v] && dist[u] + road[u][v] < dist[v]) {
559 |                     dist[v] = dist[u] + road[u][v];
560 |                     parent[v] = u;
561 |                 }
562 |             }
563 |         }
564 |
565 |         if (dist[dest] == INF) {
566 |             cout << "  No route from " << cityName[src] << " to " << cityName[dest] << ".\n";
567 |             return -1;
568 |         }
569 |         int path[MAX_CITIES], len = 0;
570 |         for (int c = dest; c != -1; c = parent[c]) path[len++] = c;
571 |         cout << "\n  Shortest route: ";
572 |         for (int i = len - 1; i >= 0; i--) {
573 |             cout << cityName[path[i]];
574 |             if (i > 0) cout << " -> ";
575 |         }
576 |         cout << "\n  Total distance: " << dist[dest] << " km\n";
577 |         return dist[dest];
578 |     }
```

The arrays:
- `dist[i]` is the shortest km found so far from `src` to city i.
- `parent[i]` is the city we came from to reach i. It is used to rebuild the path.
- `done[i]` is true once the shortest distance to i is final.

Steps:
1. Set every `dist` to INF, every `parent` to -1 and every `done` to false. Then set
   `dist[src] = 0`.
2. Repeat once for each city:
   1. Pick the unfinished city `u` with the smallest `dist`. If none is left, or the closest
      is INF (unreachable), stop.
   2. Mark `u` as done.
   3. **Relax** each road `u -> v`: if going through `u` is shorter than the current
      `dist[v]`, update `dist[v]` and set `parent[v] = u`.
3. If `dist[dest]` is still INF, print "No route" and return -1.
4. Otherwise follow `parent[]` back from `dest` to `src` and store the cities in `path`.
   They are in reverse order, so print them from the end. Then print the total km and
   return it.

## Fewest stops (BFS with parent[])

```cpp
580 |     // BFS with parent[]: prints the route with the fewest stops and returns the stops, or -1. O(V^2)
581 |     int fewestStops(int src, int dest) {
582 |         bool visited[MAX_CITIES] = {false};
583 |         int parent[MAX_CITIES];
584 |         for (int i = 0; i < numCities; i++) parent[i] = -1;
585 |         queue<int> q;
586 |         visited[src] = true;
587 |         q.push(src);
588 |         while (!q.empty()) {
589 |             int u = q.front();
590 |             q.pop();
591 |             if (u == dest) break;
592 |             for (int v = 0; v < numCities; v++) {
593 |                 if (road[u][v] > 0 && !visited[v]) {
594 |                     visited[v] = true;
595 |                     parent[v] = u;
596 |                     q.push(v);
597 |                 }
598 |             }
599 |         }
600 |
601 |         if (!visited[dest]) {
602 |             cout << "  No route from " << cityName[src] << " to " << cityName[dest] << ".\n";
603 |             return -1;
604 |         }
605 |         int path[MAX_CITIES], len = 0, km = 0;
606 |         for (int c = dest; c != -1; c = parent[c]) {
607 |             path[len++] = c;
608 |             if (parent[c] != -1) km += road[parent[c]][c];
609 |         }
610 |         cout << "\n  Fewest-stops route: ";
611 |         for (int i = len - 1; i >= 0; i--) {
612 |             cout << cityName[path[i]];
613 |             if (i > 0) cout << " -> ";
614 |         }
615 |         cout << "\n  Number of stops: " << len - 1 << " road(s)"
616 |              << "\n  Total distance: " << km << " km\n";
617 |         return len - 1;
618 |     }
619 | };
```

This is the same BFS as above, with two additions taken from Dijkstra: a `parent[]` array
and the path-rebuilding loop. "Stops" here means the number of roads travelled.

1. Set every `parent` to -1. Mark `src` as visited and put it in the queue.
2. While the queue is not empty, take the front city `u`. If `u` is the destination, stop
   early. Otherwise, for every unvisited neighbour `v`, mark it visited, set
   `parent[v] = u` and add it to the queue.
3. If `dest` was never visited, print "No route" and return -1.
4. Follow `parent[]` back from `dest` to `src`, storing the cities in `path` and adding up
   the km of each road on the way (`road[parent[c]][c]`).
5. Print the path from the end, then the number of stops (`len - 1`, one less than the
   number of cities on the path) and the total km. Return the number of stops.

**Why BFS gives the fewest stops:** BFS takes cities in order of level (0 roads away, then
1, then 2, ...). The first time a city is reached is therefore through the smallest number
of roads, and that is the moment `parent[v]` is set. It is never changed afterwards.

**Difference from Dijkstra:** Dijkstra minimises km and may take more roads; this BFS
minimises roads and may cover more km. It ignores the weights while searching and only
adds the km up at the end.

## Worked example

The sample map has these roads (km):

```
Delhi-Jaipur 280   Delhi-Agra 230    Delhi-Chandigarh 245   Delhi-Lucknow 600
Jaipur-Agra 240    Agra-Lucknow 335  Jaipur-Mumbai 1150     Mumbai-Pune 150
Pune-Bengaluru 840 Mumbai-Bengaluru 1000
```

**BFS from Delhi** (the queue shown after each step):

```
start                  queue: Delhi
take Delhi (level 0)   add Jaipur, Agra, Lucknow, Chandigarh (level 1)
take Jaipur            add Mumbai (level 2)    (Agra already visited)
take Agra, Lucknow, Chandigarh: nothing new
take Mumbai            add Pune, Bengaluru (level 3)
take Pune, Bengaluru   queue empty
Order: Delhi, Jaipur, Agra, Lucknow, Chandigarh, Mumbai, Pune, Bengaluru
```

**DFS from Delhi**: Delhi, then Jaipur, then Agra, then Lucknow. Lucknow is a dead end, so
backtrack to Jaipur. Then Mumbai, then Pune, then Bengaluru. Backtrack all the way to
Delhi, then visit Chandigarh.

Order: Delhi, Jaipur, Agra, Lucknow, Mumbai, Pune, Bengaluru, Chandigarh.

**Dijkstra from Delhi to Lucknow**

| Round | Picked city (dist) | Updates |
|---|---|---|
| 1 | Delhi (0) | Jaipur 280, Agra 230, Lucknow 600, Chandigarh 245 |
| 2 | Agra (230) | Lucknow: 230 + 335 = 565 < 600, so it becomes 565 (parent Agra) |
| 3 | Chandigarh (245) | nothing new |
| 4 | Jaipur (280) | Mumbai 1430 |
| 5 | Lucknow (565) | final |

Result: **Delhi -> Agra -> Lucknow, 565 km**. This beats the direct 600 km road.
The same steps give Delhi to Bengaluru as Delhi -> Jaipur -> Mumbai -> Pune -> Bengaluru,
2420 km. The route through Pune (1580 + 840) beats the direct Mumbai-Bengaluru road
(1430 + 1000 = 2430).

**Fewest stops from Delhi to Lucknow**

BFS from Delhi adds Jaipur, Agra, Lucknow and Chandigarh at level 1, each with
`parent = Delhi`. Lucknow is reached on the first level, so the rebuilt path is
**Delhi -> Lucknow: 1 stop, 600 km**. Compare with Dijkstra: 2 stops through Agra, but only
565 km.

Delhi to Bengaluru: Mumbai is reached from Jaipur (level 2). Mumbai then adds both Pune and
Bengaluru (level 3) with `parent = Mumbai`, so the road through Pune is never used. Result:
**Delhi -> Jaipur -> Mumbai -> Bengaluru: 3 stops, 280 + 1150 + 1000 = 2430 km**. Dijkstra's
route has 4 stops but is 10 km shorter.

## Time complexity

V is the number of cities.

| Operation | Time | Why |
|---|---|---|
| `addCity`, `addRoad` | O(1) | direct array write |
| constructor, `display` | O(V^2) | the whole matrix is visited |
| BFS | O(V^2) | each city is taken once, and its whole row (V entries) is scanned |
| DFS | O(V^2) | same reason |
| Dijkstra | O(V^2) | V rounds, each with an O(V) search for the closest city and O(V) relaxing |
| Fewest stops (BFS) | O(V^2) | same as BFS; rebuilding the path is only O(V) more |

With an adjacency **list** instead, BFS and DFS would be O(V + E). For 8 cities the
matrix is simpler.

## Viva questions

1. **Why did you use an adjacency matrix and not an adjacency list?**
   There are only 8 cities, so the matrix is small and simple, and checking for a road is O(1).
   A list saves memory for big, sparse graphs.
2. **What is the difference between BFS and DFS?**
   BFS uses a queue and visits level by level (nearest in number of roads first). DFS uses
   recursion (a stack) and goes deep along one path before backtracking.
3. **Why does BFS not give the shortest route in km?**
   BFS counts roads, not km. Delhi to Lucknow is 1 road (600 km), but the shortest distance
   is 2 roads through Agra (565 km). Dijkstra uses the weights. That is why the menu has both:
   "Fewest stops (BFS)" gives the direct road and "Shortest route (Dijkstra)" gives the route
   via Agra.
4. **What does "relaxing" an edge mean?**
   It means checking whether reaching v through u is shorter than the best known distance
   to v, and updating `dist[v]` and `parent[v]` if it is.
5. **How is the path printed?**
   `parent[]` stores where we came from. We follow it back from the destination to the
   source, then print in reverse. Dijkstra and the fewest-stops BFS use exactly the same
   idea; only the way `parent[]` gets filled is different.
6. **Can a parent change in the fewest-stops BFS?**
   No. A city gets its parent when it is first visited, and that is already through the
   fewest roads. In Dijkstra a parent can change many times while relaxing.
