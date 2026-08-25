# TradeVerse — University Viva Defense Q&A Guide

### Q1: Where is the Data Structure in this project?
**Answer**: All core data structures are written from scratch in C++20 located in `backend/include/dsa/`:
- `MaxHeap.hpp`: Binary Max-Heap for the Buy order book.
- `MinHeap.hpp`: Binary Min-Heap for the Sell order book.
- `Queue.hpp`: FIFO Queue for incoming order buffers and BFS.
- `Stack.hpp`: LIFO Stack for action undo and DFS.
- `HashMap.hpp`: Separate chaining hash table with dynamic rehashing.
- `DoublyLinkedList.hpp`: Bidirectional list for chronological trade streams.
- `Graph.hpp`: Weighted graph with BFS, DFS, and Dijkstra shortest path.
- `Sorting.hpp`: Merge Sort (Leaderboard) and Quick Sort (Market Movers).
- `Searching.hpp`: Binary Search on sorted price arrays.

### Q2: Why did you choose a Binary Heap for the Order Book?
**Answer**: The fundamental requirement of an order matching engine is finding the best price instantaneously ($O(1)$ peek) and updating the book efficiently upon insertions and cancellations ($O(\log N)$ push/pop). Binary heaps guarantee that the highest bid and lowest ask are always at the top without sorting the entire array on every tick.

### Q3: How does your Price-Time Priority matching work?
**Answer**: We embed timestamps directly into the heap comparator:
For Buy orders:
```cpp
bool operator()(const Order& a, const Order& b) const {
    if (a.price != b.price) return a.price < b.price; // highest price first
    return a.timestamp > b.timestamp;                 // earlier time first
}
```
This ensures orders at equal price points are executed in strict FIFO order in logarithmic time without secondary sorting passes.

### Q4: How is Graph theory used in a stock trading project?
**Answer**: Real-world financial markets are deeply interconnected networks. In TradeVerse, we represent sectors (Semiconductors, Cloud, Automotive) and companies (NVDA, AAPL, MSFT) as vertices in a weighted graph. When macro events occur, Dijkstra's algorithm calculates the minimum shock resistance distance, causing price volatility and return drifts to propagate realistically across supply chains.

### Q5: Why is Merge Sort used for the Leaderboard instead of Quick Sort?
**Answer**: Merge Sort is a **stable** sorting algorithm with guaranteed $O(N \log N)$ worst-case time complexity. Stability ensures that if two users have identical portfolio net worth, their relative ranking precedence is deterministically preserved.