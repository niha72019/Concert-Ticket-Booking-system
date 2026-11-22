# Part 2: Circular Queue (Waiting List) and Stack (Undo Cancellation)

Lines 154 to 233 of `main.cpp`.

## What this part does and why

**Circular queue (waiting list).** When a concert is sold out, users can join its waiting
list. The first person to join must be the first to get a freed seat. That is
**First In, First Out (FIFO)**, which is what a queue does.

In a simple array queue, the places at the front are wasted after people leave. A
**circular** queue fixes this: when `rear` reaches the end of the array it wraps back to
index 0, so freed places are used again. The array has a fixed size of 5, which also gives
a natural "Waiting list full" limit. Each concert has its own queue, and the queue stores
user IDs.

**Stack (undo).** Every cancelled booking ID is pushed onto a stack. "Undo Last
Cancellation" must undo the **most recent** cancellation first. That is **Last In, First
Out (LIFO)**, which is what a stack does.

## Constants

```cpp
154 | // Part 2: Circular Queue (Waiting List) and Stack (Undo Cancellation)
155 |
156 | const int WAIT_SIZE = 5;
157 | const int STACK_SIZE = 100;
```

- `WAIT_SIZE = 5`: at most 5 people can wait for one concert.
- `STACK_SIZE = 100`: the stack can remember up to 100 cancellations.

## The CircularQueue class

```cpp
159 | class CircularQueue {
160 | public:
161 |     int items[WAIT_SIZE];
162 |     int front, rear, count;
163 |
164 |     // Creates an empty queue. O(1)
165 |     CircularQueue() : front(0), rear(-1), count(0) {}
166 |
167 |     // True if nobody is waiting. O(1)
168 |     bool isEmpty() { return count == 0; }
169 |
170 |     // True if all 5 places are used. O(1)
171 |     bool isFull() { return count == WAIT_SIZE; }
172 |
173 |     // Adds a user at the rear, wrapping to index 0 after the end. O(1)
174 |     bool enqueue(int userId) {
175 |         if (isFull()) return false;
176 |         rear = (rear + 1) % WAIT_SIZE;
177 |         items[rear] = userId;
178 |         count++;
179 |         return true;
180 |     }
181 |
182 |     // Removes and returns the user at the front, or -1 if empty. O(1)
183 |     int dequeue() {
184 |         if (isEmpty()) return -1;
185 |         int userId = items[front];
186 |         front = (front + 1) % WAIT_SIZE;
187 |         count--;
188 |         return userId;
189 |     }
190 |
191 |     // Prints the waiting users from front to rear. O(n)
192 |     void display() {
193 |         if (isEmpty()) {
194 |             cout << "  Waiting list is empty.\n";
195 |             return;
196 |         }
197 |         cout << "  (front index = " << front << ", rear index = " << rear
198 |              << ", " << count << "/" << WAIT_SIZE << " used)\n";
199 |         for (int i = 0; i < count; i++) {
200 |             int index = (front + i) % WAIT_SIZE;
201 |             cout << "  Position " << i + 1 << ": User " << items[index]
202 |                  << "   [array slot " << index << "]\n";
203 |         }
204 |     }
205 | };
```

The queue uses three variables:
- `front` is the index of the first person in line.
- `rear` is the index of the last person in line.
- `count` is how many people are waiting.

**Constructor** (line 165): `front = 0`, `rear = -1` (nobody yet), `count = 0`.

**`isEmpty()` / `isFull()`** (lines 168, 171): compare `count` with 0 and with 5. Keeping
`count` makes these checks simple and clear.

**`enqueue(userId)`** (lines 174-180)
1. If the queue is full, return false (the menu then prints "Waiting list full").
2. Move rear forward by one: `rear = (rear + 1) % WAIT_SIZE`.
   The `%` (remainder) is what makes it circular: after index 4 comes index 0.
3. Store the user ID at `items[rear]`.
4. Add 1 to count and return true.

**`dequeue()`** (lines 183-189)
1. If the queue is empty, return -1.
2. Remember the user at `items[front]`.
3. Move front forward with the same wrap-around: `front = (front + 1) % WAIT_SIZE`.
4. Subtract 1 from count and return the user ID.
The old slot is not cleared. It is simply outside the front-to-rear range now, so it can be
reused.

