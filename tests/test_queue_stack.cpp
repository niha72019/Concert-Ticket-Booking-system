// Test for Part 2 of main.cpp: circular queue and stack
// From the project folder:
//   g++ -std=c++17 tests/test_queue_stack.cpp -o tests/t2 && tests/t2
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
    CircularQueue q;
    check(q.isEmpty() && !q.isFull() && q.dequeue() == -1, "new queue empty, dequeue gives -1");
    for (int u = 1; u <= WAIT_SIZE; u++) q.enqueue(u);
    check(q.isFull() && !q.enqueue(99), "queue full after 5, 6th rejected");
    check(q.dequeue() == 1 && q.dequeue() == 2, "FIFO order: 1 then 2");
    check(q.enqueue(6) && q.enqueue(7), "freed slots reused");
    check(q.rear == 1, "rear wrapped around to index 1");
    q.display();
    int order[] = {3, 4, 5, 6, 7};
    bool ok = true;
    for (int i = 0; i < 5; i++) if (q.dequeue() != order[i]) ok = false;
    check(ok && q.isEmpty(), "dequeue order 3,4,5,6,7 across the wrap");
    q.display();

    CancelStack s;
    check(s.isEmpty() && s.pop() == -1, "new stack empty, pop gives -1");
    s.push(1001); s.push(1002); s.push(1003);
    check(s.pop() == 1003 && s.pop() == 1002, "LIFO order: 1003 then 1002");
    check(s.pop() == 1001 && s.isEmpty(), "stack empty again");
    for (int i = 0; i < STACK_SIZE; i++) s.push(i);
    check(s.isFull() && !s.push(5), "stack full rejects push");

    cout << (fails ? "\nSOME TESTS FAILED\n" : "\nALL TESTS PASSED\n");
    return fails;
}
