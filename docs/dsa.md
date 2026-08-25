# TradeVerse — Data Structures & Algorithms Specification

This document provides mathematical complexity proofs, memory layouts, and practical justifications for every data structure and algorithm implemented in TradeVerse.

---

## 1. Binary Max-Heap & Min-Heap (`MaxHeap.hpp` & `MinHeap.hpp`)

### Purpose
- **Max-Heap**: Powers the Buy side (Bids) of the Order Book. The highest buying price is always at the root.
- **Min-Heap**: Powers the Sell side (Asks) of the Order Book. The lowest asking price is always at the root.

### Heap Invariant
For any given node $i$ in a zero-indexed vector:
- Parent: $\lfloor (i - 1) / 2 \rfloor$
- Left Child: $2i + 1$
- Right Child: $2i + 2$
- Max-Heap Property: $\text{Price}(\text{Parent}(i)) \ge \text{Price}(i)$
- Tie-Breaker: When $\text{Price}(A) = \text{Price}(B)$, the order with the earlier timestamp $\text{Timestamp}(A) < \text{Timestamp}(B)$ has higher priority.

### Complexity
- **Insertion (push)**: $O(\log N)$ comparisons and swaps via `heapifyUp`.
- **Extraction (pop)**: $O(\log N)$ comparisons and swaps via `heapifyDown`.
- **Peek (top)**: $O(1)$ root access.
- **Space Complexity**: $O(N)$ contiguous memory.

---

## 2. Price-Time Priority Matching Engine (`MatchingEngine.hpp`)

### Algorithm
1. Ingest incoming order $O_{\text{in}}$.
2. If $O_{\text{in}}.\text{side} = \text{BUY}$:
   - While $O_{\text{in}}.\text{rem} > 0$ and $\text{sellHeap.size}() > 0$ and ($O_{\text{in}}.\text{type} = \text{MARKET}$ or $O_{\text{in}}.\text{price} \ge \text{sellHeap.top}().\text{price}$):
     - $O_{\text{maker}} = \text{sellHeap.pop}()$.
     - $\Delta Q = \min(O_{\text{in}}.\text{rem}, O_{\text{maker}}.\text{rem})$.
     - Execute trade at $P = O_{\text{maker}}.\text{price}$.
     - Update filled quantities.
     - If $O_{\text{maker}}.\text{rem} > 0$, re-insert $O_{\text{maker}}$ into `sellHeap`.
3. If $O_{\text{in}}.\text{rem} > 0$ and $O_{\text{in}}.\text{type} = \text{LIMIT}$, push $O_{\text{in}}$ into `buyHeap`.

---

## 3. Weighted Graph & Dijkstra Shortest Path (`Graph.hpp`)

### Purpose
Represents market supply chains, sector peers, and contagion channels.

### Dijkstra Formulation
Given graph $G = (V, E)$ with edge weights $w(u, v) \ge 0$ representing transmission resistance:
$$\text{dist}[v] = \min_{(u, v) \in E} (\text{dist}[u] + w(u, v))$$
Using Min-Priority Queue, complexity is:
$$T = O((|V| + |E|) \log |V|)$$

### Shockwave Decay Equation
When an event hits epicentre node $S$ with base impact $I_0$:
$$I(v) = I_0 \cdot e^{-0.8 \cdot \text{dist}(S, v)}$$
Stocks with lower resistance distances absorb stronger market drift shifts.

---

## 4. Merge Sort & Quick Sort (`Sorting.hpp`)

### Merge Sort (Leaderboard)
- **Recurrence**: $T(N) = 2T(N/2) + O(N) \implies O(N \log N)$
- **Stability**: Stable — preserves relative order of participants with identical portfolio valuations.

### Quick Sort (Top Movers)
- **Recurrence**: Average $T(N) = O(N \log N)$ using Lomuto partitioning.
- **Space**: $O(\log N)$ recursion stack, in-place array partitioning.

---

## 5. Separate Chaining Hash Table (`HashMap.hpp`)

### Purpose
Constant-time resolution of ticker symbols to stock metadata and active user session authentication tokens.

### Rehashing Policy
- Initial Buckets: 16
- Max Load Factor: $\lambda_{\max} = 0.75$
- When $\lambda = N / M > 0.75$, dynamically doubles capacity ($M \leftarrow 2M$) and rehashes all elements in $O(N)$ time.
- Average Lookup / Insert / Delete: $O(1)$.