**`display()`** (lines 192-204)
1. If empty, print "Waiting list is empty."
2. Print front, rear and how many places are used.
3. For i = 0 to count-1, the real array index is `(front + i) % WAIT_SIZE`. Print the
   position in line, the user ID and the array slot, which shows the wrap-around.

## The CancelStack class

```cpp
207 | class CancelStack {
208 | public:
209 |     int items[STACK_SIZE];
210 |     int top;
211 |
212 |     // Creates an empty stack (top = -1). O(1)
213 |     CancelStack() { top = -1; }
214 |
215 |     // True if the stack has no items. O(1)
216 |     bool isEmpty() { return top == -1; }
217 |
218 |     // True if the array is full. O(1)
219 |     bool isFull() { return top == STACK_SIZE - 1; }
220 |
221 |     // Puts a cancelled booking ID on top. O(1)
222 |     bool push(int bookingId) {
223 |         if (isFull()) return false;
224 |         items[++top] = bookingId;
225 |         return true;
226 |     }
227 |
228 |     // Removes and returns the top booking ID, or -1 if empty. O(1)
229 |     int pop() {
230 |         if (isEmpty()) return -1;
231 |         return items[top--];
232 |     }
233 | };
```

`top` is the index of the top item. It is -1 when the stack is empty.

**`isEmpty()`** is true when top is -1, and **`isFull()`** is true when top is 99.

**`push(bookingId)`** (lines 222-226)
1. If full, return false.
2. `items[++top] = bookingId`: first increase top, then store the ID there.

**`pop()`** (lines 229-232)
1. If empty, return -1.
2. `return items[top--]`: return the top item, then decrease top.

## Worked example

**Queue.** WAIT_SIZE = 5. A `_` means an unused slot.

| Step | items[0..4] | front | rear | count | Returns |
|---|---|---|---|---|---|
| start | `_ _ _ _ _` | 0 | -1 | 0 | |
| enqueue(4) | `4 _ _ _ _` | 0 | 0 | 1 | true |
| enqueue(5) | `4 5 _ _ _` | 0 | 1 | 2 | true |
| enqueue(1) | `4 5 1 _ _` | 0 | 2 | 3 | true |
| dequeue() | `4 5 1 _ _` | 1 | 2 | 2 | 4 (first in, first out) |
| enqueue(2) | `4 5 1 2 _` | 1 | 3 | 3 | true |
| enqueue(3) | `4 5 1 2 3` | 1 | 4 | 4 | true |
| enqueue(7) | `7 5 1 2 3` | 1 | 0 | 5 | true: rear wrapped to slot 0 |
| enqueue(8) | no change | 1 | 0 | 5 | false: "Waiting list full" |

After the 3 enqueues and 1 dequeue, the line is 5, 1 (front = 1, rear = 2). Slot 0 still holds
the old 4, but it is outside the line and is overwritten later by 7.

**Stack.** Cancel booking 1001, then booking 1013, then undo:

```
push(1001)   [1001]           top = 0
push(1013)   [1001, 1013]     top = 1
pop()        returns 1013     top = 0   (most recent cancellation is undone first)
```

## Time complexity

| Operation | Time |
|---|---|
| `enqueue`, `dequeue`, `isEmpty`, `isFull` | O(1) |
| queue `display` | O(n), n = people waiting (at most 5) |
| `push`, `pop`, stack `isEmpty`, `isFull` | O(1) |

## Viva questions

1. **Why a circular queue and not a normal array queue?**
   In a normal array queue the front slots are wasted after dequeues. In a circular queue,
   `% WAIT_SIZE` wraps the index back to 0, so those slots are reused.
2. **How do you know if the queue is full or empty?**
   We keep a `count` variable: empty when `count == 0`, full when `count == WAIT_SIZE`.
3. **What happens when a booking is cancelled and someone is waiting?**
   The seat is freed, `dequeue()` gives the first waiting user, and a new booking is made
   for them on the same seat automatically.
4. **Why is a stack used for undo?**
   Undo must reverse the latest cancellation first, which is Last In, First Out.
5. **Can every undo succeed?**
   No. If the freed seat was already given to a waiting user or booked by someone else,
   the undo is refused with a message.
